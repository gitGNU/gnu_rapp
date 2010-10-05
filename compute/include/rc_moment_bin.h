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
 *  @file   rc_moment_bin.h
 *  @brief  RAPP Compute layer binary image moments.
 */

#ifndef RC_MOMENT_BIN_H
#define RC_MOMENT_BIN_H

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
 *  First order binary image moments.
 *
 *  @param[in]   buf     Input pixel buffer.
 *  @param       dim     Row dimension of the input buffer.
 *  @param       width   Image width in pixels.
 *  @param       height  Image height in pixels.
 *  @param[out]  mom     Computed 1:st order moments.
 */
RC_EXPORT void
rc_moment_order1_bin(const uint8_t *buf, int dim,
                     int width, int height, uint32_t mom[3]);

/**
 *  Second order binary image moments.
 *
 *  @param[in]   buf     Input pixel buffer.
 *  @param       dim     Row dimension of the input buffer.
 *  @param       width   Image width in pixels.
 *  @param       height  Image height in pixels.
 *  @param[out]  mom     Computed 2:nd order moments.
 */
RC_EXPORT void
rc_moment_order2_bin(const uint8_t *buf, int dim,
                     int width, int height, uintmax_t mom[6]);


#ifdef __cplusplus
};
#endif

#endif /* RC_MOMENT_H */
