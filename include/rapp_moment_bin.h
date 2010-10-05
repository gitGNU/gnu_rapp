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
 *  @file   rapp_moment_bin.h
 *  @brief  RAPP binary image moments.
 */

/**
 *  @defgroup grp_moment Moments
 *  @brief 1:st and 2:nd order moments for binary images.
 *
 *  All moments are image-global. The functions may read data up to
 *  the nearest alignment boundary, so if those pixels are not properly
 *  cleared the output may be invalid.
 *
 *  The performance of these functions is input-sensitive, with
 *  the best case occurring for images with all-zero pixels. The worst
 *  case is a dense, but not contiguous pattern, e.g. a checker board
 *  pattern. For actual performance figures, see the benchmark plots.
 *
 *  All images must be aligned.
 *
 *  <p>@ref filter "Next section: Spatial Filtering"</p>
 *
 *  @{
 */

#ifndef RAPP_MOMENT_BIN_H
#define RAPP_MOMENT_BIN_H

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
 *  First order binary image moments.
 *  The result is only valid if it is not greater than 2<sup>32</sup>.
 *
 *  @param[in]   buf    Source pixel buffer.
 *  @param       dim    Row dimension of the source buffer.
 *  @param       width  Image width in pixels.
 *  @param       height Image height in pixels.
 *  @param[out]  mom    Storage array for the moment output with at
 *                      least 3 elements. It is filled in the following way:
 *                        - mom[0]: moment of order 0 (pixel sum)
 *                        - mom[1]: 1:st order moment (x)
 *                        - mom[2]: 1:st order moment (y)
 *  @return             A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_moment_order1_bin(const uint8_t *buf, int dim,
                       int width, int height, uint32_t mom[3]);

/**
 *  First and second order binary image moments.
 *  The 1:st and 2:nd order moments are only valid if they are
 *  not greater than 2<sup>32</sup> and 2<sup>8*sizeof(uintmax_t)</sup>,
 *  respectively.
 *
 *  @param[in]   buf    Source pixel buffer.
 *  @param       dim    Row dimension of the source buffer.
 *  @param       width  Image width in pixels.
 *  @param       height Image height in pixels.
 *  @param[out]  mom    Storage array for the moment output with at
 *                      least 6 elements. It is filled in the following way:
 *                        - mom[0]: moment of order 0 (pixel sum)
 *                        - mom[1]: 1:st order moment (x)
 *                        - mom[2]: 1:st order moment (y)
 *                        - mom[3]: 2:nd order moment (x)
 *                        - mom[4]: 2:nd order moment (y)
 *                        - mom[5]: 2:nd order moment (xy)
 *  @return             A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_moment_order2_bin(const uint8_t *buf, int dim,
                       int width, int height, uintmax_t mom[6]);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_MOMENT_BIN_H */
/** @} */
