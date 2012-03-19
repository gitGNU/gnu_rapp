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
 *  @file   rc_type.c
 *  @brief  RAPP Compute layer type conversions, vector implementation.
 */

#include "rc_impl_cfg.h"   /* Implementation config */
#include "rc_vector.h"     /* Vector operations     */
#include "rc_thresh_tpl.h" /* Thresholding template */
#include "rc_type.h"       /* Type conversion API   */


/*
 * -------------------------------------------------------------
 *  Local macros
 * -------------------------------------------------------------
 */

/**
 *  Conversion u8 to binary macro for thresholding template.
 */
#define RC_TYPE_U8_TO_BIN(dstv, srcv, arg1, arg2) \
    ((dstv) = (srcv))

/**
 *  Iteration part of converting a binary vector at src into u8 data at
 *  dst.
 */
#if defined RC_VEC_SETMASKV && defined RC_VEC_SHLC
#define RC_TYPE_BIN_TO_U8_ITER_MAX(max, dst, src, j, i, arg1, arg2)     \
do {                                                                    \
    rc_vec_t sv_;                                                       \
    int k;                                                              \
    RC_VEC_LOAD(sv_, &(src)[i]);                                        \
    for (k = 0; k < max; k++, j += RC_VEC_SIZE) {                       \
        rc_vec_t dv_, tv_;                                              \
        RC_VEC_SETMASKV(dv_, sv_);                                      \
        RC_VEC_STORE(&(dst)[j], dv_);                                   \
        RC_VEC_SHLC(tv_, sv_, RC_VEC_SIZE / 8);                         \
        sv_ = tv_;                                                      \
    }                                                                   \
    (i) += RC_VEC_SIZE;                                                 \
} while (0)
#define RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2) \
    RC_TYPE_BIN_TO_U8_ITER_MAX(8, dst, src, j, i, arg1, arg2)
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Conversion u8 to binary.
 */
#if    RC_IMPL(rc_type_u8_to_bin, 1)
#ifdef RC_THRESH_TEMPLATE
void
rc_type_u8_to_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, 0, 0, RC_TYPE_U8_TO_BIN,
                       RC_UNROLL(rc_type_u8_to_bin));
}
#endif
#endif


/**
 *  Conversion binary to u8.
 */
#if RC_IMPL(rc_type_bin_to_u8, 1)
#ifdef RC_TYPE_BIN_TO_U8_ITER
void
rc_type_bin_to_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    /* We use the total number of destination vectors as the base. */
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE * 8 / 8);

    /* We count whole source vectors for unrolling. */
    int len = tot / (8 * RC_UNROLL(rc_type_bin_to_u8));
    int rem = tot % (8 * RC_UNROLL(rc_type_bin_to_u8));
    int y;

    RC_VEC_DECLARE();

    /* Process all rows. */
    for (y = 0; y < height; y++) {
        int i = y * src_dim;
        int j = y * dst_dim;
        int x;

        /* Perform unrolled operation. */
        for (x = 0; x < len; x++) {
            RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2);

            if (RC_UNROLL(rc_type_bin_to_u8) >= 2)
                RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2);

            if (RC_UNROLL(rc_type_bin_to_u8) == 4) {
                RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2);
                RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2);
            }
        }

        /* Handle the remaining vectors. */
        if (rem) {
            int r;

            /**
             *  For unroll factors > 1, we may still have some
             *  whole one source-vector -> 8 dest-vectors expansions.
             */
            for (r = rem; RC_UNROLL(rc_type_bin_to_u8) > 1 && r > 8; r -= 8)
                RC_TYPE_BIN_TO_U8_ITER(dst, src, j, i, arg1, arg2);

            /**
             *  The source image width is padded to the size of a whole
             *  vector, but the destination image padding is not
             *  required to scale to *eight* vector-sizes, thus we need
             *  to allow for a partial final source-to-destination
             *  iteration.
             */
            RC_TYPE_BIN_TO_U8_ITER_MAX(r, dst, src, j, i, arg1, arg2);
        }
    }

    RC_VEC_CLEANUP();
}
#endif
#endif
