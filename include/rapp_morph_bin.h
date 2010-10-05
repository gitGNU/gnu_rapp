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
 *  @file   rapp_morph_bin.h
 *  @brief  RAPP binary morphology.
 */

/**
 *  @defgroup grp_binmorph Binary Morphology
 *
 *  @section Overview
 *  These functions perform dilation and erosion on binary images
 *  using pre-defined structuring elements (SEs for short). Four
 *  different SE shapes are available:
 *
 *    -# Rectangular solid bricks with sides 1 - 63 pixels.
 *    -# Right-angle diamonds with a radius between 2 and 32 pixels.
 *    -# Regular octagons with a radius between 2 and 32 pixels.
 *    -# Discs with a radius between 2 and 32 pixels.
 *
 *  By @e radius we mean half the width of the axis-aligned bounding
 *  box of the shape, rounded up to the nearest integer. For example,
 *  a 3x3 square is said to have the radius 2.
 *
 *  The rectangle and diamond shapes are exact. Octagons cannot be
 *  represented exactly on the pixel grid, but they are optimal
 *  approximations with respect to the euclidian error. The disc shapes
 *  are sub-optimal approximations, meaning that the error is greater
 *  than the minimum error. The error is nevertheless small &ndash;
 *  around one pixel for larger discs.
 *
 *  @section Performance
 *  The various structuring element shapes are decomposed into smaller
 *  operations using separation and logarithmic decomposition. This,
 *  combined with the bit-parallel operations possible on binary images,
 *  makes it feasible to use very large structuring elements. See
 *  the @ref benchmarks "benchmarks" for actual performance figures.
 *
 *  @section Usage
 *  All functions need a user-allocated working buffer.
 *  The minimum size in bytes of the buffer is given by
 *  rapp_morph_worksize_bin(), and must be aligned on #rapp_alignment
 *  boundaries. As with most other RAPP functions, the images must
 *  also be aligned.
 *
 *  @section Padding
 *  The user is responsible for @ref padding "padding" the source buffer.
 *  The padding needed is bounded by min(size / 2, 16), where size
 *  is the maximum size of the structuring element in the horizontal
 *  and vertical directions. If the source buffer is padded with
 *  values other than all-zeros or all-ones the behaviour is undefined.
 *
 *  <p>@ref padding "Next section: Border Padding"</p>
 *
 *  @{
 */

#ifndef RAPP_MORPH_BIN_H
#define RAPP_MORPH_BIN_H

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
 *  Compute the minimum size of the working buffer needed by the
 *  binary morphological functions.
 *
 *  @param width  The image width in pixels.
 *  @param height The image height in pixels.
 *  @return       The minimum buffer size in bytes,
 *                or a negative error code on error.
 */
RAPP_EXPORT int
rapp_morph_worksize_bin(int width, int height);


/**
 *  Erosion with a rectangular structuring element.
 *  All SE sizes up to and including 63x63 pixels are supported,
 *  except the 1x1 degenerate case.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      wrect    The width of the SE rectangle, in the range 2 - 63.
 *  @param      hrect    The height of the SE rectangle, in the range 2 - 63.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_erode_rect_bin(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height, int wrect, int hrect,
                          void *restrict work);

/**
 *  Dilation with a rectangular structuring element.
 *  All SE sizes up to and including 63x63 pixels are supported,
 *  except the 1x1 degenerate case.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      wrect    The width of the SE rectangle, in the range 2 - 63.
 *  @param      hrect    The height of the SE rectangle, in the range 2 - 63.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_dilate_rect_bin(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height, int wrect, int hrect,
                           void *restrict work);

/**
 *  Erosion with a diamond structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_erode_diam_bin(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height, int radius,
                          void *restrict work);

/**
 *  Dilation with a diamond structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_dilate_diam_bin(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height, int radius,
                           void *restrict work);

/**
 *  Erosion with an octagon structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_erode_oct_bin(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height, int radius,
                         void *restrict work);

/**
 *  Dilation with an octagon structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_dilate_oct_bin(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height, int radius,
                          void *restrict work);

/**
 *  Erosion with a approximately disc-shaped structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_erode_disc_bin(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height, int radius,
                          void *restrict work);

/**
 *  Dilation with an approximately disc-shaped structuring element.
 *  All SE radii from 2 up to 32 pixels are supported.
 *
 *  @param[out] dst      Destination pixel buffer.
 *  @param      dst_dim  Destination buffer row dimension.
 *  @param[in]  src      Source pixel buffer with padding.
 *  @param      src_dim  Source buffer row dimension.
 *  @param      width    The image width in pixels.
 *  @param      height   The image height in pixels.
 *  @param      radius   The diamond radius, in the range 2 - 32.
 *  @param      work     Internal working buffer.
 *  @return              A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_morph_dilate_disc_bin(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height, int radius,
                           void *restrict work);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_MORPH_BIN_H */
/** @} */
