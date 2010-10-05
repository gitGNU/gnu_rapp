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
 *  @file   rapp_ref_pad.c
 *  @brief  RAPP 8-bit image padding, reference implementation.
 */

#include <string.h>       /* memset()      */
#include <assert.h>       /* assert()      */
#include "rapp.h"         /* RAPP API      */
#include "rapp_ref_pad.h" /* 8-bit padding */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_pad_align_u8(uint8_t *buf, int dim, int width, int height, int value)
{
    int left  = (uintptr_t)buf % rapp_alignment;
    int right = rapp_alignment - (left + width - 1) % (rapp_alignment) - 1;
    int y;

    assert(width + left + right <= dim);

    for (y = 0; y < height; y++) {
        int x;

        for (x = 0; x < left; x++) {
            rapp_pixel_set_u8(buf, dim, -(x + 1), y, value);
        }

        for (x = 0; x < right; x++) {
            rapp_pixel_set_u8(buf, dim, width + x, y, value);
        }
    }
}


void
rapp_ref_pad_const_u8(uint8_t *buf, int dim,
                      int width, int height, int size, int value)
{
    int x, y, k;

    for (y = 0; y < height; y++) {
        for (k = 0; k < size; k++) {
            rapp_pixel_set_u8(buf, dim, -(k + 1),  y, value);
            rapp_pixel_set_u8(buf, dim, width + k, y, value);
        }
    }

    for (x = -size; x < width + size; x++) {
        for (k = 0; k < size; k++) {
            rapp_pixel_set_u8(buf, dim, x, -(k + 1),   value);
            rapp_pixel_set_u8(buf, dim, x, height + k, value);
        }
    }
}


void
rapp_ref_pad_clamp_u8(uint8_t *buf, int dim, int width, int height, int size)
{
    int x, y, k;

    for (y = 0; y < height; y++) {
        int left  = rapp_pixel_get_u8(buf, dim, 0,         y);
        int right = rapp_pixel_get_u8(buf, dim, width - 1, y);

        for (k = 0; k < size; k++) {
            rapp_pixel_set_u8(buf, dim, -(k + 1),  y, left);
            rapp_pixel_set_u8(buf, dim, width + k, y, right);
        }
    }

    for (x = -size; x < width + size; x++) {
        int top = rapp_pixel_get_u8(buf, dim, x, 0);
        int bot = rapp_pixel_get_u8(buf, dim, x, height - 1);

        for (k = 0; k < size; k++) {
            rapp_pixel_set_u8(buf, dim, x, -(k + 1),   top);
            rapp_pixel_set_u8(buf, dim, x, height + k, bot);
        }
    }
}
