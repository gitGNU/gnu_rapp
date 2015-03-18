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
 *  @file   rapp_type.h
 *  @brief  RAPP type conversions.
 */

/**
 *  @defgroup grp_type Type Conversions
 *  @brief Convert between pixel depths.
 *
 *  These functions convert images between different pixel depths,
 *  e.g. between binary and 8-bit images. The binary pixel values
 *  0 and 1 values will be converted to the 8-bit values 0 and 255,
 *  respectively. An 8-bit value of 128 or greater will be converted
 *  to binary 1, and to binary 0 below that value. This is equivalent
 *  of calling rapp_thresh_gt_u8() with the threshold value 127.
 *
 *  To convert between 8-bit unsigned-with-bias to 8-bit two's complement,
 *  use the rapp_pixop_flip_u8() operation.
 *
 *  All images must be aligned.
 *
 *  <p>@ref statistics "Next section: Statistical Operations"</p>
 *
 *  @{
 */

#ifndef RAPP_TYPE_H
#define RAPP_TYPE_H

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
 *  Convert u8 pixels to binary pixels.
 *
 *  @param[out] dst       Destination pixel buffer.
 *  @param      dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]  src       Source pixel buffer.
 *  @param      src_dim   Row dimension in bytes of the source buffer.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_type_u8_to_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height);

/**
 *  Convert binary pixels to u8 pixels.
 *
 *  @param[out] dst       Destination pixel buffer.
 *  @param      dst_dim   Row dimension in bytes of the destination buffer.
 *  @param[in]  src       Source pixel buffer.
 *  @param      src_dim   Row dimension in bytes of the source buffer.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_type_bin_to_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_TYPE_H */
/** @} */
