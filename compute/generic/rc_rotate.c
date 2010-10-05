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

/*
 *  @file   rc_rotate.c
 *  @brief  RAPP Compute layer 8-bit image rotation.
 */

#include "rc_impl_cfg.h" /* Implementation cfg */
#include "rc_platform.h" /* Platform-specific  */
#include "rc_rotate.h"   /* 8-bit rotation API */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Perform a CW rotation iteration.
 */
#define RC_ROTATE_CW_ITER(dst, src, dim, i, j) \
do {                                           \
    (dst)[j] = (src)[i];                       \
    (i)++, (j) += (dim);                       \
} while (0)

/**
 *  Perform a CCW rotation iteration.
 */
#define RC_ROTATE_CCW_ITER(dst, src, dim, i, j) \
do {                                            \
    (dst)[j] = (src)[i];                        \
    (i)++, (j) -= (dim);                        \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Rotate an 8-bit image 90 degrees in clockwise direction.
 */
#if RC_IMPL(rc_rotate_cw_u8, 1)
void
rc_rotate_cw_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    int len = width / RC_UNROLL(rc_rotate_cw_u8);
    int rem = width % RC_UNROLL(rc_rotate_cw_u8);
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = height - 1 - y;
        int x;

        /* Handle all unrolled iterations */
        for (x = 0; x < len; x++) {
            RC_ROTATE_CW_ITER(dst, src, dst_dim, i, j);
            if (RC_UNROLL(rc_rotate_cw_u8) >= 2) {
                RC_ROTATE_CW_ITER(dst, src, dst_dim, i, j);
            }
            if (RC_UNROLL(rc_rotate_cw_u8) == 4) {
                RC_ROTATE_CW_ITER(dst, src, dst_dim, i, j);
                RC_ROTATE_CW_ITER(dst, src, dst_dim, i, j);
            }
        }

        /* Handle the remaining pixels */
        for (x = 0; x < rem; x++) {
            RC_ROTATE_CW_ITER(dst, src, dst_dim, i, j);
        }
    }
}
#endif


/**
 *  Rotate an 8-bit image 90 degrees in counter-clockwise direction.
 */
#if RC_IMPL(rc_rotate_ccw_u8, 1)
void
rc_rotate_ccw_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int len = width / RC_UNROLL(rc_rotate_ccw_u8);
    int rem = width % RC_UNROLL(rc_rotate_ccw_u8);
    int end = (width - 1)*dst_dim;
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y + end;
        int x;

        /* Handle all unrolled iterations */
        for (x = 0; x < len; x++) {
            RC_ROTATE_CCW_ITER(dst, src, dst_dim, i, j);
            if (RC_UNROLL(rc_rotate_ccw_u8) >= 2) {
                RC_ROTATE_CCW_ITER(dst, src, dst_dim, i, j);
            }
            if (RC_UNROLL(rc_rotate_ccw_u8) == 4) {
                RC_ROTATE_CCW_ITER(dst, src, dst_dim, i, j);
                RC_ROTATE_CCW_ITER(dst, src, dst_dim, i, j);
            }
        }

        /* Handle the remaining pixels */
        for (x = 0; x < rem; x++) {
            RC_ROTATE_CCW_ITER(dst, src, dst_dim, i, j);
        }
    }
}
#endif
