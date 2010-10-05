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
 *  @file   rapp_ref_reduce_bin.c
 *  @brief  RAPP binary 2x reduction, reference implementation.
 */

#include "rapp.h"                /* RAPP API            */
#include "rapp_ref_reduce_bin.h" /* Binary 2x reduction */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_ref_reduce_1x2_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int rank);
static void
rapp_ref_reduce_2x1_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int rank);
static void
rapp_ref_reduce_2x2_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height, int rank);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_reduce_1x2_rk1_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_1x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 1);
}

void
rapp_ref_reduce_1x2_rk2_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_1x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 2);
}

void
rapp_ref_reduce_2x1_rk1_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x1_bin(dst, dst_dim, src, src_dim,
                                   width, height, 1);
}

void
rapp_ref_reduce_2x1_rk2_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x1_bin(dst, dst_dim, src, src_dim,
                                   width, height, 2);
}

void
rapp_ref_reduce_2x2_rk1_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 1);
}

void
rapp_ref_reduce_2x2_rk2_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 2);
}

void
rapp_ref_reduce_2x2_rk3_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 3);
}

void
rapp_ref_reduce_2x2_rk4_bin(uint8_t *dst, int dst_dim,
                            const uint8_t *src, int src_dim,
                            int width, int height)
{
    return rapp_ref_reduce_2x2_bin(dst, dst_dim, src, src_dim,
                                   width, height, 4);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rapp_ref_reduce_1x2_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int rank)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x += 2) {
            int sum;

            sum = rapp_pixel_get_bin(src, src_dim, 0, x,     y) +
                  rapp_pixel_get_bin(src, src_dim, 0, x + 1, y);

            rapp_pixel_set_bin(dst, dst_dim, 0, x/2, y, sum >= rank);
        }
    }
}

static void
rapp_ref_reduce_2x1_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int rank)
{
    int x, y;

    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x++) {
            int sum;

            sum = rapp_pixel_get_bin(src, src_dim, 0, x, y    ) +
                  rapp_pixel_get_bin(src, src_dim, 0, x, y + 1);

            rapp_pixel_set_bin(dst, dst_dim, 0, x, y/2, sum >= rank);
        }
    }
}

static void
rapp_ref_reduce_2x2_bin(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, int rank)
{
    int x, y;

    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x += 2) {
            int sum;

            sum = rapp_pixel_get_bin(src, src_dim, 0, x,     y    ) +
                  rapp_pixel_get_bin(src, src_dim, 0, x + 1, y    ) +
                  rapp_pixel_get_bin(src, src_dim, 0, x,     y + 1) +
                  rapp_pixel_get_bin(src, src_dim, 0, x + 1, y + 1);

            rapp_pixel_set_bin(dst, dst_dim, 0, x/2, y/2, sum >= rank);
        }
    }
}
