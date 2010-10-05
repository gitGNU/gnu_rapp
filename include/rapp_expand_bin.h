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
 *  @file   rapp_expand_bin.h
 *  @brief  RAPP binary 2x expansion.
 */

/**
 *  @defgroup grp_expand Spatial Expansion
 *  Perform 2x spatial expansion (upscaling).
 *  The images must be aligned.
 *
 *  <p>@ref grp_rotate "Next section: Rotations"</p>
 *
 *  @{
 */

#ifndef RAPP_EXPAND_BIN_H
#define RAPP_EXPAND_BIN_H

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
 *  Binary 1x2 expansion.
 *  Upscaling to double size in horizontal direction using pixel
 *  replication. The size of the expanded image is 2*width, height.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the source buffer.
 *  @param       width    Source image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_expand_1x2_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height);

/**
 *  Binary 2x1 expansion.
 *  Upscaling to double size in vertical direction using pixel
 *  replication. The size of the expanded image is width, 2*height.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the source buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Source image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_expand_2x1_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height);

/**
 *  Binary 2x2 expansion.
 *  Upscaling to double size using pixel replication.
 *  The size of the expanded image is 2*width, 2*height.
 *
 *  @param[out]  dst      Destination pixel buffer.
 *  @param       dst_dim  Row dimension of the destination buffer.
 *  @param[in]   src      Source pixel buffer.
 *  @param       src_dim  Row dimension of the source buffer.
 *  @param       width    Source image width in pixels.
 *  @param       height   Source image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_expand_2x2_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_EXPAND_BIN_H */
/** @} */
