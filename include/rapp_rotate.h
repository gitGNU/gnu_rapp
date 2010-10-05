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
 *  @file   rapp_rotate.h
 *  @brief  RAPP 8-bit image rotation.
 */

/**
 *  @defgroup grp_rotate Rotations
 *  Perform image rotation by 90 degrees, clockwise and counter-clockwise.
 *  All images must be aligned.
 *
 *  <p>@ref grp_margin "Next section: Logical Margins"</p>
 */

/**
 *  @ingroup grp_rotate
 *  @{
 */

#ifndef RAPP_ROTATE_H
#define RAPP_ROTATE_H

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
 *  Rotate an 8-bit image 90 degrees in clockwise direction.
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
rapp_rotate_cw_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height);

/**
 *  Rotate an 8-bit image 90 degrees in counter-clockwise direction.
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
rapp_rotate_ccw_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_ROTATE_H */
/** @} */
