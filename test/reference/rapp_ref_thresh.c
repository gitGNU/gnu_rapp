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
 *  @file   rapp_thresh.c
 *  @brief  RAPP thresholding to binary, reference implementation.
 */

#include "rapp.h"             /* RAPP API      */
#include "rapp_ref_thresh.h"  /* Reference API */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_ref_thresh_gt(int val, int thr, int arg);

static int
rapp_ref_thresh_lt(int val, int thr, int arg);

static int
rapp_ref_thresh_gtlt(int val, int low, int high);

static int
rapp_ref_thresh_ltgt(int val, int low, int high);

static void
rapp_ref_thresh_driver(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, int low, int high,
                       int (*cmp)(int, int, int));

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_thresh_gt_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height, int thresh)
{
    rapp_ref_thresh_driver(dst, dst_dim, src, src_dim, width, height,
                           thresh, 0, &rapp_ref_thresh_gt);
}

void
rapp_ref_thresh_lt_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height, int thresh)
{
    rapp_ref_thresh_driver(dst, dst_dim, src, src_dim, width, height,
                           thresh, 0, &rapp_ref_thresh_lt);
}

void
rapp_ref_thresh_gtlt_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int low, int high)
{
    rapp_ref_thresh_driver(dst, dst_dim, src, src_dim, width, height,
                           low, high, &rapp_ref_thresh_gtlt);
}

void
rapp_ref_thresh_ltgt_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int low, int high)
{
    rapp_ref_thresh_driver(dst, dst_dim, src, src_dim, width, height,
                           low, high, &rapp_ref_thresh_ltgt);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rapp_ref_thresh_gt(int val, int thr, int arg)
{
    (void)arg;
    return val > thr;
}

static int
rapp_ref_thresh_lt(int val, int thr, int arg)
{
    (void)arg;
    return val < thr;
}

static int
rapp_ref_thresh_gtlt(int val, int low, int high)
{
    return val > low && val < high;
}

static int
rapp_ref_thresh_ltgt(int val, int low, int high)
{
    return val < low || val > high;
}

static void
rapp_ref_thresh_driver(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, int low, int high,
                       int (*cmp)(int, int, int))
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int val = rapp_pixel_get_u8(src, src_dim, x, y);
            int bit = (*cmp)(val, low, high);

            rapp_pixel_set_bin(dst, dst_dim, 0, x, y, bit);
        }
    }
}
