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
 *  @file   rapp_ref_reduce.c
 *  @brief  RAPP 8-bit 2x reduction, reference implementation.
 */

#include "rapp_ref_reduce.h" /* 8-bit 2x reduction */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_reduce_1x2_u8(float *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    int x, y;

    dst_dim /= sizeof *dst;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x += 2) {
            int i = y*src_dim + x;
            int j = y*dst_dim + x/2;

            dst[j] = (src[i] + src[i + 1]) / 2.0f;
        }
    }
}

void
rapp_ref_reduce_2x1_u8(float *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    int x, y;

    dst_dim /= sizeof *dst;

    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x++) {
            int i =  y   *src_dim + x;
            int j = (y/2)*dst_dim + x;

            dst[j] = (src[i] + src[i + src_dim]) / 2.0f;
        }
    }
}

void
rapp_ref_reduce_2x2_u8(float *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    int x, y;

    dst_dim /= sizeof *dst;

    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x += 2) {
            int i =  y   *src_dim + x;
            int j = (y/2)*dst_dim + x/2;

            dst[j] = (src[i          ] + src[i           + 1] +
                      src[i + src_dim] + src[i + src_dim + 1]) / 4.0f;
        }
    }
}
