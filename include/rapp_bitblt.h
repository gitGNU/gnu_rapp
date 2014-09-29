/*  Copyright (C) 2005-2010, 2014 Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_bitblt.h
 *  @brief  RAPP bitblt operations.
 */

/**
 *  @defgroup grp_bitblt Bitblit Operations
 *  @brief Bitblit operations on images of arbitrary pixel depth.
 *
 *  These functions implement bitblit operations using different
 *  raster operations. The image buffers can be misaligned, but there
 *  may be severe performance penalties if this is the case.
 *
 *  Even though the functions are designated as binary, the bitblit
 *  operations are independent of the pixel bit depth. Simply adjust
 *  the @e width, @e src_off and @e dst_off parameters accordingly.
 *  (Remember, the dimension parameters are always specified in bytes.)
 *  When blitting 8-bit images, the @e width passed should be 8 times
 *  the width of the image. The @e src_off and @e dst_off parameters
 *  should be 0.
 *
 *  The bitblit functions may affect data up to the nearest alignment
 *  boundary of the @e destination image. For the affected data, the
 *  corresponding location in the source image may also be read, so
 *  these functions may actually read data @e beyond the nearest
 *  alignment boundaries of the source image. This can cause problems
 *  when blitting into a sub image of a larger image. In these cases
 *  the caller must set the extra source pixels to the proper value
 *  using the @ref grp_pad "padding" functions. Data will never be
 *  read outside the actual source image memory area. For the pixels
 *  outside this area the value 0 will be used instead.
 *
 *  <p>@ref grp_pixop "Next section: Pixelwise Arithmetic Operations"</p>
 *
 *  @{
 */

#ifndef RAPP_BITBLT_H
#define RAPP_BITBLT_H

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
 *  Bitblt COPY operation.
 *  Computes dst = src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_copy_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                     const uint8_t *restrict src, int src_dim, int src_off,
                     int width, int height);

/**
 *  Bitblt NOT operation.
 *  Computes dst = ~src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_not_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                    const uint8_t *restrict src, int src_dim, int src_off,
                    int width, int height);

/**
 *  Bitblt AND operation.
 *  Computes dst = dst & src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_and_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                    const uint8_t *restrict src, int src_dim, int src_off,
                    int width, int height);

/**
 *  Bitblt OR operation.
 *  Computes dst = dst | src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_or_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                   const uint8_t *restrict src, int src_dim, int src_off,
                   int width, int height);

/**
 *  Bitblt XOR operation.
 *  Computes dst = dst ^ src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_xor_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                   const uint8_t *restrict src, int src_dim, int src_off,
                   int width, int height);

/**
 *  Bitblt NAND operation.
 *  Computes dst = ~(dst & src) for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_nand_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                     const uint8_t *restrict src, int src_dim, int src_off,
                     int width, int height);

/**
 *  Bitblt NOR operation.
 *  Computes dst = ~(dst | src) for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_nor_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                    const uint8_t *restrict src, int src_dim, int src_off,
                    int width, int height);

/**
 *  Bitblt XNOR operation.
 *  Computes dst = ~(dst ^ src) for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_xnor_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                     const uint8_t *restrict src, int src_dim, int src_off,
                     int width, int height);

/**
 *  Bitblt ANDN operation.
 *  Computes dst = dst & ~src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_andn_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                     const uint8_t *restrict src, int src_dim, int src_off,
                     int width, int height);

/**
 *  Bitblt ORN operation.
 *  Computes dst = dst | ~src for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_orn_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                    const uint8_t *restrict src, int src_dim, int src_off,
                    int width, int height);

/**
 *  Bitblt NANDN operation.
 *  Computes dst = ~(dst & ~src) for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_nandn_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                      const uint8_t *restrict src, int src_dim, int src_off,
                      int width, int height);

/**
 *  Bitblt NORN operation.
 *  Computes dst = ~(dst | ~src) for all binary pixels.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param         dst_off  Binary pixel offset of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         src_off  Binary pixel offset of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_bitblt_norn_bin(uint8_t *restrict dst, int dst_dim, int dst_off,
                     const uint8_t *restrict src, int src_dim, int src_off,
                     int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_BITBLT_H */
/** @} */
