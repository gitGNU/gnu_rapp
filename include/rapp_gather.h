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
 *  @file   rapp_gather.h
 *  @brief  RAPP 8-bit pixel gather.
 */

/**
 *  @defgroup grp_gather Gather
 *  Perform the @e Gather operation with a binary mask.
 *  All images must be aligned.
 *
 *  <p>@ref grp_scatter "Next section: Scatter"</p>
 */

/**
 *  @ingroup grp_gather
 *  @{
 */

#ifndef RAPP_GATHER_H
#define RAPP_GATHER_H

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
 *  Gather 8-bit pixels designated by a binary mask.
 *
 *  @param[out] pack       Pack pixel buffer.
 *  @param      pack_dim   Row dimension of the pack buffer.
 *                         It is only needed if @e rows are more than 1.
 *  @param[in]  src        Source pixel buffer.
 *  @param      src_dim    Row dimension of the source buffer.
 *  @param      map        Map pixel buffer.
 *  @param      map_dim    Row dimension of the map buffer.
 *  @param      width      Image width in pixels.
 *  @param      height     Image height in pixels.
 *  @param      rows       The number of rows to gather. This also determines
 *                         height of the @e pack buffer.
 *  @return                The number of pixels gathered per row, which
 *                         is equal to the number of set pixels in the map,
 *                         a negative error code on error.
 */
RAPP_EXPORT int
rapp_gather_u8(uint8_t *restrict pack, int pack_dim,
               const uint8_t *restrict src, int src_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height, int rows);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_GATHER_H */
/** @} */
