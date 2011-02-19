/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_ref_stat.c
 *  @brief  RAPP statistics, reference implementation.
 */

#include <string.h>          /* memset()   */
#include "rapp_ref_stat.h"   /* Statistics */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_ref_stat_cnt(uint64_t sum[2], unsigned pix);

static void
rapp_ref_stat_sum(uint64_t sum[2], unsigned pix);

static void
rapp_ref_stat_min(uint64_t *min, unsigned pix);

static void
rapp_ref_stat_max(uint64_t *max, unsigned pix);

static void
rapp_ref_stat_driver(const uint8_t *buf, int dim, int width, int height,
                     uint64_t *sum, void (*accum)(uint64_t*, unsigned));


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int32_t
rapp_ref_stat_sum_bin(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t sum[2] = {0, 0};
    rapp_ref_stat_driver(buf, dim, (width + 7) / 8, height,
                         sum, &rapp_ref_stat_cnt);
    return (int32_t)sum[0];
}

int32_t
rapp_ref_stat_sum_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t sum[2] = {0, 0};
    rapp_ref_stat_driver(buf, dim, width, height,
                         sum, &rapp_ref_stat_sum);
    return (int32_t)sum[0];
}

void
rapp_ref_stat_sum2_u8(const uint8_t *buf, int dim,
                      int width, int height, uint64_t sum[2])
{
    sum[0] = 0;
    sum[1] = 0;
    rapp_ref_stat_driver(buf, dim, width, height,
                         sum, &rapp_ref_stat_sum);
}

void
rapp_ref_stat_xsum_u8(const uint8_t *buf1, int dim1,
                      const uint8_t *buf2, int dim2,
                      int width, int height, uintmax_t sum[5])
{
    int x, y;

    memset(sum, 0, 5*sizeof *sum);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            unsigned pix1 = buf1[y*dim1 + x];
            unsigned pix2 = buf2[y*dim2 + x];

            sum[0] += pix1;
            sum[1] += pix2;
            sum[2] += pix1*pix1;
            sum[3] += pix2*pix2;
            sum[4] += pix1*pix2;
        }
    }
}

int
rapp_ref_stat_min_bin(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t min = 0xff;
    rapp_ref_stat_driver(buf, dim, (width + 7) / 8, height,
                         &min, &rapp_ref_stat_min);
    return min == 0xff;
}

int
rapp_ref_stat_max_bin(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t max = 0;
    rapp_ref_stat_driver(buf, dim, (width + 7) / 8, height,
                         &max, &rapp_ref_stat_max);
    return max != 0;
}

int
rapp_ref_stat_min_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t min = 0xff;
    rapp_ref_stat_driver(buf, dim, width, height,
                         &min, &rapp_ref_stat_min);
    return min;
}

int
rapp_ref_stat_max_u8(const uint8_t *buf, int dim, int width, int height)
{
    uint64_t max = 0;
    rapp_ref_stat_driver(buf, dim, width, height,
                         &max, &rapp_ref_stat_max);
    return max;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rapp_ref_stat_cnt(uint64_t sum[2], unsigned pix)
{
    int b;
    for (b = 0; b < 8; b++) {
        sum[0] += (pix >> b) & 1;
    }
}

static void
rapp_ref_stat_sum(uint64_t sum[2], unsigned pix)
{
    sum[0] += pix;
    sum[1] += pix*pix;
}

static void
rapp_ref_stat_min(uint64_t *min, unsigned pix)
{
    if (pix < *min) {
        *min = pix;
    }
}

static void
rapp_ref_stat_max(uint64_t *max, unsigned pix)
{
    if (pix > *max) {
        *max = pix;
    }
}

static void
rapp_ref_stat_driver(const uint8_t *buf, int dim, int width, int height,
                     uint64_t *sum, void (*accum)(uint64_t*, unsigned))
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            (*accum)(sum, buf[y*dim + x]);
        }
    }
}
