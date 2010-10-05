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
 *  @file   rapp_pad_bin.h
 *  @brief  RAPP binary image padding.
 */

/**
 *  @ingroup grp_pad
 *  @{
 */

#ifndef RAPP_PAD_BIN_H
#define RAPP_PAD_BIN_H

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
 *  Sets the alignment pixels to value. The alignment pixels consists of
 *  the pixels to the left (including the binary offset) and right of the
 *  actual image area, up to the nearest alignment boundary.
 *
 *  @param[in,out]  buf     Pixel buffer, possibly misaligned.
 *  @param          dim     Row dimension of the buffer.
 *  @param          off     Bit offset of the buffer, in the range 0-7.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          value   The boolean value to set.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pad_align_bin(uint8_t *buf, int dim, int off,
                   int width, int height, int value);

/**
 *  Pad a binary image with a constant value.
 *
 *  @param[in,out]  buf     Pixel buffer, possibly misaligned.
 *  @param          dim     Row dimension of the buffer.
 *  @param          off     Bit offset of the buffer, in the range 0-7.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          size    Size of the padding in pixels.
 *  @param          set     The boolean value to set.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pad_const_bin(uint8_t *buf, int dim, int off,
                   int width, int height, int size, int set);

/**
 *  Pad a binary image by clamping the edge value.
 *
 *  @param[in,out]  buf     Pixel buffer, possibly misaligned.
 *  @param          dim     Row dimension of the buffer.
 *  @param          off     Bit offset of the buffer, in the range 0-7.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          size    Size of the padding in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pad_clamp_bin(uint8_t *buf, int dim, int off,
                   int width, int height, int size);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_PAD_BIN_H */
/** @} */
