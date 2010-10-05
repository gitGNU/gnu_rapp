/*  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_ref_integral.c
 *  @brief  RAPP integral sums, reference implementation.
 */

#include "rapp_ref_integral.h"  /* Integral */
#include "rapp.h"


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_integral_sum_bin_u8(uint8_t *dst, int dst_dim,
                             const uint8_t *src, int src_dim,
                             int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        int j1 = y * dst_dim;       /* Dest pixel row   */
        int j2 = (y-1) * dst_dim;   /* Dest index for previous row */

        for (x = 0; x < width; x++) {
            uint8_t pixel  = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            uint8_t left   = dst[j1 + (x-1)];
            uint8_t up     = dst[j2 + x];
            uint8_t upleft = dst[j2 + (x-1)];

            dst[j1 + x] = pixel + left + up - upleft;
        }
    }
}

void
rapp_ref_integral_sum_bin_u16(uint16_t *dst, int dst_dim,
                              const uint8_t *src, int src_dim,
                              int width, int height)
{
    int x, y;

    /* Adjust dimension to destination buffer */
    dst_dim = dst_dim / sizeof *dst;

    for (y = 0; y < height; y++) {
        int j1 = y * dst_dim;       /* Dest pixel row   */
        int j2 = (y-1) * dst_dim;   /* Dest index for previous row */

        for (x = 0; x < width; x++) {
            uint8_t  pixel  = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            uint16_t left   = dst[j1 + (x-1)];
            uint16_t up     = dst[j2 + x];
            uint16_t upleft = dst[j2 + (x-1)];

            dst[j1 + x] = pixel + left + up - upleft;
        }
    }
}

void
rapp_ref_integral_sum_bin_u32(uint32_t *dst, int dst_dim,
                              const uint8_t *src, int src_dim,
                              int width, int height)
{
    int x, y;

    /* Adjust dimension to destination buffer */
    dst_dim = dst_dim / sizeof *dst;

    for (y = 0; y < height; y++) {
        int j1 = y * dst_dim;       /* Dest pixel row   */
        int j2 = (y-1) * dst_dim;   /* Dest index for previous row */

        for (x = 0; x < width; x++) {
            uint8_t  pixel  = rapp_pixel_get_bin(src, src_dim, 0, x, y);
            uint32_t left   = dst[j1 + (x-1)];
            uint32_t up     = dst[j2 + x];
            uint32_t upleft = dst[j2 + (x-1)];

            dst[j1 + x] = pixel + left + up - upleft;
        }
    }
}

void
rapp_ref_integral_sum_u8_u16(uint16_t *dst, int dst_dim,
                             const uint8_t *src, int src_dim,
                             int width, int height)
{
    int x, y;

    /* Adjust dimension to destination buffer */
    dst_dim = dst_dim / sizeof *dst;

    for (y = 0; y < height; y++) {
        int j1 = y * dst_dim;       /* Dest pixel row */
        int j2 = (y-1) * dst_dim;   /* Dest index for previous row */

        for (x = 0; x < width; x++) {
            uint8_t  pixel  = rapp_pixel_get_u8(src, src_dim, x, y);
            uint16_t left   = dst[j1 + (x-1)];
            uint16_t up     = dst[j2 + x];
            uint16_t upleft = dst[j2 + (x-1)];

            dst[j1 + x] = pixel + left + up - upleft;
        }
    }
}

void
rapp_ref_integral_sum_u8_u32(uint32_t *dst, int dst_dim,
                             const uint8_t *src, int src_dim,
                             int width, int height)
{
    int x, y;

    /* Adjust dimension to destination buffer */
    dst_dim = dst_dim / sizeof *dst;

    for (y = 0; y < height; y++) {
        int j1 = y * dst_dim;       /* Dest pixel row */
        int j2 = (y-1) * dst_dim;   /* Dest index for previous row */

        for (x = 0; x < width; x++) {
            uint8_t pixel   = rapp_pixel_get_u8(src, src_dim, x, y);
            uint32_t left   = dst[j1 + (x-1)];
            uint32_t up     = dst[j2 + x];
            uint32_t upleft = dst[j2 + (x-1)];

            dst[j1 + x] = pixel + left + up - upleft;
        }
    }
}
