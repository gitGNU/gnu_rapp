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
 *  @brief  RAPP Compute layer binary logical margins, vector implementation.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_vector.h"   /* Vector operations     */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_margin.h"   /* Binary margin API     */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  An horizontal margin iteration.
 */
#ifdef RC_VEC_OR
#define RC_MARGIN_HORZ_ITER(marg, src, i, j) \
do {                                         \
    rc_vec_t mv, sv;                         \
    RC_VEC_LOAD(mv, &(marg)[j]);             \
    RC_VEC_LOAD(sv, &(src) [i]);             \
    RC_VEC_OR(mv, mv, sv);                   \
    RC_VEC_STORE(&(marg)[j], mv);            \
    (i) += RC_VEC_SIZE;                      \
    (j) += RC_VEC_SIZE;                      \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Compute the horizontal logical margin.
 */
#if    RC_IMPL(rc_margin_horz_bin, 1)
#ifdef RC_MARGIN_HORZ_ITER
void
rc_margin_horz_bin(uint8_t *restrict margin, const uint8_t *restrict src,
                   int dim, int width, int height)
{
    int len  = RC_DIV_CEIL(width, 8*RC_VEC_SIZE);    /* Total #vectors    */
    int full = len / RC_UNROLL(rc_margin_horz_bin);  /* Fully unrolled    */
    int rem  = len % RC_UNROLL(rc_margin_horz_bin);  /* Remaining vectors */
    int y;

    RC_VEC_DECLARE();

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

    RC_VEC_CLEANUP();
}
#endif
#endif
