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
 *  @file   rc_filter.c
 *  @brief  RAPP Compute layer fixed filters, generic implementation.
 *
 *  ALGORITHM
 *  ---------
 *  We use template macros for the common code, and pass operation
 *  macros as arguments to the template. Filters are separated into a
 *  vertical and a horizontal component if possible. The inner loops are
 *  unrolled to the horizontal period of the filter (2 for *x2 and 3
 *  for 3x3). This enables us to store the intermediate result in
 *  temporary variables instead of arrays. The 1x2 and 2x2 filters may
 *  be further unrolled up to four times.
 *
 *  The final filter output is correctly rounded.
 */

#include <stdlib.h>      /* abs()              */
#include "rc_impl_cfg.h" /* Implementation cfg */
#include "rc_util.h"     /* MAX()              */
#include "rc_filter.h"   /* Fixed-filter API   */

/*
 * -------------------------------------------------------------
 *  Support macros
 * -------------------------------------------------------------
 */

/**
 *  No modifier (unity).
 */
#define RC_FILTER_MOD_NONE(val) \
    (val)

/**
 *  Absolute-value modifier.
 */
#define RC_FILTER_MOD_ABS(val) \
    abs(val)

/**
 *  Output mapping transform.
 */
#define RC_FILTER_MAP(sum, add, mul, shift) \
    ((mul)*((sum) + (add)) >> (shift))


/*
 * -------------------------------------------------------------
 *  2x2 operation macros
 * -------------------------------------------------------------
 */

/**
 *  Difference operation.
 */
#define RC_FILTER_OP_DIFF(dst, src1, src2) \
    ((dst) = ((src2) - (src1) + 0x100) >> 1)

/**
 *  Absolute-value difference operation.
 */
#define RC_FILTER_OP_DIFF_ABS(dst, src1, src2) \
    ((dst) = abs((src2) - (src1)))


/*
 * -------------------------------------------------------------
 *  2x2 templates
 * -------------------------------------------------------------
 */

/**
 *  The 1x2 convolution template.
 *  The unroll factor is a compile-time constant.
 */
#define RC_FILTER_1X2(dst, dst_dim, src, src_dim, width, height, op, unroll) \
do {                                                                         \
    int len = (width) / MAX(2, unroll);                                      \
    int rem = (width) % MAX(2, unroll);                                      \
    int y;                                                                   \
    for (y = 0; y < (height); y++) {                                         \
        int j  = y*(dst_dim);     /* Destination row index    */             \
        int i  = y*(src_dim);     /* Current source row index */             \
        int s1 = (src)[i - 1];                                               \
        int s2, x;                                                           \
        /* Handle all full periods */                                        \
        for (x = 0; x < len; x++) {                                          \
            RC_FILTER_1X2_ITER(dst, src, j, i, s1, s2, op);                  \
            RC_FILTER_1X2_ITER(dst, src, j, i, s2, s1, op);                  \
            if ((unroll) == 4) {                                             \
                RC_FILTER_1X2_ITER(dst, src, j, i, s1, s2, op);              \
                RC_FILTER_1X2_ITER(dst, src, j, i, s2, s1, op);              \
            }                                                                \
        }                                                                    \
        /* Handle remaining pixels */                                        \
        if (rem > 1) {                                                       \
            RC_FILTER_1X2_ITER(dst, src, j, i, s1, s2, op);                  \
            RC_FILTER_1X2_ITER(dst, src, j, i, s2, s1, op);                  \
        }                                                                    \
        if (rem & 1) {                                                       \
            RC_FILTER_1X2_ITER(dst, src, j, i, s1, s2, op);                  \
        }                                                                    \
    }                                                                        \
} while (0)

/**
 *  A 1x2 filterolution iteration.
 */
#define RC_FILTER_1X2_ITER(dst, src, j, i, s1, s2, op) \
do {                                                   \
    (s2) = (src)[i];                                   \
    op((dst)[j], s1, s2);                              \
    (j)++, (i)++;                                      \
} while (0)

/**
 *  The 2x1 convolution template.
 *  The unroll factor is a compile-time constant.
 */
#define RC_FILTER_2X1(dst, dst_dim, src, src_dim, width, height, op, unroll) \
do {                                                                         \
    int len = (width) / (unroll);                                            \
    int rem = (width) % (unroll);                                            \
    int y;                                                                   \
    for (y = 0; y < (height); y++) {                                         \
        int j  = y*(dst_dim);     /* Destination row index     */            \
        int i2 = y*(src_dim);     /* Current source row index  */            \
        int i1 = i2 - (src_dim);  /* Previous source row index */            \
        int x;                                                               \
        /* Handle all full periods */                                        \
        for (x = 0; x < len; x++) {                                          \
            RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op);                     \
            if ((unroll) >= 2) {                                             \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op);                 \
            }                                                                \
            if ((unroll) == 4) {                                             \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op);                 \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op);                 \
            }                                                                \
        }                                                                    \
        /* Handle remaining pixels */                                        \
        for (x = 0; x < rem; x++) {                                          \
            RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op);                     \
        }                                                                    \
    }                                                                        \
} while (0)

/**
 *  A 2x1 convolution iteration.
 */
#define RC_FILTER_2X1_ITER(dst, src, j, i1, i2, op) \
do {                                                \
    op((dst)[j], (src)[i1], (src)[i2]);             \
    (j)++, (i1)++, (i2)++;                          \
} while (0)

/**
 *  A 2x2 difference magnitude iteration step.
 */
#define RC_FILTER_2X2_ITER(dst, src, j, i1, i2, \
                           s11, s12, s21, s22)  \
do {                                            \
    (s12) = (src)[i1];                          \
    (s22) = (src)[i2];                          \
    (dst)[j] = (abs((s21) - (s22)) +            \
                abs((s12) - (s22)) + 1) >> 1;   \
    (j)++, (i1)++, (i2)++;                      \
} while (0)


/*
 * -------------------------------------------------------------
 *  3x3 operation macros
 * -------------------------------------------------------------
 */

/**
 *  3-point symmetric difference.
 */
#define RC_FILTER_SEP_DIFF(dst, s1, s2, s3) \
    ((dst) = (s1) - (s3))

/**
 *  3-point gaussian.
 */
#define RC_FILTER_SEP_GAUSS(dst, s1, s2, s3) \
    ((dst) = (s1) + 2*(s2) + (s3))

/**
 *  3x3 laplacian mask.
 */
#define RC_FILTER_GEN_LAPLACE(dst, s11, s12, s13, s21, \
                              s22, s23, s31, s32, s33) \
    ((dst) = 4*(s22) - ((s12) + (s21) + (s23) + (s32)))

/**
 *  3x3 highpass mask.
 */
#define RC_FILTER_GEN_HIGHPASS(dst, s11, s12, s13, s21, \
                               s22, s23, s31, s32, s33) \
    ((dst) = 8*(s22) - ((s11) + (s12) + (s13) +         \
                        (s21) +         (s23) +         \
                        (s31) + (s32) + (s33)))

/*
 * -------------------------------------------------------------
 *  3x3 template macros
 * -------------------------------------------------------------
 */

/**
 *  The 3x3 separable convolution template.
 */
#define RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,    \
                          horz, vert, mod, add, mul, shift)             \
do {                                                                    \
    int len = (width) / 3;                                              \
    int rem = (width) % 3;                                              \
    int y;                                                              \
    for (y = 0; y < (height); y++) {                                    \
        int j  = y*(dst_dim);     /* Destination row index    */        \
        int i2 = y*(src_dim) + 1; /* Current source row index */        \
        int i1 = i2 - (src_dim);  /* Above source row index   */        \
        int i3 = i2 + (src_dim);  /* Below souce row index    */        \
        int v1, v2, v3, x;                                              \
        vert(v1, (src)[i3 - 2], (src)[i2 - 2], (src)[i1 - 2]);          \
        vert(v2, (src)[i3 - 1], (src)[i2 - 1], (src)[i1 - 1]);          \
        v3 = 0;                                                         \
        /* Handle all full 3-cycles */                                  \
        for (x = 0; x < len; x++) {                                     \
            RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v1, v2, v3, \
                                 horz, vert, mod, add, mul, shift);     \
            RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v2, v3, v1, \
                                 horz, vert, mod, add, mul, shift);     \
            RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v3, v1, v2, \
                                 horz, vert, mod, add, mul, shift);     \
        }                                                               \
        /* Handle remaining pixels */                                   \
        if (rem > 0) {                                                  \
            RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v1, v2, v3, \
                                 horz, vert, mod, add, mul, shift);     \
        }                                                               \
        if (rem > 1) {                                                  \
            RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v2, v3, v1, \
                                 horz, vert, mod, add, mul, shift);     \
        }                                                               \
    }                                                                   \
} while (0)

/**
 *  A 3x3 separable convolution iteration.
 */
#define RC_FILTER_3X3_SEP_ITER(dst, src, j, i1, i2, i3, v1, v2, v3, \
                               horz, vert, mod, add, mul, shift)    \
do {                                                                \
    int sum;                                                        \
    vert(v3, (src)[i3], (src)[i2], (src)[i1]);                      \
    horz(sum, v3, v2, v1);                                          \
    (dst)[j] = RC_FILTER_MAP(mod(sum), add, mul, shift);            \
    (j)++, (i1)++, (i2)++, (i3)++;                                  \
} while (0)

/**
 *  The 3x3 separable convolution magnitude template.
 */
#define RC_FILTER_3X3_SEP_MAGN(dst, dst_dim, src, src_dim, width, height, \
                               horz, vert, add, mul, shift)               \
do {                                                                      \
    int len = (width) / 3;                                                \
    int rem = (width) % 3;                                                \
    int y;                                                                \
    for (y = 0; y < (height); y++) {                                      \
        int j  = y*(dst_dim);     /* Destination row index    */          \
        int i2 = y*(src_dim) + 1; /* Current source row index */          \
        int i1 = i2 - (src_dim);  /* Above source row index   */          \
        int i3 = i2 + (src_dim);  /* Below souce row index    */          \
        int v1, v2, v3;                                                   \
        int h1, h2, h3, x;                                                \
        vert(v1, (src)[i3 - 2], (src)[i2 - 2], (src)[i1 - 2]);            \
        vert(v2, (src)[i3 - 1], (src)[i2 - 1], (src)[i1 - 1]);            \
        horz(h1, (src)[i3 - 2], (src)[i2 - 2], (src)[i1 - 2]);            \
        horz(h2, (src)[i3 - 1], (src)[i2 - 1], (src)[i1 - 1]);            \
        v3 = 0, h3 = 0;                                                   \
        /* Handle all full 3-cycles */                                    \
        for (x = 0; x < len; x++) {                                       \
            RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                        v1, v2, v3, h1, h2, h3,           \
                                        horz, vert, add, mul, shift);     \
            RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                        v2, v3, v1, h2, h3, h1,           \
                                        horz, vert, add, mul, shift);     \
            RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                        v3, v1, v2, h3, h1, h2,           \
                                        horz, vert, add, mul, shift);     \
        }                                                                 \
        /* Handle remaining pixels */                                     \
        if (rem > 0) {                                                    \
            RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                        v1, v2, v3, h1, h2, h3,           \
                                        horz, vert, add, mul, shift);     \
        }                                                                 \
        if (rem > 1) {                                                    \
            RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                        v2, v3, v1, h2, h3, h1,           \
                                        horz, vert, add, mul, shift);     \
        }                                                                 \
    }                                                                     \
} while (0)

/**
 *  A 3x3 separable convolution iteration.
 */
#define RC_FILTER_3X3_SEP_MAGN_ITER(dst, src, j, i1, i2, i3,          \
                                    v1, v2, v3, h1, h2, h3,           \
                                    horz, vert, add, mul, shift)      \
do {                                                                  \
    int vsum, hsum;                                                   \
    vert(v3, (src)[i3], (src)[i2], (src)[i1]);                        \
    horz(h3, (src)[i3], (src)[i2], (src)[i1]);                        \
    horz(hsum, v3, v2, v1);                                           \
    vert(vsum, h3, h2, h1);                                           \
    (dst)[j] = RC_FILTER_MAP(abs(vsum) + abs(hsum), add, mul, shift); \
    (j)++, (i1)++, (i2)++, (i3)++;                                    \
} while (0)

/**
 *  The 3x3 generic convolution template.
 */
#define RC_FILTER_3X3_GEN(dst, dst_dim, src, src_dim, width, height, \
                          mask, mod, add, mul, shift)                \
do {                                                                 \
    int len = (width) / 3;                                           \
    int rem = (width) % 3;                                           \
    int y;                                                           \
    for (y = 0; y < (height); y++) {                                 \
        int j  = y*(dst_dim);     /* Destination row index    */     \
        int i2 = y*(src_dim) + 1; /* Current source row index */     \
        int i1 = i2 - (src_dim);  /* Above source row index   */     \
        int i3 = i2 + (src_dim);  /* Below souce row index    */     \
        int s11 = (src)[i1 - 2];                                     \
        int s12 = (src)[i1 - 1];                                     \
        int s21 = (src)[i2 - 2];                                     \
        int s22 = (src)[i2 - 1];                                     \
        int s31 = (src)[i3 - 2];                                     \
        int s32 = (src)[i3 - 1];                                     \
        int s13 = 0, s23 = 0, s33 = 0;                               \
        int x;                                                       \
        /* Handle all full 3-cycles */                               \
        for (x = 0; x < len; x++) {                                  \
            RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,          \
                                   s11, s12, s13,                    \
                                   s21, s22, s23,                    \
                                   s31, s32, s33,                    \
                                   mask, mod, add, mul, shift);      \
            RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,          \
                                   s12, s13, s11,                    \
                                   s22, s23, s21,                    \
                                   s32, s33, s31,                    \
                                   mask, mod, add, mul, shift);      \
            RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,          \
                                   s13, s11, s12,                    \
                                   s23, s21, s22,                    \
                                   s33, s31, s32,                    \
                                   mask, mod, add, mul, shift);      \
        }                                                            \
        /* Handle remaining pixels */                                \
        if (rem > 0) {                                               \
            RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,          \
                                   s11, s12, s13,                    \
                                   s21, s22, s23,                    \
                                   s31, s32, s33,                    \
                                   mask, mod, add, mul, shift);      \
        }                                                            \
        if (rem > 1) {                                               \
            RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,          \
                                   s12, s13, s11,                    \
                                   s22, s23, s21,                    \
                                   s32, s33, s31,                    \
                                   mask, mod, add, mul, shift);      \
        }                                                            \
    }                                                                \
} while (0)

/**
 *  A 3x3 generic convolution iteration.
 */
#define RC_FILTER_3X3_GEN_ITER(dst, src, j, i1, i2, i3,                     \
                               s11, s12, s13, s21, s22, s23, s31, s32, s33, \
                               mask, mod, add, mul, shift)                  \
do {                                                                        \
    int sum;                                                                \
    (s13) = (src)[i1];                                                      \
    (s23) = (src)[i2];                                                      \
    (s33) = (src)[i3];                                                      \
    mask(sum, s11, s12, s13, s21, s22, s23, s31, s32, s33);                 \
    (dst)[j] = RC_FILTER_MAP(mod(sum), add, mul, shift);                    \
    (j)++, (i1)++, (i2)++, (i3)++;                                          \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  1x2 horizontal difference.
 */
#if RC_IMPL(rc_filter_diff_1x2_horz_u8, 1)
void
rc_filter_diff_1x2_horz_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    RC_FILTER_1X2(dst, dst_dim, src, src_dim, width, height,
                  RC_FILTER_OP_DIFF, RC_UNROLL(rc_filter_diff_1x2_horz_u8));
}
#endif

/**
 *  1x2 horizontal difference, absolute value.
 */
#if RC_IMPL(rc_filter_diff_1x2_horz_abs_u8, 1)
void
rc_filter_diff_1x2_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height)
{
    RC_FILTER_1X2(dst, dst_dim, src, src_dim, width, height,
                  RC_FILTER_OP_DIFF_ABS,
                  RC_UNROLL(rc_filter_diff_1x2_horz_abs_u8));
}
#endif

/**
 *  2x1 vertical difference.
 */
#if RC_IMPL(rc_filter_diff_2x1_vert_u8, 1)
void
rc_filter_diff_2x1_vert_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    RC_FILTER_2X1(dst, dst_dim, src, src_dim, width, height,
                  RC_FILTER_OP_DIFF, RC_UNROLL(rc_filter_diff_2x1_vert_u8));
}
#endif

/**
 *  2x1 vertical difference, absolute value.
 */
#if RC_IMPL(rc_filter_diff_2x1_vert_abs_u8, 1)
void
rc_filter_diff_2x1_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height)
{
    RC_FILTER_2X1(dst, dst_dim, src, src_dim, width, height,
                  RC_FILTER_OP_DIFF_ABS,
                  RC_UNROLL(rc_filter_diff_2x1_vert_abs_u8));
}
#endif

/**
 *  2x2 difference magnitude.
 */
#if RC_IMPL(rc_filter_diff_2x2_magn_u8, 1)
void
rc_filter_diff_2x2_magn_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    int len = width / MAX(2, RC_UNROLL(rc_filter_diff_2x2_magn_u8));
    int rem = width % MAX(2, RC_UNROLL(rc_filter_diff_2x2_magn_u8));
    int y;

    for (y = 0; y < height; y++) {
        int s11, s12;
        int s21, s22;
        int j  = y*dst_dim;
        int i2 = y*src_dim;
        int i1 = i2 - src_dim;
        int x;

        /* Load the first column */
        s11 = src[i1 - 1];
        s21 = src[i2 - 1];

        /* Handle all full periods */
        for (x = 0; x < len; x++) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s11, s12, s21, s22);
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s12, s11, s22, s21);

            if (RC_UNROLL(rc_filter_diff_2x2_magn_u8) == 4) {
                 RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s11, s12, s21, s22);
                 RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s12, s11, s22, s21);
            }
        }

        /* Handle the partial period */
        if (rem > 1) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s11, s12, s21, s22);
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s12, s11, s22, s21);
        }
        if (rem & 1) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, s11, s12, s21, s22);
        }
    }
}
#endif

/**
 *  3x3 horizontal Sobel gradient.
 */
#if RC_IMPL(rc_filter_sobel_3x3_horz_u8, 0)
void
rc_filter_sobel_3x3_horz_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_SEP_DIFF, RC_FILTER_SEP_GAUSS,
                      RC_FILTER_MOD_NONE, 4*(0x100 + 1), 1, 3);
}
#endif

/**
 *  3x3 horizontal Sobel gradient, absolute value.
 */
#if RC_IMPL(rc_filter_sobel_3x3_horz_abs_u8, 0)
void
rc_filter_sobel_3x3_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height)
{
    RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_SEP_DIFF, RC_FILTER_SEP_GAUSS,
                      RC_FILTER_MOD_ABS, 2, 1, 2);
}
#endif

/**
 *  3x3 vertical Sobel gradient.
 */
#if RC_IMPL(rc_filter_sobel_3x3_vert_u8, 0)
void
rc_filter_sobel_3x3_vert_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_SEP_GAUSS, RC_FILTER_SEP_DIFF,
                      RC_FILTER_MOD_NONE, 4*(0x100 + 1), 1, 3);
}
#endif

/**
 *  3x3 vertical Sobel gradient, absolute value.
 */
#if RC_IMPL(rc_filter_sobel_3x3_vert_abs_u8, 0)
void
rc_filter_sobel_3x3_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height)
{
    RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_SEP_GAUSS, RC_FILTER_SEP_DIFF,
                      RC_FILTER_MOD_ABS, 2, 1, 2);
}
#endif

/**
 *  3x3 Sobel gradient magnitude.
 */
#if RC_IMPL(rc_filter_sobel_3x3_magn_u8, 0)
void
rc_filter_sobel_3x3_magn_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_SEP_MAGN(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SEP_GAUSS, RC_FILTER_SEP_DIFF, 4, 1, 3);
}
#endif

/**
 *  3x3 gaussian.
 */
#if RC_IMPL(rc_filter_gauss_3x3_u8, 0)
void
rc_filter_gauss_3x3_u8(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_FILTER_3X3_SEP(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_SEP_GAUSS, RC_FILTER_SEP_GAUSS,
                      RC_FILTER_MOD_NONE, 8, 1, 4);
}
#endif

/**
 *  3x3 laplacian.
 */
#if RC_IMPL(rc_filter_laplace_3x3_u8, 0)
void
rc_filter_laplace_3x3_u8(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height)
{
    RC_FILTER_3X3_GEN(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_GEN_LAPLACE, RC_FILTER_MOD_NONE,
                      4*(0x100 + 1), 1, 3);
}
#endif

/**
 *  3x3 laplacian, absolute value.
 */
#if RC_IMPL(rc_filter_laplace_3x3_abs_u8, 0)
void
rc_filter_laplace_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height)
{
    RC_FILTER_3X3_GEN(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_GEN_LAPLACE, RC_FILTER_MOD_ABS, 2, 1, 2);
}
#endif

/**
 *  3x3 highpass filter.
 */
#if RC_IMPL(rc_filter_highpass_3x3_u8, 0)
void
rc_filter_highpass_3x3_u8(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height)
{
    RC_FILTER_3X3_GEN(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_GEN_HIGHPASS, RC_FILTER_MOD_NONE,
                      8*(0x100 + 1), 1, 4);
}
#endif

/**
 *  3x3 highpass filter, absolute value.
 */
#if RC_IMPL(rc_filter_highpass_3x3_abs_u8, 0)
void
rc_filter_highpass_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height)
{
    RC_FILTER_3X3_GEN(dst, dst_dim, src, src_dim, width, height,
                      RC_FILTER_GEN_HIGHPASS, RC_FILTER_MOD_ABS, 4, 1, 3);
}
#endif
