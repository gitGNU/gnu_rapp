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
 *  @file   rc_filter.h
 *  @brief  RAPP Compute layer fixed filters.
 */

#ifndef RC_FILTER_H
#define RC_FILTER_H

#include <stdint.h>
#include "rc_export.h"

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
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_diff_1x2_horz_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height);

/**
 *  1x2 horizontal difference, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_diff_1x2_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height);

/**
 *  2x1 vertical difference.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_diff_2x1_vert_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height);

/**
 *  2x1 vertical difference, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_diff_2x1_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height);

/**
 *  2x2 difference magnitude.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_diff_2x2_magn_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height);

/**
 *  3x3 horizontal Sobel gradient.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_sobel_3x3_horz_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height);

/**
 *  3x3 horizontal Sobel gradient, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_sobel_3x3_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height);

/**
 *  3x3 vertical Sobel gradient.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_sobel_3x3_vert_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height);

/**
 *  3x3 vertical Sobel gradient, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_sobel_3x3_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height);

/**
 *  3x3 Sobel gradient magnitude.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_sobel_3x3_magn_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height);

/**
 *  3x3 gaussian.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_gauss_3x3_u8(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height);

/**
 *  3x3 laplacian.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_laplace_3x3_u8(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height);

/**
 *  3x3 laplacian, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_laplace_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height);

/**
 *  3x3 highpass filter.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_highpass_3x3_u8(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height);

/**
 *  3x3 highpass filter, absolute value.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the destination buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 */
RC_EXPORT void
rc_filter_highpass_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RC_FILTER_H */
