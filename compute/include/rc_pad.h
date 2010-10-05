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
 *  @file   rc_pad.h
 *  @brief  RAPP Compute layer 8-bit image padding.
 */

#ifndef RC_PAD_H
#define RC_PAD_H

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
 *  Pad to the left with a constant value.
 *
 *  @param[in,out] buf     Pixel buffer to pad. May be misaligned.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         size    Number of pixels to pad.
 *  @param         value   The value to set, 0-255.
 */
RC_EXPORT void
rc_pad_const_left_u8(uint8_t *buf, int dim,
                     int width, int height, int size, int value);

/**
 *  Pad to the right with a constant value.
 *
 *  @param[in,out] buf     Pixel buffer to pad. May be misaligned.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         size    Number of pixels to pad.
 *  @param         value   The value to set, 0-255.
 */
RC_EXPORT void
rc_pad_const_right_u8(uint8_t *buf, int dim,
                      int width, int height, int size, int value);

/**
 *  Pad to the left by clamping (replication).
 *
 *  @param[in,out] buf     Pixel buffer to pad. May be misaligned.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         size    Number of pixels to pad.
 */
RC_EXPORT void
rc_pad_clamp_left_u8(uint8_t *buf, int dim,
                     int width, int height, int size);

/**
 *  Pad to the right by clamping (replication).
 *
 *  @param[in,out] buf     Pixel buffer to pad. May be misaligned.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         size    Number of pixels to pad.
 */
RC_EXPORT void
rc_pad_clamp_right_u8(uint8_t *buf, int dim,
                      int width, int height, int size);


#ifdef __cplusplus
};
#endif

#endif /* RC_PAD_H */
