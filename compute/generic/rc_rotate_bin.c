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
 *  @file   rc_rotate_bin.c
 *  @brief  RAPP Compute layer binary image rotation.
 */

#include "rc_word.h"       /* Word operations     */
#include "rc_rotate_bin.h" /* Binary rotation API */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rc_rotate_block_cw(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int height, rc_word_t mask);

static void
rc_rotate_block_ccw(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int height, rc_word_t mask);

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Rotate a binary image 90 degrees in clockwise direction.
 */
void
rc_rotate_cw_bin(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int       xblock = width  / (8*RC_WORD_SIZE); /* Num blocks in x dir     */
    int       yblock = height / (8*RC_WORD_SIZE); /* Num blocks in y dir     */
    int       xrem   = width  % (8*RC_WORD_SIZE); /* Partial block, x dir    */
    int       yrem   = height % (8*RC_WORD_SIZE); /* Partial block, y dir    */
    rc_word_t mask   = RC_WORD_SHL(RC_WORD_ONE,   /* Partial block word mask */
                                   8*RC_WORD_SIZE - xrem);
    int       step   = 8*RC_WORD_SIZE*dst_dim;    /* Dst buffer block step   */
    int       yb, y;

    /* Rotate the first row of partial blocks */
    if (yrem) {
        int i = 0;
        int j = yblock*RC_WORD_SIZE;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RC_WORD_SIZE, j += step) {
            rc_rotate_block_cw(&dst[j], dst_dim,
                               &src[i], src_dim,
                               yrem, RC_WORD_ONE);
        }
        if (xrem) {
            rc_rotate_block_cw(&dst[j], dst_dim,
                               &src[i], src_dim,
                               yrem, mask);
        }
    }

    /* Rotate all full blocks */
    for (yb = 0, y = 0; yb < yblock; yb++, y += 8*RC_WORD_SIZE) {
        int i = (yrem + y)*src_dim;
        int j = (yblock - yb - 1)*RC_WORD_SIZE;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RC_WORD_SIZE, j += step) {
            rc_rotate_block_cw(&dst[j], dst_dim,
                               &src[i], src_dim,
                               8*RC_WORD_SIZE, RC_WORD_ONE);
        }

        if (xrem) {
            rc_rotate_block_cw(&dst[j], dst_dim,
                               &src[i], src_dim,
                               8*RC_WORD_SIZE, mask);
        }
    }
}


/**
 *  Rotate a binary image 90 degrees in counter-clockwise direction.
 */
void
rc_rotate_ccw_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    int       xblock = width  / (8*RC_WORD_SIZE); /* Num blocks in x dir     */
    int       yblock = height / (8*RC_WORD_SIZE); /* Num blocks in y dir     */
    int       xrem   = width  % (8*RC_WORD_SIZE); /* Partial block, x dir    */
    int       yrem   = height % (8*RC_WORD_SIZE); /* Partial block, y dir    */
    rc_word_t mask   = RC_WORD_SHL(RC_WORD_ONE,   /* Partial block word mask */
                                   8*RC_WORD_SIZE - xrem);
    int       step   = 8*RC_WORD_SIZE*dst_dim;    /* Dst buffer block step   */
    int       end    = (width - 1)*dst_dim;       /* Dst buffer last row     */
    int       yb, y;

    /* Rotate all full blocks */
    for (yb = 0, y = 0; yb < yblock; yb++, y += 8*RC_WORD_SIZE) {
        int i = y*src_dim;
        int j = yb*RC_WORD_SIZE + end;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RC_WORD_SIZE, j -= step) {
            rc_rotate_block_ccw(&dst[j], dst_dim,
                                &src[i], src_dim,
                                8*RC_WORD_SIZE, RC_WORD_ONE);
        }

        if (xrem) {
            rc_rotate_block_ccw(&dst[j], dst_dim,
                                &src[i], src_dim,
                                8*RC_WORD_SIZE, mask);
        }
    }

    /* Rotate the last row of partial blocks */
    if (yrem) {
        int i = 8*RC_WORD_SIZE*yblock*src_dim;
        int j = RC_WORD_SIZE*yblock + end;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RC_WORD_SIZE, j -= step) {
            rc_rotate_block_ccw(&dst[j], dst_dim,
                                &src[i], src_dim,
                                yrem, RC_WORD_ONE);
        }
        if (xrem) {
            rc_rotate_block_ccw(&dst[j], dst_dim,
                                &src[i], src_dim,
                                yrem, mask);
        }
    }
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

/**
 *  Rotate a block of 8*RC_WORD_SIZE pixels in CW direction.
 *  The dst buffer points to the first row of the block,
 *  i.e. the upper-left corner. The src buffer points to the
 *  start of the block. Partial blocks are handled via the
 *  height and mask parameters for the y and x dimensions,
 *  respectivly.
 */
static void
rc_rotate_block_cw(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int height, rc_word_t mask)
{
    rc_word_t bit = RC_WORD_BIT(height - 1);
    int       y, i;

    for (y = 0, i = 0;
         y < height;
         y++, i += src_dim, bit = RC_WORD_SHL(bit, 1))
    {
        rc_word_t word = RC_WORD_LOAD(&src[i]) & mask;

        if (word) {
            int b, j;

            /* Handle all bits in the word */
            for (b = 0, j = 0; b < 8*RC_WORD_SIZE; b++, j += dst_dim) {
                if (word & RC_WORD_BIT(b)) {

                    /* Set the corresponding bit in the dst word */
                    rc_word_t w = RC_WORD_LOAD(&dst[j]);
                    RC_WORD_STORE(&dst[j], w | bit);
                }
            }
        }
    }
}

/**
 *  Rotate a block of 8*RC_WORD_SIZE pixels in CCW direction.
 *  The dst buffer points to the last row of the block,
 *  i.e. the lower-left corner. The src buffer points to the
 *  start of the block. Partial blocks are handled via the
 *  height and mask parameters for the y and x dimensions,
 *  respectivly.
 */
static void
rc_rotate_block_ccw(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int height, rc_word_t mask)
{
    rc_word_t bit = RC_WORD_BIT(0);
    int       y, i;

    for (y = 0, i = 0;
         y < height;
         y++, i += src_dim, bit = RC_WORD_SHR(bit, 1))
    {
        rc_word_t word = RC_WORD_LOAD(&src[i]) & mask;

        if (word) {
            int b, j;

            /* Handle all bits in the word */
            for (b = 0, j = 0; b < 8*RC_WORD_SIZE; b++, j -= dst_dim) {
                if (word & RC_WORD_BIT(b)) {

                    /* Set the corresponding bit in the dst word */
                    rc_word_t w = RC_WORD_LOAD(&dst[j]);
                    RC_WORD_STORE(&dst[j], w | bit);
                }
            }
        }
    }
}
