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
 *  @file   rapp_filter.h
 *  @brief  RAPP fixed-filter convolutions.
 */

/**
 *  @defgroup grp_filter Fixed-Filter Convolutions
 *
 *  @section Overview
 *  These functions perform convolutions with common filter kernels
 *  up to size 3x3. In some cases there are a few variants that perform
 *  additional operations such as absolute-value and magnitude after
 *  the actual convolution.
 *
 *  @section Precision
 *  For this family of functions, there is a performance/precision
 *  trade-off, and performance is favoured over precision. This means
 *  that the functions listed here do not always produce the exact results.
 *  We define the error as @e d &ndash; @e r, where @e d is the computed
 *  value and @e r is the reference value. The error is measured in @e ulp,
 *  unit in the last place. The error is platform-dependent, but it
 *  is never more than 1 ulp excluding bias. Functions that specify only
 *  one error value have a symmetric error.
 *
 *  Even though these functions do not provide full precision, the
 *  existence of variants somewhat makes up for that. The 3x3 absolute-value
 *  Sobel gradient provides higher precision than cascading a full-precision
 *  Sobel gradient and an absolute value operation, given that the intermediate
 *  result is 8 bits.
 *
 *  All images must be aligned.
 *
 *  <p>@ref grp_binmorph "Next section: Binary Morphology"</p>
 *
 *  @{
 */

#ifndef RAPP_FILTER_H
#define RAPP_FILTER_H

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
 *  1x2 horizontal difference.
 *  Computes the convolution with the mask [1 -1].
 *  The result is correctly rounded.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_diff_1x2_horz_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height);

/**
 *  1x2 horizontal difference, absolute value.
 *  Computes the absolute value of the horizontal difference.
 *  The result is exact.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_diff_1x2_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                                 const uint8_t *restrict src, int src_dim,
                                 int width, int height);

/**
 *  2x1 vertical difference.
 *  Computes the convolution with the mask [1; -1].
 *  The result is correctly rounded.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_diff_2x1_vert_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height);

/**
 *  2x1 vertical difference, absolute value.
 *  Computes the absolute value of the vertical difference.
 *  The result is exact.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_diff_2x1_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                                 const uint8_t *restrict src, int src_dim,
                                 int width, int height);

/**
 *  2x2 difference magnitude.
 *  Computes the 1-norm of the horizontal and vertical differences.
 *  The result is correctly rounded.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_diff_2x2_magn_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height);

/**
 *  3x3 horizontal Sobel gradient.
 *  Computes the convolution with the mask [1 0 -1; 2 0 -2; 1 0 -1].
 *  The error is within 5/8 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_sobel_3x3_horz_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height);

/**
 *  3x3 horizontal Sobel gradient, absolute value.
 *  Computes the absolute value of the horizontal Sobel gradient.
 *  The error is within 3/4 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_sobel_3x3_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                                  const uint8_t *restrict src, int src_dim,
                                  int width, int height);

/**
 *  3x3 vertical Sobel gradient.
 *  Computes the convolution with the mask [1 2 1; 0 0 0; -1 -2 -1].
 *  The error is within 5/8 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_sobel_3x3_vert_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height);

/**
 *  3x3 vertical Sobel gradient, absolute value.
 *  Computes the absolute value of the vertical Sobel gradient.
 *  The error is within 3/4 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_sobel_3x3_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                                  const uint8_t *restrict src, int src_dim,
                                  int width, int height);

/**
 *  3x3 vertical Sobel gradient magnitude.
 *  Computes the 1-norm of the horizontal and vertical Sobel gradients.
 *  The error is within [-5/4, 3/4] ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_sobel_3x3_magn_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height);

/**
 *  3x3 gaussian smoothing.
 *  Computes convolution with the mask [1 2 1; 2 4 2; 1 2 1].
 *  The error is within 3/4 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_gauss_3x3_u8(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height);

/**
 *  3x3 laplacian.
 *  Computes convolution with the mask [0 -1 0; -1 4 -1; 0 -1 0].
 *  The error is within 1/2 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_laplace_3x3_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height);

/**
 *  3x3 laplacian, absolute value.
 *  Computes absolute value of the laplacian.
 *  The error is within 1/2 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_laplace_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height);

/**
 *  3x3 highpass filter.
 *  Computes convolution with the mask [-1 -1 -1; -1 8 -1; -1 -1 -1].
 *  The error is within 5/8 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_highpass_3x3_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height);

/**
 *  3x3 highpass filter, absolute value.
 *  Computes absolute value of the highpass filter.
 *  The error is within 3/4 ulp.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Source buffer row dimension in bytes.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_filter_highpass_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_FILTER_H */
/** @} */
