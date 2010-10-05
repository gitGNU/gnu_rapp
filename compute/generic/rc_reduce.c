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
 *  @file   rc_reduce.c
 *  @brief  RAPP Compute layer 8-bit 2x reductions, generic implementation.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_reduce.h"   /* 8-bit reduction API   */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  1x2 reduction iteration.
 */
#define RC_REDUCE_1X2_ITER(dst, src, i, j) \
do {                                       \
    unsigned p1 = (src)[(i)++];            \
    unsigned p2 = (src)[(i)++];            \
    (dst)[j] = (p1 + p2 + 1) >> 1;         \
    (j)++;                                 \
} while (0)

/**
 *  2x1 reduction iteration.
 */
#define RC_REDUCE_2X1_ITER(dst, src, i1, i2, j) \
do {                                            \
    unsigned p1 = (src)[(i1)++];                \
    unsigned p2 = (src)[(i2)++];                \
    (dst)[j] = (p1 + p2 + 1) >> 1;              \
    (j)++;                                      \
} while (0)

/**
 *  2x2 reduction iteration.
 */
#define RC_REDUCE_2X2_ITER(dst, src, i1, i2, j)  \
do {                                             \
    unsigned p11 = (src)[(i1)++];                \
    unsigned p12 = (src)[(i1)++];                \
    unsigned p21 = (src)[(i2)++];                \
    unsigned p22 = (src)[(i2)++];                \
    (dst)[j] = (p11 + p12 + p21 + p22 + 2) >> 2; \
    (j)++;                                       \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  8-bit 1x2 (horizontal) reduction.
 */
#if RC_IMPL(rc_reduce_1x2_u8, 1)
void
rc_reduce_1x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int w2   = width / 2;
    int full = w2 / RC_UNROLL(rc_reduce_1x2_u8);
    int rem  = w2 % RC_UNROLL(rc_reduce_1x2_u8);
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all unrolled iterations */
        for (x = 0; x < full; x++) {
            RC_REDUCE_1X2_ITER(dst, src, i, j);
            if (RC_UNROLL(rc_reduce_1x2_u8) >= 2) {
                RC_REDUCE_1X2_ITER(dst, src, i, j);
            }
            if (RC_UNROLL(rc_reduce_1x2_u8) == 4) {
                RC_REDUCE_1X2_ITER(dst, src, i, j);
                RC_REDUCE_1X2_ITER(dst, src, i, j);
            }
        }

        /* Handle remaining iterations */
        for (x = 0; x < rem; x++) {
            RC_REDUCE_1X2_ITER(dst, src, i, j);
        }
    }
}
#endif


/**
 *  8-bit 2x1 (vertical) reduction.
 */
#if RC_IMPL(rc_reduce_2x1_u8, 1)
void
rc_reduce_2x1_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int h2  = height / 2;
    int len = RC_DIV_CEIL(width, RC_UNROLL(rc_reduce_2x1_u8));
    int y;

    for (y = 0; y < h2; y++) {
        int i1 = 2*y*src_dim;
        int i2 = i1 + src_dim;
        int j  = y*dst_dim;
        int x;

        /* Handle the unrolled iterations */
        for (x = 0; x < len; x++) {
            RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            if (RC_UNROLL(rc_reduce_2x1_u8) >= 2) {
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            }
            if (RC_UNROLL(rc_reduce_2x1_u8) == 4) {
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            }
        }
    }
}
#endif


/**
 *  8-bit 2x2 reduction.
 */
#if RC_IMPL(rc_reduce_2x2_u8, 1)
void
rc_reduce_2x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int w2   = width  / 2;
    int h2   = height / 2;
    int full = w2 / RC_UNROLL(rc_reduce_2x2_u8);
    int rem  = w2 % RC_UNROLL(rc_reduce_2x2_u8);
    int y;

    for (y = 0; y < h2; y++) {
        int i1 = 2*y*src_dim;
        int i2 = i1 + src_dim;
        int j  = y*dst_dim;
        int x;

        /* Handle all unrolled iterations */
        for (x = 0; x < full; x++) {
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            if (RC_UNROLL(rc_reduce_2x2_u8) >= 2) {
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            }
            if (RC_UNROLL(rc_reduce_2x2_u8) == 4) {
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            }
        }

        /* Handle remaining iterations */
        for (x = 0; x < rem; x++) {
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
        }
    }
}
#endif
