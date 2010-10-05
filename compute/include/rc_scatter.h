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
 *  @file   rc_scatter.h
 *  @brief  RAPP Compute layer 8-bit scatter.
 */

#ifndef RC_SCATTER_H
#define RC_SCATTER_H

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
 *  8-bit image scatter.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  map      Binary map pixel buffer.
 *  @param      map_dim  Row dimension of the map buffer.
 *  @param[in]  pack     Pixel buffer with pixels to scatter.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @return              The number of pixels scattered.
 */
RC_EXPORT int
rc_scatter_u8(uint8_t *restrict dst, int dst_dim,
              const uint8_t *restrict map, int map_dim,
              const uint8_t *restrict pack, int width, int height);


#ifdef __cplusplus
};
#endif

#endif /* RC_SCATTER_H */
