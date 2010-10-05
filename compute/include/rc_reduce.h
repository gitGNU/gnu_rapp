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
 *  @file   rc_reduce.h
 *  @brief  RAPP Compute layer 8-bit reduction.
 */

#ifndef RC_REDUCE_H
#define RC_REDUCE_H

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
 *  8-bit 1x2 (horizontal) reduction.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Row dimension of the source buffer.
 *  @param      width    Source image width in pixels.
 *  @param      height   Image height in pixels.
 */
RC_EXPORT void
rc_reduce_1x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);

/**
 *  8-bit 2x1 (vertical) reduction.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Row dimension of the source buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Source image height in pixels.
 */
RC_EXPORT void
rc_reduce_2x1_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);

/**
 *  8-bit 2x2 reduction.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Row dimension of the destination buffer.
 *  @param[in]  src      Source pixel buffer.
 *  @param      src_dim  Row dimension of the source buffer.
 *  @param      width    Source image width in pixels.
 *  @param      height   Source image height in pixels.
 */
RC_EXPORT void
rc_reduce_2x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);


#ifdef __cplusplus
};
#endif

#endif /* RC_REDUCE_H */
