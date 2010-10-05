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
 *  @file   rapp_ref_filter.c
 *  @brief  RAPP fixed-filter convolutions, reference implementation
 */

#include <stdlib.h>          /* abs()         */
#include "rc_stdbool.h" /* Portable stdbool.h */
#include "rapp_ref_filter.h" /* Reference API */

/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static const int rapp_ref_mask_diff_1x2[] =
    {1, -1};

static const int rapp_ref_mask_sobel_3x3[] =
    {1, 0, -1,
     2, 0, -2,
     1, 0, -1};

static const int rapp_ref_mask_gauss_3x3[] =
    {1, 2, 1,
     2, 4, 2,
     1, 2, 1};

static const int rapp_ref_mask_laplace_3x3[] =
    { 0, -1,  0,
     -1,  4, -1,
      0, -1,  0};

static const int rapp_ref_mask_highpass_3x3[] =
    {-1, -1, -1,
     -1,  8, -1,
     -1, -1, -1};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_ref_filter_driver(float *dst, int dst_dim, const uint8_t *src,
                       int src_dim, int width, int height,
                       const int *mask, int mask_width, int mask_height,
                       int bias, int norm,
                       bool flip, bool absolute, bool magnitude);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_filter_diff_1x2_horz_u8(float *dst, int dst_dim,
                                 const uint8_t *src, int src_dim,
                                 int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_diff_1x2, 2, 1, 0x80, 2,
                           false, false, false);
}

void
rapp_ref_filter_diff_1x2_horz_abs_u8(float *dst, int dst_dim,
                                     const uint8_t *src, int src_dim,
                                     int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_diff_1x2, 2, 1, 0, 1,
                           false, true, false);
}

void
rapp_ref_filter_diff_2x1_vert_u8(float *dst, int dst_dim,
                                 const uint8_t *src, int src_dim,
                                 int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_diff_1x2, 2, 1, 0x80, 2,
                           true, false, false);
}

void
rapp_ref_filter_diff_2x1_vert_abs_u8(float *dst, int dst_dim,
                                     const uint8_t *src, int src_dim,
                                     int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_diff_1x2, 2, 1, 0, 1,
                           true, true, false);
}

void
rapp_ref_filter_diff_2x2_magn_u8(float *dst, int dst_dim,
                                 const uint8_t *src, int src_dim,
                                 int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_diff_1x2, 2, 1, 0, 2,
                           false, false, true);
}

void
rapp_ref_filter_sobel_3x3_horz_u8(float *dst, int dst_dim,
                                  const uint8_t *src, int src_dim,
                                  int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_sobel_3x3, 3, 3, 0x80, 8,
                           false, false, false);
}

void
rapp_ref_filter_sobel_3x3_horz_abs_u8(float *dst, int dst_dim,
                                      const uint8_t *src, int src_dim,
                                      int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_sobel_3x3, 3, 3, 0, 4,
                           false, true, false);
}

void
rapp_ref_filter_sobel_3x3_vert_u8(float *dst, int dst_dim,
                                  const uint8_t *src, int src_dim,
                                  int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_sobel_3x3, 3, 3, 0x80, 8,
                           true, false, false);
}

void
rapp_ref_filter_sobel_3x3_vert_abs_u8(float *dst, int dst_dim,
                                      const uint8_t *src, int src_dim,
                                      int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_sobel_3x3, 3, 3, 0, 4,
                           true, true, false);
}

void
rapp_ref_filter_sobel_3x3_magn_u8(float *dst, int dst_dim,
                                  const uint8_t *src, int src_dim,
                                  int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_sobel_3x3, 3, 3, 0, 8,
                           false, false, true);
}

void
rapp_ref_filter_gauss_3x3_u8(float *dst, int dst_dim,
                             const uint8_t *src, int src_dim,
                             int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_gauss_3x3, 3, 3, 0, 16,
                           false, false, false);
}

void
rapp_ref_filter_laplace_3x3_u8(float *dst, int dst_dim,
                               const uint8_t *src, int src_dim,
                               int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_laplace_3x3, 3, 3, 0x80, 8,
                           false, false, false);
}

void
rapp_ref_filter_laplace_3x3_abs_u8(float *dst, int dst_dim,
                                   const uint8_t *src, int src_dim,
                                   int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_laplace_3x3, 3, 3, 0, 4,
                           false, true, false);
}

void
rapp_ref_filter_highpass_3x3_u8(float *dst, int dst_dim,
                                const uint8_t *src, int src_dim,
                                int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_highpass_3x3, 3, 3, 0x80, 16,
                           false, false, false);
}

void
rapp_ref_filter_highpass_3x3_abs_u8(float *dst, int dst_dim,
                                    const uint8_t *src, int src_dim,
                                    int width, int height)
{
    rapp_ref_filter_driver(dst, dst_dim, src, src_dim, width, height,
                           rapp_ref_mask_highpass_3x3, 3, 3, 0, 8,
                           false, true, false);
}


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_ref_filter_driver(float *dst, int dst_dim, const uint8_t *src,
                       int src_dim, int width, int height,
                       const int *mask, int mask_width, int mask_height,
                       int bias, int norm,
                       bool flip, bool absolute, bool magnitude)
{
    float scale = 1.0f / (float)norm;
    int   x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int nsum = 0;
            int tsum = 0;
            int xx, yy;

            for (yy = 0; yy < mask_height; yy++) {
                for (xx = 0; xx < mask_width; xx++) {
                    int i = (y + yy - mask_height/2)*src_dim +
                             x + xx - mask_width/2;
                    int k = (mask_height - yy - 1)*mask_width +
                             mask_width  - xx - 1;

                    nsum += src[i]*mask[k];
                }
            }

            for (yy = 0; yy < mask_height; yy++) {
                for (xx = 0; xx < mask_width; xx++) {
                    int i = (y + xx - mask_width/2)*src_dim +
                             x + yy - mask_height/2;
                    int k = (mask_height - yy - 1)*mask_width +
                             mask_width  - xx - 1;

                    tsum += src[i]*mask[k];
                }
            }

            if (magnitude) {
                nsum = abs(nsum) + abs(tsum);
            }
            else if (flip) {
                nsum = tsum;
            }
            if (absolute) {
                nsum = abs(nsum);
            }

            dst[y*dst_dim/sizeof(float) + x] = (float)nsum*scale + (float)bias;
        }
    }
}
