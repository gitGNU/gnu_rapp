/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_cond.h
 *  @brief  RAPP conditional set, copy, and add operations.
 */

/**
 *  @defgroup grp_cond Single Conditional Operations
 *  @brief Perform conditional pixelwise set, copy and add operations.
 *
 *  These functions process individual pixels conditionally
 *  based on the value of the corresponding pixels in a binary
 *  map image. A pixel is processed if and only if the pixel in
 *  the binary map is @e set. Performance is highly input-sensitive.
 *  If the map is sparse, i.e. most pixels are @e zero, the operation
 *  will be very fast. A map image of alternating ones and zeros in a
 *  checkerboard may be between one or two magnitudes slower than
 *  the all-zeros case.
 *
 *  Only @e copy, @e set and @e add operations are available as conditional
 *  operations. For more complex conditional processing, use the
 *  @ref cond_gs "gather/scatter" mechanism.
 *
 *  The images must be aligned.
 *
 *  <p>@ref grp_gather "Next section: Gather"</p>
 *
 *  @{
 */

#ifndef RAPP_COND_H
#define RAPP_COND_H

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
 *  Set pixels conditionally.
 *  Sets pixels to a fixed value if the corresponding map pixel is set.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  map      Binary map pixel buffer.
 *  @param      map_dim  Row dimension of the binary map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param      value    The value to set, in the range 0 - 255.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_cond_set_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict map, int map_dim,
                 int width, int height, unsigned value);

/**
 *  Add signed constant conditionally.
 *  Computes buf[i] = buf[i] + value, where map[i] is set.
 *  The result is saturated.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  map      Binary map pixel buffer.
 *  @param      map_dim  Row dimension of the binary map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param      value    Constant term, in the range -255 - 255.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_cond_addc_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict map, int map_dim,
                  int width, int height, int value);

/**
 *  Copy pixels conditionally.
 *  Copies pixels if the corresponding map pixel is set.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Row dimension of the source buffer.
 *  @param[in]  map      Binary map pixel buffer.
 *  @param      map_dim  Row dimension of the binary map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_cond_copy_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  const uint8_t *restrict map, int map_dim,
                  int width, int height);

/**
 *  Add pixels conditionally.
 *  Computes dst[i] = dst[i] + src[i], where map[i] is set.
 *  The result is saturated.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Row dimension of the source buffer.
 *  @param[in]  map      Binary map pixel buffer.
 *  @param      map_dim  Row dimension of the binary map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_cond_add_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 const uint8_t *restrict map, int map_dim,
                 int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_COND_H */
/** @} */
