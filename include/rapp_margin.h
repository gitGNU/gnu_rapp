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
 *  @file   rapp_margin.h
 *  @brief  RAPP binary logical margins.
 */

/**
 *  @defgroup grp_margin Logical Margins
 *  @brief Horizontal and vertial logical margins.
 *
 *  Computes the logical @e margins of a binary image. The margin is
 *  defined as the logical AND between all pixels in the rows (vertical)
 *  or in the columns (horizontal). The result is a binary image that
 *  consist only of one row or one column. The main use case is to compute
 *  the vertical margin and divide the image into horizontal blocks
 *  containing non-zero pixels that can be processed efficiently,
 *  while skipping the parts in between.
 *
 *  All images must be aligned.
 *
 *  <p>@ref segmentation "Next section: Segmentation and Labelling"</p>
 *
 *  @{
 */

#ifndef RAPP_MARGIN_H
#define RAPP_MARGIN_H

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
 *  Compute the horizontal logical margin.
 *
 *  @param[out]  margin   Margin pixel buffer with one row of
 *                        at least @e width pixels.
 *  @param[in]   src      Source pixel buffer.
 *  @param       dim      Row dimension of the source buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_margin_horz_bin(uint8_t *restrict margin, const uint8_t *restrict src,
                     int dim, int width, int height);

/**
 *  Compute the vertical logical margin.
 *  The margin is stored as a 1xheight horizontal
 *  image in the @e margin buffer. Since this function may process
 *  pixels up to the nearest alignment boundary, the alignment padding
 *  must be explicitly cleared by the caller or the result may be invalid.
 *
 *  @param[out]  margin   Margin pixel buffer with one row of
 *                        least @e height pixels.
 *  @param[in]   src      Source pixel buffer.
 *  @param       dim      Row dimension of the source buffer.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_margin_vert_bin(uint8_t *restrict margin, const uint8_t *restrict src,
                     int dim, int width, int height);


#ifdef __cplusplus
};
#endif

#endif /* RAPP_MARGIN_H */
/** @} */
