/*  Copyright (C) 2005-2010, Axis Communications AB, LUND, SWEDEN
 *
 *  This file is part of RAPP.
 *
 *  RAPP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  You can use the comments under either the terms of the GNU Lesser General
 *  Public License version 3 as published by the Free Software Foundation,
 *  either version 3 of the License or (at your option) any later version, or
 *  the GNU Free Documentation License version 1.3 or any later version
 *  published by the Free Software Foundation; with no Invariant Sections, no
 *  Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the documentation section entitled
 *  "GNU Free Documentation License".
 *
 *  RAPP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License and a copy of the GNU Free Documentation License along
 *  with RAPP. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file   rc_stat.c
 *  @brief  RAPP Compute layer statistics, generic implementation.
 *
 *  IMPLEMENTATION
 *  --------------
 *  The template macros process all pixels in the image. There are
 *  two macros per operation: reduce() and update(). The first one
 *  reduces the input pixel values to the temporary statistical sums,
 *  and the second updates the image-global sums. The reason for this
 *  differentiation is that the global sums may be 64 bits, and we
 *  want to update them less often than once per pixel.
 */

#include "rc_impl_cfg.h" /* Implementation cfg     */
#include "rc_platform.h" /* Platform-specific      */
#include "rc_table.h"    /* rc_table_bitcount[]    */
#include "rc_stat.h"     /* Statistics API         */
#include "rc_util.h"     /* MIN, MAX               */
#include "rc_word.h"     /* Word operations        */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Bit count operation.
 */
#define RC_STAT_BITCOUNT(sum, arg, pix) \
    ((sum) += rc_table_bitcount[pix])

/**
 *  Sum operation.
 */
#define RC_STAT_SUM(sum, arg, pix) \
    ((sum) += (pix))

/**
 *  Squared sum operation.
 */
#define RC_STAT_SUM2(sum1, sum2, pix) \
do {                                  \
    unsigned val = (pix);             \
    (sum1) += val;                    \
    (sum2) += val*val;                \
} while (0)

/**
 *  Cross sum operation.
 */
#define RC_STAT_XSUM(s1, s2, s11, s22, s12, pix1, pix2) \
do {                                                    \
    unsigned val1 = (pix1);                             \
    unsigned val2 = (pix2);                             \
    s1  += val1;                                        \
    s2  += val2;                                        \
    s11 += val1*val1;                                   \
    s22 += val2*val2;                                   \
    s12 += val1*val2;                                   \
} while (0)

/**
 *  Sum update.
 */
#define RC_STAT_UPDATE(sum1, sum2, tmp1, tmp2) \
    ((sum1) += (tmp1))

/**
 *  Squared sum update.
 */
#define RC_STAT_UPDATE2(sum1, sum2, tmp1, tmp2) \
    ((sum1) += (tmp1), (sum2) += (tmp2))

/**
 *  Single-operand template for both sum and squared sum.
 *  The unroll value is a compile-time constant.
 */
#define RC_STAT_TEMPLATE(buf, dim, width, height, sum1, sum2, \
                         reduce, update, unroll)              \
do {                                                          \
    int len = ((width) + (unroll) - 1) / (unroll);            \
    int y;                                                    \
    for (y = 0; y < (height); y++) {                          \
        int i = y*(dim);                                      \
        int x;                                                \
        for (x = 0; x < len; x++) {                           \
            RC_STAT_ITER(sum1, sum2, buf, i,                  \
                         reduce, update, unroll);             \
        }                                                     \
    }                                                         \
} while (0)

/**
 *  Single-operand iteration.
 *  The unroll value is a compile-time constant.
 */
#define RC_STAT_ITER(sum1, sum2, buf, idx,       \
                     reduce, update, unroll)     \
do {                                             \
    unsigned s1 = 0;                             \
    uint32_t s2 = 0;                             \
    (void)s2;                                    \
    if ((unroll) == 1) {                         \
        reduce(sum1, sum2, (buf)[idx]); (idx)++; \
    }                                            \
    else if ((unroll) == 2) {                    \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        update(sum1, sum2, s1, s2);              \
    }                                            \
    else if ((unroll) == 4) {                    \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        reduce(s1, s2, (buf)[idx]); (idx)++;     \
        update(sum1, sum2, s1, s2);              \
    }                                            \
} while (0)

/**
 *  Cross sum iteration.
 *  The unroll value is a compile-time constant.
 */
#define RC_STAT_ITERX(s1, s2, s11, s22, s12,          \
                      src1, src2, idx1, idx2, unroll) \
do {                                                  \
    unsigned t1  = 0, t2  = 0;                        \
    uint32_t t11 = 0, t12 = 0, t22 = 0;               \
    if ((unroll) == 1) {                              \
        RC_STAT_XSUM(s1, s2, s11, s22, s12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
    }                                                 \
    else if ((unroll) == 2) {                         \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        (s1)  += t1;                                  \
        (s2)  += t2;                                  \
        (s11) += t11;                                 \
        (s22) += t22;                                 \
        (s12) += t12;                                 \
    }                                                 \
    else if ((unroll) == 4) {                         \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        RC_STAT_XSUM(t1, t2, t11, t22, t12,           \
                     (src1)[idx1], (src2)[idx2]);     \
        (idx1)++, (idx2)++;                           \
        (s1)  += t1;                                  \
        (s2)  += t2;                                  \
        (s11) += t11;                                 \
        (s22) += t22;                                 \
        (s12) += t12;                                 \
    }                                                 \
} while (0)

/**
 *  Min and max values for 8-bit images.
 */
#define RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height, \
                                accum, minmax, unroll)   \
do {                                                     \
    int len = ((width) + (unroll) - 1) / (unroll);       \
    int y;                                               \
                                                         \
    for (y = 0; y < (height); y++) {                     \
        int i = y*(dim);                                 \
        int x;                                           \
        for (x = 0; x < len; x++) {                      \
            (accum) = minmax(accum, buf[i]); i++;        \
            if ((unroll) >= 2) {                         \
                (accum) = minmax(accum, buf[i]); i++;    \
            }                                            \
            if ((unroll) == 4) {                         \
                (accum) = minmax(accum, buf[i]); i++;    \
                (accum) = minmax(accum, buf[i]); i++;    \
            }                                            \
        }                                                \
    }                                                    \
} while (0)

/**
 *  Word-based template - min and max values for binary images.
 */
#define RC_STAT_WORD_TEMPLATE(buf, dim, width, height,   \
                              accum, bitop, unroll)      \
do {                                                     \
    int tot = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);        \
    int len = tot / (unroll);                            \
    int rem = tot % (unroll);                            \
    int y;                                               \
    for (y = 0; y < (height); y++) {                     \
        int i = y*(dim);                                 \
        int x;                                           \
                                                         \
        /* Process all unrolled words */                 \
        for (x = 0; x < len; x++) {                      \
            RC_STAT_WORD_ITER(accum, buf, i, bitop);     \
            if ((unroll) >= 2) {                         \
                RC_STAT_WORD_ITER(accum, buf, i, bitop); \
            }                                            \
            if ((unroll) == 4) {                         \
                RC_STAT_WORD_ITER(accum, buf, i, bitop); \
                RC_STAT_WORD_ITER(accum, buf, i, bitop); \
            }                                            \
        }                                                \
                                                         \
        /* Process all remaining words */                \
        for (x = 0; x < rem; x++) {                      \
            RC_STAT_WORD_ITER(accum, buf, i, bitop);     \
        }                                                \
    }                                                    \
} while (0)

/**
 *  An iteration for the word template.
 */
#define RC_STAT_WORD_ITER(acc, buf, i, bitop) \
do {                                          \
    rc_word_t w = RC_WORD_LOAD(&(buf)[i]);    \
    (acc) = (acc) bitop  w;                   \
    (i) += RC_WORD_SIZE;                      \
} while (0)




/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary pixel sum.
 */
#if RC_IMPL(rc_stat_sum_bin, 1)
uint32_t
rc_stat_sum_bin(const uint8_t *buf, int dim, int width, int height)
{
    uint32_t sum = 0;
    RC_STAT_TEMPLATE(buf, dim, (width + 7) / 8, height, sum, 0,
                     RC_STAT_BITCOUNT, RC_STAT_UPDATE,
                     RC_UNROLL(rc_stat_sum_bin));
    return sum;
}
#endif


/**
 *  8-bit pixel sum.
 */
#if RC_IMPL(rc_stat_sum_u8, 1)
uint32_t
rc_stat_sum_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint32_t sum = 0;
    RC_STAT_TEMPLATE(buf, dim, width, height, sum, 0,
                     RC_STAT_SUM, RC_STAT_UPDATE,
                     RC_UNROLL(rc_stat_sum_u8));
    return sum;
}
#endif


/**
 *  8-bit pixel sum and squared sum.
 */
#if RC_IMPL(rc_stat_sum2_u8, 1)
void
rc_stat_sum2_u8(const uint8_t *buf, int dim,
                int width, int height, uintmax_t sum[2])
{
    uint32_t  sum1 = 0;
    uintmax_t sum2 = 0;
    RC_STAT_TEMPLATE(buf, dim, width, height, sum1, sum2,
                     RC_STAT_SUM2, RC_STAT_UPDATE2,
                     RC_UNROLL(rc_stat_sum2_u8));
    sum[0] = sum1;
    sum[1] = sum2;
}
#endif


/**
 *  8-bit pixel cross sums.
 */
#if RC_IMPL(rc_stat_xsum_u8, 1)
void
rc_stat_xsum_u8(const uint8_t *restrict src1, int src1_dim,
                const uint8_t *restrict src2, int src2_dim,
                int width, int height, uintmax_t sum[5])
{
    uint32_t  s2  = 0;
    uint32_t  s1  = 0;
    uintmax_t s11 = 0;
    uintmax_t s22 = 0;
    uintmax_t s12 = 0;

    int len = (width + RC_UNROLL(rc_stat_xsum_u8) - 1) /
               RC_UNROLL(rc_stat_xsum_u8);
    int y;

    for (y = 0; y < height; y++) {
        int i1 = y*src1_dim;
        int i2 = y*src2_dim;
        int x;

        for (x = 0; x < len; x++) {
            RC_STAT_ITERX(s1, s2, s11, s22, s12,
                          src1, src2, i1, i2,
                          RC_UNROLL(rc_stat_xsum_u8));
        }
    }

    sum[0] = s1;
    sum[1] = s2;
    sum[2] = s11;
    sum[3] = s22;
    sum[4] = s12;
}
#endif


/**
 *  Binary pixel min.
 */
#if RC_IMPL(rc_stat_min_bin, 1)
int
rc_stat_min_bin(const uint8_t *buf, int dim, int width, int height)
{
    rc_word_t acc = RC_WORD_ONE;
    RC_STAT_WORD_TEMPLATE(buf, dim, width, height, acc,
                          &, RC_UNROLL(rc_stat_min_bin));
    return acc == RC_WORD_ONE;
}
#endif


/**
 *  Binary pixel max.
 */
#if RC_IMPL(rc_stat_max_bin, 1)
int
rc_stat_max_bin(const uint8_t *buf, int dim, int width, int height)
{
    rc_word_t acc = 0;
    RC_STAT_WORD_TEMPLATE(buf, dim, width, height, acc,
                          |, RC_UNROLL(rc_stat_max_bin));
    return acc != 0;
}
#endif
#ifdef RAPP_FORCE_EXPORT
/* This one is used when tuning, for all internal references. */
int
rc_stat_max_bin__internal(const uint8_t *buf, int dim, int width, int height)
{
    rc_word_t acc = 0;
    RC_STAT_WORD_TEMPLATE(buf, dim, width, height, acc,
                          |, 1);
    return acc != 0;
}
#endif


/**
 *  8-bit pixel min.
 */
#if RC_IMPL(rc_stat_min_u8, 1)
int
rc_stat_min_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint8_t min = UINT8_MAX;
    RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height,
                            min, MIN, RC_UNROLL(rc_stat_min_u8));
    return min;
}
#endif

/**
 *  8-bit pixel max.
 */
#if RC_IMPL(rc_stat_max_u8, 1)
int
rc_stat_max_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint8_t max = 0;
    RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height,
                            max, MAX, RC_UNROLL(rc_stat_max_u8));
    return max;
}
#endif
