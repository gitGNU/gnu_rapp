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
 *  @file   rapp_pixel.h
 *  @brief  RAPP pixel access functions.
 */

/**
 *  @defgroup grp_pixel Pixel Access
 *  @brief Read and write individual pixels.
 *
 *  These functions are intended for handling individual pixels,
 *  and are not suitable for any kind of processing. The images may
 *  be misaligned.
 *
 *  <p>@ref pixelwise "Next section: Pixelwise Operations"</p>
 *
 *  @{
 */

#ifndef RAPP_PIXEL_H
#define RAPP_PIXEL_H

#include <stdint.h> /* uint8_t */
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
 *  Get a binary pixel value.
 *
 *  @param[in]  buf  A pixel buffer.
 *  @param      dim  Row dimension of the pixel buffer.
 *  @param      off  Binary offset of the pixel buffer.
 *  @param      x    X coordinate of the pixel to read.
 *  @param      y    Y coordinate of the pixel to read.
 *  @return          The binary pixel value,
 *                   or a negative error code on error.
 */
RAPP_EXPORT int
rapp_pixel_get_bin(const uint8_t *buf, int dim, int off, int x, int y);

/**
 *  Set a binary pixel value.
 *
 *  @param[in]  buf    A pixel buffer.
 *  @param      dim    Row dimension of the pixel buffer.
 *  @param      off    Binary offset of the pixel buffer.
 *  @param      x      X coordinate of the pixel to read.
 *  @param      y      Y coordinate of the pixel to read.
 *  @param      value  The binary value to set, zero or one.
 *  @return            A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixel_set_bin(uint8_t *buf, int dim, int off, int x, int y, int value);

/**
 *  Get an 8-bit pixel value.
 *
 *  @param[in]  buf  A pixel buffer.
 *  @param      dim  Row dimension of the pixel buffer.
 *  @param      x    X coordinate of the pixel to read.
 *  @param      y    Y coordinate of the pixel to read.
 *  @return          The 8-bit pixel value,
 *                   or a negative error code on error.
 */
RAPP_EXPORT int
rapp_pixel_get_u8(const uint8_t *buf, int dim, int x, int y);

/**
 *  Set an 8-bit pixel value.
 *
 *  @param[in]  buf    A pixel buffer.
 *  @param      dim    Row dimension of the pixel buffer.
 *  @param      x      X coordinate of the pixel to read.
 *  @param      y      Y coordinate of the pixel to read.
 *  @param      value  The 8-bit value to set, 0-255.
 *  @return            A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixel_set_u8(uint8_t *buf, int dim, int x, int y, int value);


#ifdef __cplusplus
};
#endif

#endif /* RAPP_PIXEL_H */
/** @} */
