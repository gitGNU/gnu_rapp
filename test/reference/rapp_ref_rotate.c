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
 *  @file   rapp_ref_rotate.c
 *  @brief  RAPP 8-bit image rotation, reference implementation.
 */

#include "rapp.h"            /* RAPP API  */
#include "rapp_ref_rotate.h" /* Rotations */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_rotate_cw_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int pix = rapp_pixel_get_u8(src, src_dim, x, y);
            rapp_pixel_set_u8(dst, dst_dim, height - y - 1, x, pix);
        }
    }
}

void
rapp_ref_rotate_ccw_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int pix = rapp_pixel_get_u8(src, src_dim, x, y);
            rapp_pixel_set_u8(dst, dst_dim, y, width - x - 1, pix);
        }
    }
}
