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
 *  @file   rapp_stat.h
 *  @brief  RAPP statistical functions.
 */

/**
 *  @defgroup grp_stat Sum, Sum-of-Squares, Min and Max Statistics
 *  @brief Image sum, sum-of-squares, cross-sum, min and max statistics.
 *
 *  All measures are image-global. The functions may read data up to
 *  the nearest alignment boundary, so if those pixels are not properly
 *  cleared the output may be invalid.
 *
 *  All images must be aligned.
 *
 *  <p>@ref grp_moment "Next section: Moments"</p>
 *
 *  @{
 */

#ifndef RAPP_STAT_H
#define RAPP_STAT_H

#include <stdint.h>
#include "rapp_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary image sum.
 *  Compute the sum of all pixels in a binary image.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The sum of the pixels in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int32_t
rapp_stat_sum_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel sum.
 *  Compute the sum of all pixels in an 8-bit image.
 *  The result may be invalid if the image is larger than
 *  2<sup>8*(sizeof(int) - 1)</sup> pixels.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Image pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The sum of the pixels in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int32_t
rapp_stat_sum_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel squared sum.
 *  Compute the sum and the sum-of-squares of all pixels in an 8-bit image.
 *  The result may be invalid if the image is larger than
 *  2<sup>8*(sizeof(int) - 1)</sup> pixels.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in]  buf     The image pixel buffer.
 *  @param      dim     The row dimension in bytes of the pixel buffer.
 *  @param      width   The image width in pixels.
 *  @param      height  The image height in pixels.
 *  @param[out] sum     The location where to store the sum and the
 *                      sum-of-squares of the image.
 *  @return             A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_stat_sum2_u8(const uint8_t *buf, int dim,
                  int width, int height, uintmax_t sum[2]);

/**
 *  8-bit pixel cross sum.
 *  Compute the cross sums of two 8-bit images.
 *  The result may be invalid if the image is larger than
 *  2<sup>8*(sizeof(int) - 1)</sup> pixels.
 *  Image buffers and dimensions must be aligned.
 *
 *  @param[in]  src1      First image pixel buffer.
 *  @param      src1_dim  Row dimension in bytes of the first buffer.
 *  @param[in]  src2      Second image pixel buffer.
 *  @param      src2_dim  Row dimension in bytes of the second buffer.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @param[out] sum       Output sum storage array with at least five elements.
 *                        It is filled in as follows:
 *                          - sum[0] = sum(src1)
 *                          - sum[1] = sum(src2)
 *                          - sum[2] = sum(src1*src1)
 *                          - sum[3] = sum(src2*src2)
 *                          - sum[4] = sum(src1*src2)
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_stat_xsum_u8(const uint8_t *restrict src1, int src1_dim,
                  const uint8_t *restrict src2, int src2_dim,
                  int width, int height, uintmax_t sum[5]);

/**
 *  Binary pixel min.
 *  Finds the minimum pixel value in a binary image.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Image pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The minimum pixel value in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int
rapp_stat_min_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  Binary pixel max.
 *  Finds the maximum pixel value in a binary image.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Image pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The maximum pixel value in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int
rapp_stat_max_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel min.
 *  Finds the minimum pixel value in an 8-bit image.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Image pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The minimum pixel value in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int
rapp_stat_min_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel max.
 *  Finds the maximum pixel value in an 8-bit image.
 *  Image buffer and dimension must be aligned.
 *
 *  @param[in] buf     Image pixel buffer.
 *  @param     dim     Row dimension in bytes of the pixel buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The maximum pixel value in the image,
 *                     or a negative error code on error.
 */
RAPP_EXPORT int
rapp_stat_max_u8(const uint8_t *buf, int dim, int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_STAT_H */
/** @} */
