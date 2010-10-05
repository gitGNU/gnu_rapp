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
 *  @file   rapp_ref_crop.c
 *  @brief  RAPP binary image cropping, reference implementation.
 */

#include <string.h>        /* memset()         */
#include <assert.h>        /* assert()         */
#include <limits.h>        /* INT_MIN, INT_MAX */
#include "rapp.h"          /* RAPP API         */
#include "rapp_ref_crop.h" /* Binary cropping  */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#undef  MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef  MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
rapp_ref_crop_seek_bin(const uint8_t *buf, int dim, int width, int height,
                       unsigned pos[2])
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (rapp_pixel_get_bin(buf, dim, 0, x, y)) {
                pos[0] = x;
                pos[1] = y;
                return 1;
            }
        }
    }

    return 0;
}

int
rapp_ref_crop_box_bin(const uint8_t *buf, int dim, int width, int height,
                      unsigned box[4])
{
    int xmin = INT_MAX;
    int xmax = INT_MIN;
    int ymin = INT_MAX;
    int ymax = INT_MIN;
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (rapp_pixel_get_bin(buf, dim, 0, x, y)) {
                xmin  = MIN(xmin, x);
                xmax  = MAX(xmax, x);
                ymin  = MIN(ymin, y);
                ymax  = MAX(ymax, y);
            }
        }
    }

    if (xmin != INT_MAX) {
        box[0] = xmin;
        box[1] = ymin;
        box[2] = xmax - xmin + 1;
        box[3] = ymax - ymin + 1;
        return 1;
    }

    return 0;
}
