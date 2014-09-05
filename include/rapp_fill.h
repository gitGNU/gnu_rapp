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
 *  @file   rapp_fill.h
 *  @brief  RAPP connected-components seed fill.
 */

/**
 *  @defgroup grp_fill Connected-Components Seed Fill
 *  @brief 4- and 8-connectivity seed fill on binary images.
 *
 *  The input of these functions is a binary map image, and a seed pixel
 *  position. The functions set pixels in the destination image that
 *  are connected to the seed pixel in the map image. All other pixels
 *  in the destination image are set to zero. The alignment pixels must
 *  be cleared before calling any of these functions, or the result may
 *  be invalid. The images must be aligned.
 *
 *  <p>@ref chaincode "Next section: Chain Codes"</p>
 *
 *  @{
 */

#ifndef RAPP_FILL_H
#define RAPP_FILL_H

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
 *  4-connectivity connected-components seed fill.
 *
 *  @param[out] dst      Destination buffer.
 *  @param      dst_dim  Row dimension in bytes of the destination buffer.
 *  @param[in]  map      Source map buffer containing the binary object to fill.
 *  @param      map_dim  Row dimension in bytes of map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param      xseed    X-coordinate of the seed point where the fill starts.
 *  @param      yseed    Y-coordinate of the seed point where the fill starts.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_fill_4conn_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict map, int map_dim,
                    int width, int height, int xseed, int yseed);

/**
 *  8-connectivity connected-components seed fill.
 *
 *  @param[out] dst      Destination buffer.
 *  @param      dst_dim  Row dimension in bytes of the destination buffer.
 *  @param[in]  map      Source map buffer containing the binary object to fill.
 *  @param      map_dim  Row dimension in bytes of map buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param      xseed    X-coordinate of the seed point where the fill starts.
 *  @param      yseed    Y-coordinate of the seed point where the fill starts.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_fill_8conn_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict map, int map_dim,
                    int width, int height, int xseed, int yseed);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_FILL_H */
/** @} */
