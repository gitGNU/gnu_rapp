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
 *  @file   rapp_contour.h
 *  @brief  RAPP contour chain code generation.
 */

/**
 *  @defgroup grp_contour Contour Chain Code Generation
 *  @brief Generate a contour chain code from binary image regions.
 *
 *  These functions generate the chain code for the outer contour of a
 *  binary image. The contour is traced in clockwise order, starting at
 *  the first pixel found in traversal order (memory order). Only one
 *  contour is traced. To generate all contours, use the
 *  @ref grp_fill "connected-components seed fill" functions to segment
 *  the image into regions and then trace the contours one by one. The
 *  images must be aligned.
 *
 *  @note The contour tracing algorithm is only limited by zero-pixels,
 *        not by the image boundaries. The caller must set a frame of
 *        zero-pixel around the region to trace, either padding pixels or
 *        pixels in the image, otherwise the contour tracing will trace
 *        out into unallocated memory.
 *
 *  <p>@ref grp_rasterize "Next section: Chain Code Line Rasterization"</p>
 *
 *  @{
 */

#ifndef RAPP_CONTOUR_H
#define RAPP_CONTOUR_H

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
 *  4-connectivity contour chain code.
 *
 *  @param[out]  origin   An array where to store the start position of
 *                        the contour. It must have at least two elements.
 *  @param[out]  contour  Chain code contour buffer.
 *                        May be NULL iff @e len is 0.
 *  @param       len      The size in bytes of the contour buffer.
 *  @param[in]   buf      Pixel buffer of the image.
 *  @param       dim      Row dimension of the pixel buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               The number of codes that should have been written
 *                        to the chain code buffer if it was long enough,
 *                        @e not including the trailing '\\0' character
 *                        (the same semantics as snprintf), or a negative
 *                        error code on error.
 */
RAPP_EXPORT int
rapp_contour_4conn_bin(unsigned origin[2], char *restrict contour, int len,
                       const uint8_t *restrict buf, int dim,
                       int width, int height);

/**
 *  8-connectivity contour chain code.
 *
 *  @param[out]  origin   An array where to store the start position of
 *                        the contour. It must have at least two elements.
 *  @param[out]  contour  Chain code contour buffer.
 *                        May be NULL iff @e len is 0.
 *  @param       len      The size in bytes of the contour buffer.
 *  @param[in]   buf      Pixel buffer of the image.
 *  @param       dim      Row dimension of the pixel buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               The number of codes that should have been written
 *                        to the chain code buffer if it was long enough,
 *                        @e not including the trailing '\\0' character
 *                        (the same semantics as snprintf), or a negative
 *                        error code on error.
 */
RAPP_EXPORT int
rapp_contour_8conn_bin(unsigned origin[2], char *restrict contour, int len,
                       const uint8_t *restrict buf, int dim,
                       int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_CONTOUR_H */
/** @} */
