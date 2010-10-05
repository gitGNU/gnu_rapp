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
 *  @file   rc_contour.h
 *  @brief  RAPP Compute layer contour chain code generation.
 */

#ifndef RC_CONTOUR_H
#define RC_CONTOUR_H

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
 *  4-connectivity contour chain code.
 *
 *  @param[out]  origin   Start coordinate of the contour.
 *  @param[out]  contour  Chain code contour string.
 *  @param       len      Length in bytes of the contour string.
 *  @param[in]   buf      Input pixel buffer.
 *  @param       dim      Row dimension of the pixel buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               The length of contour string excluding
 *                        the NUL-termination.
 */
RC_EXPORT int
rc_contour_4conn_bin(unsigned origin[2], char *restrict contour, int len,
                     const uint8_t *restrict buf, int dim,
                     int width, int height);

/**
 *  8-connectivity contour chain code.
 *
 *  @param[out]  origin   Start coordinate of the contour.
 *  @param[out]  contour  Chain code contour string.
 *  @param       len      Length in bytes of the contour string.
 *  @param[in]   buf      Input pixel buffer.
 *  @param       dim      Row dimension of the pixel buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               The length of contour string excluding
 *                        the NUL-termination.
 */
RC_EXPORT int
rc_contour_8conn_bin(unsigned origin[2], char *restrict contour, int len,
                     const uint8_t *restrict buf, int dim,
                     int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RC_CROP_H */
