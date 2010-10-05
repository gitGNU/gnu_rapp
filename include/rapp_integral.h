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
 *  @file   rapp_integral.h
 *  @brief  RAPP integral images.
 */

/**
 *  @defgroup grp_integral Integral Images
 *  @brief Integral image calculations for binary and 8-bit images.
 *
 *  The functions may read data up to the nearest alignment boundary,
 *  so if those pixels are not properly cleared the output may be invalid.
 *
 *  All images must be aligned.
 *
 *  <p>@ref license "Next section: License"</p>
 *  @{
 */

#ifndef RAPP_INTEGRAL_H
#define RAPP_INTEGRAL_H

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
 *  Integral image.
 *  Compute the 8-bit integral image from a binary source.
 *
 *  @param[out] dst       Integral image buffer.
 *  @param      dst_dim   Row dimension of the integral image.
 *  @param[in]  src       Source image buffer.
 *  @param      src_dim   Row dimension of the source image.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_integral_sum_bin_u8(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height);

/**
 *  Integral image.
 *  Compute the 16-bit integral image from a binary source.
 *
 *  @param[out] dst       Integral image buffer.
 *  @param      dst_dim   Row dimension of the integral image.
 *  @param[in]  src       Source image buffer.
 *  @param      src_dim   Row dimension of the source image.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_integral_sum_bin_u16(uint16_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height);

/**
 *  Integral image.
 *  Compute the 32-bit integral image from a binary source.
 *
 *  @param[out] dst       Integral image buffer.
 *  @param      dst_dim   Row dimension of the integral image.
 *  @param[in]  src       Source image buffer.
 *  @param      src_dim   Row dimension of the source image.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_integral_sum_bin_u32(uint32_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height);

/**
 *  Integral image.
 *  Compute the 16-bit integral image from an 8-bit source.
 *
 *  @param[out] dst       Integral image buffer.
 *  @param      dst_dim   Row dimension of the integral image.
 *  @param[in]  src       Source image buffer.
 *  @param      src_dim   Row dimension of the source image.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_integral_sum_u8_u16(uint16_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height);

/**
 *  Integral image.
 *  Compute the 32-bit integral image from an 8-bit source.
 *
 *  @param[out] dst       Integral image buffer.
 *  @param      dst_dim   Row dimension of the integral image.
 *  @param[in]  src       Source image buffer.
 *  @param      src_dim   Row dimension of the source image.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_integral_sum_u8_u32(uint32_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_INTEGRAL_H */
/** @} */
