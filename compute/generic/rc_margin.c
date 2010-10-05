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
 *  @file   rc_margin.c
 *  @brief  RAPP Compute layer binary logical margins, generic implementation.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_word.h"     /* Word operations       */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_pixel.h"    /* RC_PIXEL_SET_BIN()    */
#include "rc_margin.h"   /* Binary margin API     */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  An horizontal margin iteration.
 */
#define RC_MARGIN_HORZ_ITER(marg, src, i, j) \
do {                                         \
    rc_word_t mw = RC_WORD_LOAD(&(marg)[j]); \
    rc_word_t sw = RC_WORD_LOAD(&(src) [i]); \
    RC_WORD_STORE(&(marg)[j], mw | sw);      \
    (i) += RC_WORD_SIZE;                     \
    (j) += RC_WORD_SIZE;                     \
} while (0)

/**
 *  A vertical margin iteration.
 */
#define RC_MARGIN_VERT_ITER(src, acc, i) \
do {                                     \
    (acc) |= RC_WORD_LOAD(&(src)[i]);    \
    (i)   += RC_WORD_SIZE;               \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Compute the horizontal logical margin.
 */
#if RC_IMPL(rc_margin_horz_bin, 1)
void
rc_margin_horz_bin(uint8_t *restrict margin, const uint8_t *restrict src,
                   int dim, int width, int height)
{
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);  /* Total #words    */
    int full = len / RC_UNROLL(rc_margin_horz_bin); /* Fully unrolled  */
    int rem  = len % RC_UNROLL(rc_margin_horz_bin); /* Remaining words */
    int y;

    /* Process all rows */
    for (y = 0; y < height; y++) {
        int i = y*dim;
        int j = 0;
        int x;

        /* Handle all unrolled words */
        for (x = 0; x < full; x++) {
            RC_MARGIN_HORZ_ITER(margin, src, i, j);
            if (RC_UNROLL(rc_margin_horz_bin) >= 2) {
                RC_MARGIN_HORZ_ITER(margin, src, i, j);
            }
            if (RC_UNROLL(rc_margin_horz_bin) == 4) {
                RC_MARGIN_HORZ_ITER(margin, src, i, j);
                RC_MARGIN_HORZ_ITER(margin, src, i, j);
            }
        }

        /* Handle remaining words */
        for (x = 0; x < rem; x++) {
            RC_MARGIN_HORZ_ITER(margin, src, i, j);
        }
    }
}
#endif


/**
 *  Compute the vertical logical margin.
 */
#if RC_IMPL(rc_margin_vert_bin, 1)
void
rc_margin_vert_bin(uint8_t *restrict margin, const uint8_t *restrict src,
                   int dim, int width, int height)
{
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);  /* Total #words    */
    int full = len / RC_UNROLL(rc_margin_vert_bin); /* Fully unrolled  */
    int rem  = len % RC_UNROLL(rc_margin_vert_bin); /* Remaining words */
    int y;

    /* Process all rows */
    for (y = 0; y < height; y++) {
        rc_word_t acc = 0;
        int       i = y*dim;
        int       x;

        /* Handle all unrolled words */
        for (x = 0; x < full; x++) {
            RC_MARGIN_VERT_ITER(src, acc, i);
            if (RC_UNROLL(rc_margin_vert_bin) >= 2) {
                RC_MARGIN_VERT_ITER(src, acc, i);
            }
            if (RC_UNROLL(rc_margin_vert_bin) == 4) {
                RC_MARGIN_VERT_ITER(src, acc, i);
                RC_MARGIN_VERT_ITER(src, acc, i);
            }
        }

        /* Handle remaining words */
        for (x = 0; x < rem; x++) {
            RC_MARGIN_VERT_ITER(src, acc, i);
        }

        /* Set the pixel in the margin buffer */
        RC_PIXEL_SET_BIN(margin, 0, 0, y, 0, acc != 0);
    }
}
#endif
