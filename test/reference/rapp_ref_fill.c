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
 *  @file   rapp_ref_fill.c
 *  @brief  RAPP connected-components seed fill, reference implementation.
 */

#include <string.h>        /* memset()  */
#include "rapp.h"          /* RAPP API  */
#include "rapp_ref_fill.h" /* Seed fill */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_ref_fill_driver(uint8_t *dst, int dst_dim,
                     const uint8_t *map, int map_dim,
                     int width, int height, int xseed, int yseed,
                     const char *mask);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_fill_4conn_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *map, int map_dim,
                        int width, int height, int xseed, int yseed)
{
    const char mask[] = " 1 "
                        "111"
                        " 1 ";

    rapp_ref_fill_driver(dst, dst_dim, map, map_dim,
                         width, height, xseed, yseed, mask);
}

void
rapp_ref_fill_8conn_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *map, int map_dim,
                        int width, int height, int xseed, int yseed)
{
    const char mask[] = "111"
                        "111"
                        "111";

    rapp_ref_fill_driver(dst, dst_dim, map, map_dim,
                         width, height, xseed, yseed, mask);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rapp_ref_fill_driver(uint8_t *dst, int dst_dim,
                     const uint8_t *map, int map_dim,
                     int width, int height, int xseed, int yseed,
                     const char *mask)
{
    /* Clear destination buffer */
    memset(dst, 0, dst_dim*height);

    if (rapp_pixel_get_bin(map, map_dim, 0, xseed, yseed)) {
        int *stack;

        /* Allocate the stack buffer */
        stack = malloc(2*width*height*sizeof *stack);

        /* Set the seed pixel */
        rapp_pixel_set_bin(dst, dst_dim, 0, xseed, yseed, 1);

        /* Push end marker */
        stack[0] = -1;

        /* Push seed coordinate */
        *++stack = xseed;
        *++stack = yseed;

        /* Process all pixels */
        while (*stack >= 0) {
            int y = *stack--;
            int x = *stack--;
            int i, j;

            /* Process neighbors */
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    if (mask[3*i + j] == '1') {
                        int xx = x + j - 1;
                        int yy = y + i - 1;

                        if (xx >= 0 && xx < width &&
                            yy >= 0 && yy < height)
                        {

                            /* Check if the pixel is settable */
                            if ( rapp_pixel_get_bin(map, map_dim, 0, xx, yy) &&
                                !rapp_pixel_get_bin(dst, dst_dim, 0, xx, yy))
                            {
                                /* Set the pixel and push on the stack */
                                rapp_pixel_set_bin(dst, dst_dim, 0, xx, yy, 1);
                                *++stack = xx;
                                *++stack = yy;
                            }
                        }
                    }
                }
            }
        }

        /* Release the stack buffeer */
        free(stack);
    }
}
