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
 *  @file   rapp_crop.h
 *  @brief  RAPP binary image cropping.
 */

/**
 *  @defgroup grp_crop Cropping
 *  @brief Compute bounding boxes and start positions of binary images.
 *
 *  These functions compute the bounding coordinates in either
 *  traversal order or in 2D image space that crop image, i.e.
 *  discards zero-pixels. The alignment pixels must be @ref padding
 *  "cleared" before calling any of these functions, or the result may be
 *  invalid. The image must be aligned.
 *
 *  <p>@ref grp_fill "Next section: Connected-Components Seed Fill"</p>
 *
 *  @{
 */

#ifndef RAPP_CROP_H
#define RAPP_CROP_H

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
 *  Find the first non-zero pixel in traversal order.
 *
 *  @param[in]  buf      Pixel buffer.
 *  @param      dim      Row dimension of the buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param[out] pos      An array with at least two elements where to store
 *                       the position found. It is filled in as follows:
 *                         - pos[0]: x coordinate
 *                         - pos[1]: y coordinate
 *  @return              A positive value if the image is non-empty, zero
 *                       if it is empty, and a negative error code on error.
 */
RAPP_EXPORT int
rapp_crop_seek_bin(const uint8_t *buf, int dim, int width, int height,
                   unsigned pos[2]);

/**
 *  Compute the bounding box.
 *
 *  @param[in]  buf      Pixel buffer.
 *  @param      dim      Row dimension of the buffer.
 *  @param      width    Image width in pixels.
 *  @param      height   Image height in pixels.
 *  @param[out] box      An array with at least four elements where to store
 *                       the bounding box. It is filled in as follows:
 *                         - box[0]: x coordinate of upper-left corner
 *                         - box[1]: y coordinate of upper-left corner
 *                         - box[2]: bounding box width
 *                         - box[3]: bounding box height
 *  @return              A positive value if the image is non-empty, zero
 *                       if it is empty, and a negative error code on error.
 */
RAPP_EXPORT int
rapp_crop_box_bin(const uint8_t *buf, int dim, int width, int height,
                  unsigned box[4]);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_CROP_H */
/** @} */
