/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @brief  RAPP Compute layer type conversions, generic implementation.
 */

#include "rc_impl_cfg.h" /* Implementation cfg     */
#include "rc_thresh_tpl.h" /* Thresholding templates */
#include "rc_table.h"    /* Lookup tables          */
#include "rc_type.h"     /* Type conversion API    */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Compute op > 0x7f ? 1 : 0 without branches.
 */
#define RC_TYPE_U8_TO_BIN(op, arg1, arg2) \
    ((unsigned)(op) >> 7)

/**
 *  There's a loop inside the iteration macro, but for our manual unroll
 *  chunks it has known small boundaries; between 0..8*RC_WORD_SIZE in
 *  steps of 4, so the compiler is likely to unroll it independently
 *  from our efforts and we should be able to consider it already
 *  unrolled.
 */
#define RC_BIN_TO_U8_ITER(src, dst, max, srcidx, dstidx)        \
do {                                                            \
    rc_word_t word = RC_WORD_LOAD(&src[srcidx]);                \
    int k;                                                      \
                                                                \
    for (k = 0; k < max; k += 4, dstidx += 4) {                 \
        unsigned nibble = RC_WORD_EXTRACT(word, k, 4);          \
        *(uint32_t*)&dst[dstidx] = rc_table_expand[nibble];     \
    }                                                           \
    srcidx += RC_WORD_SIZE;                                     \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Convert u8 to binary.
 */
#if RC_IMPL(rc_type_u8_to_bin, 1)
void
rc_type_u8_to_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim,
                       width, height, 0, 0, RC_TYPE_U8_TO_BIN,
                       RC_UNROLL(rc_type_u8_to_bin));
}
#endif


/**
 *  Convert binary to u8.
 */
#if RC_IMPL(rc_type_bin_to_u8, 1)
void
rc_type_bin_to_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    int full = width / (8*RC_WORD_SIZE * RC_UNROLL(rc_type_bin_to_u8));
    int rem  = width % (8*RC_WORD_SIZE * RC_UNROLL(rc_type_bin_to_u8));
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all full source words. */
        for (x = 0; x < full; x++) {
            RC_BIN_TO_U8_ITER(src, dst, 8*RC_WORD_SIZE, i, j);

            if (RC_UNROLL(rc_type_bin_to_u8) >= 2)
                RC_BIN_TO_U8_ITER(src, dst, 8*RC_WORD_SIZE, i, j);

            if (RC_UNROLL(rc_type_bin_to_u8) == 4) {
                RC_BIN_TO_U8_ITER(src, dst, 8*RC_WORD_SIZE, i, j);
                RC_BIN_TO_U8_ITER(src, dst, 8*RC_WORD_SIZE, i, j);
            }
        }

        /* Handle all remaining source words. */
        if (rem) {
            int r;
            for (r = rem; r > 8*RC_WORD_SIZE; r -= 8*RC_WORD_SIZE)
                RC_BIN_TO_U8_ITER(src, dst, 8*RC_WORD_SIZE, i, j);
            RC_BIN_TO_U8_ITER(src, dst, r, i, j);
        }
    }
}
#endif
