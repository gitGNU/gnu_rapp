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
 *  @file   rapp_ref_scatter_bin.c
 *  @brief  RAPP binary scatter, reference implementation.
 */

#include "rapp.h"                 /* RAPP API           */
#include "rapp_ref_scatter_bin.h" /* Binary scatter API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
rapp_ref_scatter_bin(uint8_t *dst, int dst_dim,
                     const uint8_t *map, int map_dim,
                     const uint8_t *pack, int width, int height)
{
    int pos = 0;
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (rapp_pixel_get_bin(map, map_dim, 0, x, y)) {
                int pix = rapp_pixel_get_bin(pack, 0, 0, pos, 0);
                rapp_pixel_set_bin(dst, dst_dim, 0, x, y, pix);
                pos++;
            }
        }
    }

    return pos;
}
