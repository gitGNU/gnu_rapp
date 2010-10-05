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
 *  @file   rapp_ref_margin.c
 *  @brief  RAPP binary logical margins, reference implementation.
 */

#include <string.h>          /* memset()       */
#include <assert.h>          /* assert()       */
#include "rapp.h"            /* RAPP API       */
#include "rapp_ref_margin.h" /* Binary margins */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_margin_horz_bin(uint8_t *margin, const uint8_t *src,
                         int dim, int width, int height)
{
    int x, y;

    memset(margin, 0, (width + 7) / 8);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rapp_pixel_set_bin(margin, 0, 0, x, 0,
                               rapp_pixel_get_bin(src, dim,  0, x, y) |
                               rapp_pixel_get_bin(margin, 0, 0, x, 0));
        }
    }
}

void
rapp_ref_margin_vert_bin(uint8_t *margin, const uint8_t *src,
                         int dim, int width, int height)
{
    int x, y;

    memset(margin, 0, (height + 7) / 8);

    for (y = 0; y < height; y++) {
        int pix = rapp_pixel_get_bin(margin, 0, 0, y, 0);

        for (x = 0; x < width; x++) {
            pix |= rapp_pixel_get_bin(src, dim, 0, x, y);
        }

        rapp_pixel_set_bin(margin, 0, 0, y, 0, pix);
    }
}
