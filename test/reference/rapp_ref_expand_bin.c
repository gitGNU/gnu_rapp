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
 *  @file   rapp_ref_expand_bin.c
 *  @brief  RAPP binary 2x expansion, reference implementation.
 */

#include "rapp.h"                /* RAPP API            */
#include "rapp_ref_reduce_bin.h" /* Binary 2x reduction */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_expand_1x2_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {

            int pix = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x,     y, pix);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x + 1, y, pix);
        }
    }
}

void
rapp_ref_expand_2x1_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {

            int pix = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            rapp_pixel_set_bin(dst, dst_dim, 0, x, 2*y,     pix);
            rapp_pixel_set_bin(dst, dst_dim, 0, x, 2*y + 1, pix);
        }
    }
}

void
rapp_ref_expand_2x2_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {

            int pix = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x,     2*y,     pix);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x + 1, 2*y,     pix);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x,     2*y + 1, pix);
            rapp_pixel_set_bin(dst, dst_dim, 0, 2*x + 1, 2*y + 1, pix);
        }
    }
}
