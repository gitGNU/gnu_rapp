/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @brief  RAPP Compute layer statistics, vector implementation.
 *
 *  IMPLEMENTATION
 *  --------------
 *  The vector operations for CNT, SUM, MAC operations are divided
 *  into an accumulate step and a reduction step. The accumulate
 *  accumulates vectorized sums, and the reduction step reduces
 *  an accumulator vector to a scalar value. This means that we
 *  can run the accumulate step for every source image vector, and
 *  reduce the result and update the scalar sums much less often.
 *
 *  The maximum number of iterations in each accumulate step is
 *  implementation-specific. Therefor we have two different
 *  implementations per function: one where the maximum iteration
 *  count is 1, suffixed with gen/GEN, and one where the count is
 *  greater than 1, suffixed with acc/ACC. Template macros are used
 *  if the template is used in more than one place.
 */

#include "rc_impl_cfg.h" /* Implementation cfg */
#include "rc_vector.h"   /* Vector API         */
#include "rc_util.h"     /* Utilities          */
#include "rc_stat.h"     /* Statistics API     */

#if defined RC_VEC_CNTN && RC_VEC_CNTN > 1 && RC_VEC_CNTN % 4 != 0
#error RC_VEC_CNTN must be divisible by 4
#endif

#if defined RC_VEC_SUMN && RC_VEC_SUMN > 1 && RC_VEC_SUMN % 4 != 0
#error RC_VEC_SUMN must be divisible by 4
#endif

#if defined RC_VEC_MACN && RC_VEC_MACN > 1 && RC_VEC_MACN % 4 != 0
#error RC_VEC_MACN must be divisible by 4
#endif

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#ifdef RC_VEC_CNTV
#define RC_STAT_ACCUM_CNT(acc1, acc2, vec) \
    RC_VEC_CNTV(acc1, vec)
#endif

#ifdef RC_VEC_SUMV
#define RC_STAT_ACCUM_SUM(acc1, acc2, vec) \
    RC_VEC_SUMV(acc1, vec)
#endif

#if defined RC_VEC_SUMV && defined RC_VEC_MACV
#define RC_STAT_ACCUM_SUM2(acc1, acc2, vec) \
do {                                        \
    RC_VEC_SUMV(acc1, vec);                 \
    RC_VEC_MACV(acc2, vec, vec);            \
} while (0)
#endif

#if defined RC_VEC_CNTR && defined RC_VEC_ZERO
#define RC_STAT_REDUCE_CNT(sum1, sum2, acc1, acc2) \
do {                                               \
    uint32_t tmp;                                  \
    RC_VEC_CNTR(tmp, acc1);                        \
    RC_VEC_ZERO(acc1);                             \
    (sum1) += tmp;                                 \
} while (0)
#endif

#if defined RC_VEC_SUMR && defined RC_VEC_ZERO
#define RC_STAT_REDUCE_SUM(sum1, sum2, acc1, acc2) \
do {                                               \
    uint32_t tmp;                                  \
    RC_VEC_SUMR(tmp, acc1);                        \
    RC_VEC_ZERO(acc1);                             \
    (sum1) += tmp;                                 \
} while (0)
#endif

#if defined RC_VEC_SUMR && defined RC_VEC_MACR && defined RC_VEC_ZERO
#define RC_STAT_REDUCE_SUM2(sum1, sum2, acc1, acc2) \
do {                                                \
    uint32_t tmp1, tmp2;                            \
    RC_VEC_SUMR(tmp1, acc1);                        \
    RC_VEC_MACR(tmp2, acc2);                        \
    RC_VEC_ZERO(acc1);                              \
    RC_VEC_ZERO(acc2);                              \
    (sum1) += tmp1;                                 \
    (sum2) += tmp2;                                 \
} while (0)

#define RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12, \
                            a1, a2, a11, a22, a12) \
do {                                               \
    uint32_t t1, t2, t11, t22, t12;                \
    RC_VEC_SUMR(t1,  a1);                          \
    RC_VEC_SUMR(t2,  a2);                          \
    RC_VEC_MACR(t11, a11);                         \
    RC_VEC_MACR(t22, a22);                         \
    RC_VEC_MACR(t12, a12);                         \
    RC_VEC_ZERO(a1);                               \
    RC_VEC_ZERO(a2);                               \
    RC_VEC_ZERO(a11);                              \
    RC_VEC_ZERO(a22);                              \
    RC_VEC_ZERO(a12);                              \
    (s1)  += t1;                                   \
    (s2)  += t2;                                   \
    (s11) += t11;                                  \
    (s22) += t22;                                  \
    (s12) += t12;                                  \
} while (0)
#endif


/**
 *  Single-operand template.
 *  The parameters @e iter and @e unroll are compile-time constants.
 */
#ifdef RC_VEC_ZERO
#define RC_STAT_TEMPLATE(buf, dim, width, height, sum1, sum2,   \
                         accum, reduce, iter, unroll)           \
do {                                                            \
    int len = RC_DIV_CEIL(width, RC_VEC_SIZE);                  \
                                                                \
    RC_VEC_DECLARE();                                           \
    if ((iter) == 1) {                                          \
        RC_STAT_TEMPLATE_GEN(buf, dim, len, height, sum1, sum2, \
                             accum, reduce, unroll);            \
    }                                                           \
    else {                                                      \
        RC_STAT_TEMPLATE_ACC(buf, dim, len, height, sum1, sum2, \
                             accum, reduce, iter, unroll);      \
    }                                                           \
    RC_VEC_CLEANUP();                                           \
} while (0)

/**
 *  Single-operand template.
 *  Results are accumulated at most @e iter times before reduction.
 */
#define RC_STAT_TEMPLATE_ACC(buf, dim, len, height, sum1, sum2, \
                             accum, reduce, iter, unroll)       \
do {                                                            \
    int full = (len) / (iter);    /* Full accums        */      \
    int rem  = (len) % (iter);    /* Partial accums     */      \
    int rem1 =  rem  / (unroll);  /* Remaining unrolled */      \
    int rem2 =  rem  % (unroll);  /* Remainder          */      \
    int y;                                                      \
    for (y = 0; y < (height); y++) {                            \
        rc_vec_t acc1, acc2;                                    \
        int      i = (y)*(dim);                                 \
        int      x;                                             \
                                                                \
        /* Handle all full accumulations */                     \
        for (x = 0; x < full; x++) {                            \
            int k;                                              \
            RC_VEC_ZERO(acc1);                                  \
            RC_VEC_ZERO(acc2);                                  \
                                                                \
            /* Accumulate */                                    \
            for (k = 0; k < (iter)/(unroll); k++) {             \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
                if ((unroll) >= 2) {                            \
                    RC_STAT_ITER(buf, i, acc1, acc2, accum);    \
                }                                               \
                if ((unroll) == 4) {                            \
                    RC_STAT_ITER(buf, i, acc1, acc2, accum);    \
                    RC_STAT_ITER(buf, i, acc1, acc2, accum);    \
                }                                               \
            }                                                   \
                                                                \
            /* Reduce */                                        \
            reduce(sum1, sum2, acc1, acc2);                     \
        }                                                       \
                                                                \
        /* Handle all partial accumulations */                  \
        RC_VEC_ZERO(acc1);                                      \
        RC_VEC_ZERO(acc2);                                      \
        for (x = 0; x < rem1; x++) {                            \
            RC_STAT_ITER(buf, i, acc1, acc2, accum);            \
            if ((unroll) >= 2) {                                \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
            }                                                   \
        }                                                       \
        reduce(sum1, sum2, acc1, acc2);                         \
                                                                \
        /* Handle the remaining vectors */                      \
        for (x = 0; x < rem2; x++) {                            \
            RC_STAT_ITER(buf, i, acc1, acc2, accum);            \
        }                                                       \
        reduce(sum1, sum2, acc1, acc2);                         \
    }                                                           \
} while (0)

/**
 *  Single-source template, no accumulation.
 */
#define RC_STAT_TEMPLATE_GEN(buf, dim, len, height, sum1, sum2, \
                             accum, reduce, unroll)             \
do {                                                            \
    int full = (len) / (unroll);                                \
    int rem  = (len) % (unroll);                                \
    int y;                                                      \
    for (y = 0; y < (height); y++) {                            \
        rc_vec_t acc1, acc2;                                    \
        int      i = (y)*(dim);                                 \
        int      x;                                             \
                                                                \
        /* Handle all full vectors */                           \
        RC_VEC_ZERO(acc1);                                      \
        RC_VEC_ZERO(acc2);                                      \
        for (x = 0; x < full; x++) {                            \
            RC_STAT_ITER(buf, i, acc1, acc2, accum);            \
            reduce(sum1, sum2, acc1, acc2);                     \
            if ((unroll) >= 2) {                                \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
                reduce(sum1, sum2, acc1, acc2);                 \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
                reduce(sum1, sum2, acc1, acc2);                 \
                RC_STAT_ITER(buf, i, acc1, acc2, accum);        \
                reduce(sum1, sum2, acc1, acc2);                 \
            }                                                   \
        }                                                       \
                                                                \
        /* Handle the remaining vectors */                      \
        for (x = 0; x < rem; x++) {                             \
            RC_STAT_ITER(buf, i, acc1, acc2, accum);            \
            reduce(sum1, sum2, acc1, acc2);                     \
        }                                                       \
    }                                                           \
} while (0)
#endif /* RC_VEC_ZERO */

/**
 *  Single-source iteration.
 */
#define RC_STAT_ITER(buf, idx, acc1, acc2, accum) \
do {                                              \
    rc_vec_t vec_;                                \
    RC_VEC_LOAD(vec_, &(buf)[idx]);               \
    accum(acc1, acc2, vec_);                      \
    (idx) += RC_VEC_SIZE;                         \
} while (0)

/**
 *  Cross sum iteration.
 */
#if defined RC_VEC_SUMV && defined RC_VEC_MACV
#define RC_STAT_ITERX(src1, src2, idx1, idx2, \
                      a1, a2, a11, a22, a12)  \
do {                                          \
    rc_vec_t v1, v2;                          \
    RC_VEC_LOAD(v1, &(src1)[idx1]);           \
    RC_VEC_LOAD(v2, &(src2)[idx2]);           \
    RC_VEC_SUMV(a1,  v1);                     \
    RC_VEC_SUMV(a2,  v2);                     \
    RC_VEC_MACV(a11, v1, v1);                 \
    RC_VEC_MACV(a22, v2, v2);                 \
    RC_VEC_MACV(a12, v1, v2);                 \
    (idx1) += RC_VEC_SIZE;                    \
    (idx2) += RC_VEC_SIZE;                    \
} while (0)
#endif

/**
 *  Find min or max in vector
 */
#define RC_STAT_MINMAX_REDUCE(vec, res, minmax)           \
do {                                                      \
    /* Get a buf_.raw[] that is correctly aligned */      \
    union { rc_vec_t v; uint8_t raw[RC_VEC_SIZE]; } buf_; \
    int i;                                                \
                                                          \
    RC_VEC_STORE(&buf_.v, vec);                           \
    res = buf_.raw[0];                                    \
    for (i = 1; i < RC_VEC_SIZE; i++) {                   \
        res = minmax(res, buf_.raw[i]);                   \
    }                                                     \
} while (0)


/**
 *  Template for the min max functions.
 */
#define RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height, vec,   \
                                res, minmax, reduce, unroll)    \
do {                                                            \
    int len  = RC_DIV_CEIL(width, RC_VEC_SIZE);                 \
    int full = (len) / (unroll);                                \
    int rem  = (len) % (unroll);                                \
    int y;                                                      \
                                                                \
    RC_VEC_DECLARE();                                           \
                                                                \
    for (y = 0; y < (height); y++) {                            \
        int i = (y)*(dim);                                      \
        int x;                                                  \
                                                                \
        /* Handle all full vectors */                           \
        for (x = 0; x < full; x++) {                            \
            RC_STAT_ITER(buf, i, vec, vec, minmax);             \
            if ((unroll) >= 2) {                                \
                RC_STAT_ITER(buf, i, vec, vec, minmax);         \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_STAT_ITER(buf, i, vec, vec, minmax);         \
                RC_STAT_ITER(buf, i, vec, vec, minmax);         \
            }                                                   \
        }                                                       \
                                                                \
        /* Handle the remaining vectors */                      \
        for (x = 0; x < rem; x++) {                             \
            RC_STAT_ITER(buf, i, vec, vec, minmax);             \
        }                                                       \
    }                                                           \
                                                                \
    /* Find min or max in the resulting vector */               \
    RC_STAT_MINMAX_REDUCE(vec, res, reduce);                    \
                                                                \
    RC_VEC_CLEANUP();                                           \
} while (0)

/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

#if defined RC_VEC_ZERO   && defined RC_VEC_SUMN && defined RC_VEC_MACN && \
    defined RC_STAT_ITERX && defined RC_STAT_REDUCE_XSUM
#define RC_STAT_xsum_acc_gen
#if RC_IMPL(rc_stat_xsum_u8, 1)
static void
rc_stat_xsum_gen(const uint8_t *restrict src1, int src1_dim,
                 const uint8_t *restrict src2, int src2_dim,
                 int len, int height, uintmax_t sum[5]);
static void
rc_stat_xsum_acc(const uint8_t *restrict src1, int src1_dim,
                 const uint8_t *restrict src2, int src2_dim,
                 int len, int height, uintmax_t sum[5]);
#endif
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary pixel sum.
 */
#if defined RC_STAT_TEMPLATE && defined RC_STAT_ACCUM_CNT && \
    defined RC_STAT_REDUCE_CNT && defined RC_VEC_CNTN
#if RC_IMPL(rc_stat_sum_bin, 1)
uint32_t
rc_stat_sum_bin(const uint8_t *buf, int dim, int width, int height)
{
    uint32_t cnt = 0;
    RC_STAT_TEMPLATE(buf, dim, RC_DIV_CEIL(width, 8), height, cnt, 0,
                     RC_STAT_ACCUM_CNT, RC_STAT_REDUCE_CNT,
                     RC_VEC_CNTN, RC_UNROLL(rc_stat_sum_bin));
    return cnt;
}
#endif
#endif


/**
 *  8-bit pixel sum.
 */
#if defined RC_STAT_TEMPLATE && defined RC_STAT_ACCUM_SUM && \
    defined RC_STAT_REDUCE_SUM && defined RC_VEC_SUMN
#if RC_IMPL(rc_stat_sum_u8, 1)
uint32_t
rc_stat_sum_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint32_t sum = 0;
    RC_STAT_TEMPLATE(buf, dim, width, height, sum, 0,
                     RC_STAT_ACCUM_SUM, RC_STAT_REDUCE_SUM,
                     RC_VEC_SUMN, RC_UNROLL(rc_stat_sum_u8));
    return sum;
}
#endif
#endif


/**
 *  8-bit pixel sum and squared sum.
 */
#if defined RC_STAT_TEMPLATE && defined RC_STAT_ACCUM_SUM2 && \
    defined RC_STAT_REDUCE_SUM2 && defined RC_VEC_SUMN && \
    defined RC_VEC_MACN
#if RC_IMPL(rc_stat_sum2_u8, 1)
void
rc_stat_sum2_u8(const uint8_t *buf, int dim,
                int width, int height, uintmax_t sum[2])
{
    uint32_t  sum1 = 0;
    uintmax_t sum2 = 0;
    RC_STAT_TEMPLATE(buf, dim, width, height, sum1, sum2,
                     RC_STAT_ACCUM_SUM2, RC_STAT_REDUCE_SUM2,
                     MIN(RC_VEC_SUMN, RC_VEC_MACN),
                     RC_UNROLL(rc_stat_sum_u8));
    sum[0] = sum1;
    sum[1] = sum2;
}
#endif
#endif


/**
 *  8-bit pixel cross sums.
 */
#ifdef RC_STAT_xsum_acc_gen
#if RC_IMPL(rc_stat_xsum_u8, 1)
void
rc_stat_xsum_u8(const uint8_t *restrict src1, int src1_dim,
                const uint8_t *restrict src2, int src2_dim,
                int width, int height, uintmax_t sum[5])
{
    int len =  RC_DIV_CEIL(width, RC_VEC_SIZE);

    if (MIN(RC_VEC_SUMN, RC_VEC_MACN) == 1) {
        rc_stat_xsum_gen(src1, src1_dim, src2, src2_dim, len, height, sum);
    }
    else {
        rc_stat_xsum_acc(src1, src1_dim, src2, src2_dim, len, height, sum);
    }
}
#endif
#endif


/**
 *  Binary pixel min.
 */
#if defined RC_VEC_AND && defined RC_VEC_SPLAT
#if RC_IMPL(rc_stat_min_bin, 1)
int
rc_stat_min_bin(const uint8_t *buf, int dim, int width, int height)
{
    rc_vec_t vec;
    uint8_t  min;

    RC_VEC_SPLAT(vec, 0xff);
    RC_STAT_MINMAX_TEMPLATE(buf, dim, RC_DIV_CEIL(width, 8),
                            height, vec, min, RC_VEC_AND, MIN,
                            RC_UNROLL(rc_stat_min_bin));
    return min == 0xff;
}
#endif
#endif


/**
 *  Binary pixel max.
 */
#if defined RC_VEC_OR && defined RC_VEC_ZERO
#if RC_IMPL(rc_stat_max_bin, 1)
int
rc_stat_max_bin(const uint8_t *buf, int dim, int width, int height)
{
    rc_vec_t vec;
    uint8_t  max;

    RC_VEC_ZERO(vec);
    RC_STAT_MINMAX_TEMPLATE(buf, dim, RC_DIV_CEIL(width, 8),
                            height, vec, max, RC_VEC_OR, MAX,
                            RC_UNROLL(rc_stat_max_bin));
    return max != 0;
}
#endif
#endif


/**
 *  8-bit pixel min.
 */
#if defined RC_VEC_MIN && defined RC_VEC_SPLAT
#if RC_IMPL(rc_stat_min_u8, 1)
int
rc_stat_min_u8(const uint8_t *buf, int dim, int width, int height)
{
    rc_vec_t vec;
    uint8_t  min;

    RC_VEC_SPLAT(vec, 0xff);
    RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height,
                            vec, min, RC_VEC_MIN, MIN,
                            RC_UNROLL(rc_stat_min_u8));
    return min;
}
#endif
#endif


/**
 * 8-bit pixel max.
 */
#if defined RC_VEC_MAX && defined RC_VEC_ZERO
#if RC_IMPL(rc_stat_max_u8, 1)
int
rc_stat_max_u8(const uint8_t *buf, int dim, int width, int height)
{
    rc_vec_t vec;
    uint8_t  max;

    RC_VEC_ZERO(vec);
    RC_STAT_MINMAX_TEMPLATE(buf, dim, width, height,
                            vec, max, RC_VEC_MAX, MAX,
                            RC_UNROLL(rc_stat_max_u8));
    return max;
}
#endif
#endif


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

#ifdef RC_STAT_xsum_acc_gen
#if RC_IMPL(rc_stat_xsum_u8, 1)
static void
rc_stat_xsum_gen(const uint8_t *restrict src1, int src1_dim,
                 const uint8_t *restrict src2, int src2_dim,
                 int len, int height, uintmax_t sum[5])
{
    uint32_t  s1  = 0;
    uint32_t  s2  = 0;
    uintmax_t s11 = 0;
    uintmax_t s22 = 0;
    uintmax_t s12 = 0;

    int full = len / RC_UNROLL(rc_stat_xsum_u8);
    int rem  = len % RC_UNROLL(rc_stat_xsum_u8);
    int y;

    RC_VEC_DECLARE();

    for (y = 0; y < height; y++) {
        rc_vec_t a1, a2, a11, a22, a12;
        int      i1 = (y)*(src1_dim);
        int      i2 = (y)*(src2_dim);
        int      x;

        RC_VEC_ZERO(a1);
        RC_VEC_ZERO(a2);
        RC_VEC_ZERO(a11);
        RC_VEC_ZERO(a22);
        RC_VEC_ZERO(a12);

        /* Handle all full vectors */
        for (x = 0; x < full; x++) {
            RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
            RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                a1, a2, a11, a22, a12);
            if (RC_UNROLL(rc_stat_xsum_u8) >= 2) {
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                    a1, a2, a11, a22, a12);
            }
            if (RC_UNROLL(rc_stat_xsum_u8) == 4) {
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                    a1, a2, a11, a22, a12);
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                    a1, a2, a11, a22, a12);
            }
        }

        /* Handle the remaining vectors */
        for (x = 0; x < rem; x++) {
            RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
            RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                a1, a2, a11, a22, a12);
        }
    }

    /* Store output */
    sum[0] = s1;
    sum[1] = s2;
    sum[2] = s11;
    sum[3] = s22;
    sum[4] = s12;

    RC_VEC_CLEANUP();
}


static void
rc_stat_xsum_acc(const uint8_t *restrict src1, int src1_dim,
                 const uint8_t *restrict src2, int src2_dim,
                 int len, int height, uintmax_t sum[5])
{
    uint32_t  s1  = 0;
    uint32_t  s2  = 0;
    uintmax_t s11 = 0;
    uintmax_t s22 = 0;
    uintmax_t s12 = 0;

    int full = len / MIN(RC_VEC_SUMN, RC_VEC_MACN); /* Full accums        */
    int rem  = len % MIN(RC_VEC_SUMN, RC_VEC_MACN); /* Partial accums     */
    int rem1 = rem / RC_UNROLL(rc_stat_xsum_u8);    /* Remaining unrolled */
    int rem2 = rem % RC_UNROLL(rc_stat_xsum_u8);    /* Remainder          */
    int y;

    RC_VEC_DECLARE();

    for (y = 0; y < height; y++) {
        rc_vec_t a1, a2, a11, a22, a12;
        int      i1 = (y)*(src1_dim);
        int      i2 = (y)*(src2_dim);
        int      x;

        RC_VEC_ZERO(a1);
        RC_VEC_ZERO(a2);
        RC_VEC_ZERO(a11);
        RC_VEC_ZERO(a22);
        RC_VEC_ZERO(a12);

        /* Handle all full accumulations */
        for (x = 0; x < full; x++) {
            int k;

            /* Accumulate */
            for (k = 0; k < MIN(RC_VEC_SUMN, RC_VEC_MACN) /
                            RC_UNROLL(rc_stat_xsum_u8); k++)
            {
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);

                if (RC_UNROLL(rc_stat_xsum_u8) >= 2) {
                    RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                }
                if (RC_UNROLL(rc_stat_xsum_u8) == 4) {
                    RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                    RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                }
            }

            /* Reduce */
            RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                                a1, a2, a11, a22, a12);
        }

        /* Handle all partial accumulations */
        for (x = 0; x < rem1; x++) {
            RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);

            if (RC_UNROLL(rc_stat_xsum_u8) >= 2) {
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
            }
            if (RC_UNROLL(rc_stat_xsum_u8) == 4) {
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
                RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
            }
        }

        /* Reduce */
        RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                            a1, a2, a11, a22, a12);

        /* Handle the remaining vectors */
        for (x = 0; x < rem2; x++) {
            RC_STAT_ITERX(src1, src2, i1, i2, a1, a2, a11, a22, a12);
        }
        RC_STAT_REDUCE_XSUM(s1, s2, s11, s22, s12,
                            a1, a2, a11, a22, a12);
    }

    /* Store output */
    sum[0] = s1;
    sum[1] = s2;
    sum[2] = s11;
    sum[3] = s22;
    sum[4] = s12;

    RC_VEC_CLEANUP();
}
#endif

#endif
