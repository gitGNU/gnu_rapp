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
 *  @file   rapp_ref_moment_bin.c
 *  @brief  RAPP binary image moments, reference implementation.
 */

#include "rapp.h"                /* RAPP API       */
#include "rapp_ref_moment_bin.h" /* Binary moments */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_moment_order1_bin(const uint8_t *buf, int dim,
                           int width, int height, uint32_t mom[3])
{
    uint32_t nsum = 0;
    uint32_t xsum = 0;
    uint32_t ysum = 0;
    int      x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int pix = rapp_pixel_get_bin(buf, dim, 0, x, y);
            nsum +=   pix;
            xsum += x*pix;
            ysum += y*pix;
        }
    }

    mom[0] = nsum;
    mom[1] = xsum;
    mom[2] = ysum;
}

void
rapp_ref_moment_order2_bin(const uint8_t *buf, int dim,
                           int width, int height, uintmax_t mom[6])
{
    uint32_t  nsum  = 0;
    uint32_t  xsum  = 0;
    uint32_t  ysum  = 0;
    uintmax_t x2sum = 0;
    uintmax_t y2sum = 0;
    uintmax_t xysum = 0;
    int       x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int pix = rapp_pixel_get_bin(buf, dim, 0, x, y);
            nsum  +=     pix;
            xsum  +=   x*pix;
            ysum  +=   y*pix;
            x2sum += x*x*pix;
            y2sum += y*y*pix;
            xysum += x*y*pix;
        }
    }

    mom[0] = nsum;
    mom[1] = xsum;
    mom[2] = ysum;
    mom[3] = x2sum;
    mom[4] = y2sum;
    mom[5] = xysum;
}
