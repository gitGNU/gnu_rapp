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
 *  @file   rapp_thresh.h
 *  @brief  RAPP thresholding to binary.
 */

/**
 *  @defgroup grp_thresh Thresholding
 *  @brief Thresholding 8-bit to binary.
 *
 *  The thresholding functions convert 8-bit images to binary images,
 *  using one or two thresholds. All images must be aligned.
 *
 *  <p>@ref grp_type "Next section: Type Conversions"</p>
 *
 *  @{
 */

#ifndef RAPP_THRESH_H
#define RAPP_THRESH_H

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
 *  Single thresholding greater-than.
 *  Computes dst[i] = src[i] > thresh.
 *
 *  @param[out]  dst       Destination pixel buffer.
 *  @param       dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]   src       Source pixel buffer.
 *  @param       src_dim   Row dimension in bytes of the source buffer.
 *  @param       width     Image width in pixels.
 *  @param       height    Image height in pixels.
 *  @param       thresh    The threshold value to use.
 *  @return                A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_thresh_gt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int thresh);

/**
 *  Single thresholding less-than.
 *  Computes dst[i] = src[i] < thresh.
 *
 *  @param[out]  dst       Destination pixel buffer.
 *  @param       dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]   src       Source pixel buffer.
 *  @param       src_dim   Row dimension in bytes of the source buffer.
 *  @param       width     Image width in pixels.
 *  @param       height    Image height in pixels.
 *  @param       thresh    The threshold value to use.
 *  @return                A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_thresh_lt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int thresh);

/**
 *  Double thresholding greater-than AND less-than.
 *  Computes dst[i] = src[i] > low && src[i] < high.
 *
 *  @param[out]  dst       Destination pixel buffer.
 *  @param       dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]   src       Source pixel buffer.
 *  @param       src_dim   Row dimension in bytes of the source buffer.
 *  @param       width     Image width in pixels.
 *  @param       height    Image height in pixels.
 *  @param       low       The low threshold value.
 *  @param       high      The high threshold value.
 *  @return                A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_thresh_gtlt_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, int low, int high);

/**
 *  Double thresholding less-than OR greater-than.
 *  Computes dst[i] = src[i] < low || src[i] > high.
 *
 *  @param[out] dst       Destination pixel buffer.
 *  @param      dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]  src       Source pixel buffer.
 *  @param      src_dim   Row dimension in bytes of the source buffer.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @param      low       The low threshold value.
 *  @param      high      The high threshold value.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_thresh_ltgt_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, int low, int high);

/** @} */

#ifdef __cplusplus
};
#endif

#endif /* RAPP_THRESH_H */
