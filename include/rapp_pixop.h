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
 *  @file   rapp_pixop.h
 *  @brief  RAPP pixelwise operations.
 */

/**
 *  @defgroup grp_pixop Pixelwise Arithmetic Operations
 *  @brief Pixelwise arithmetic operations on 8-bit images.
 *
 *  These functions operate in-place, i.e. the first operand is also
 *  the destination. All images must be aligned.
 *
 *  <p>@ref grp_thresh "Next section: Thresholding"</p>
 *
 *  @{
 */

#ifndef RAPP_PIXOP_H
#define RAPP_PIXOP_H

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
 *  Set all pixels to a constant value.
 *  Computes buf[i] = value.
 *
 *  @param[out] buf     Pixel buffer pointer.
 *  @param      dim     Pixel buffer row dimension in bytes.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @param      value   The value to set, in the range 0 &ndash; 0xff.
 *  @return             A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_set_u8(uint8_t *buf, int dim,
                  int width, int height, unsigned value);

/**
 *  Negate all pixels.
 *  Computes buf[i] = 0xff - buf[i];
 *
 *  @param[in,out]  buf     Pixel buffer pointer.
 *  @param          dim     Pixel buffer row dimension in bytes.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_not_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Flip the sign bit. It is equivalant to converting
 *  between the formats two's complement and unsigned with bias.
 *  Computes buf[i] = buf[i] ^ 0x80;
 *
 *  @param[in,out]  buf     Pixel buffer pointer.
 *  @param          dim     Pixel buffer row dimension in bytes.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_flip_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Lookup-table transformation.
 *  Computes buf[i] = lut[buf[i]].
 *
 *  @param[in,out] buf     Pixel buffer pointer.
 *  @param         dim     Pixel buffer row dimension in bytes.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         lut     The 8-bit lookup table to use, with at least
 *                         256 bytes. It may be misaligned.
 *  @return                A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lut_u8(uint8_t *restrict buf, int dim,
                  int width, int height, const uint8_t *restrict lut);

/**
 *  Absolute value.
 *  Computes 2*abs(buf[i] - 0x80). The result is saturated.
 *
 *  @param[in,out] buf     Pixel buffer pointer.
 *  @param         dim     Pixel buffer row dimension in bytes.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @return                A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_abs_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Add signed constant.
 *  Computes buf[i] = buf[i] + value. The result is saturated.
 *
 *  @param[in,out]  buf     Pixel buffer pointer.
 *  @param          dim     Pixel buffer row dimension in bytes.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          value   The value to add.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_addc_u8(uint8_t *buf, int dim, int width, int height, int value);

/**
 *  Linear interpolation with constant.
 *  Computes buf[i] += alpha8*(value - buf[i]),
 *  where the multiplication is a rounded 8-bit fixed-point multiply.
 *
 *  @param[in,out]  buf     Pixel buffer pointer.
 *  @param          dim     Pixel buffer row dimension in bytes.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          value   The value to interpolate with,
 *                          in the range 0 &ndash; 0xff.
 *  @param          alpha8  The Q.8 fixed-point blend factor,
 *                          in the range 0 &ndash; 0x100.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                    int height, unsigned value, unsigned alpha8);

/**
 *  Linear interpolation with a constant, rounded towards the constant value.
 *  Computes buf[i] += alpha8*(value - buf[i]),
 *  where the multiplication is an 8-bit fixed-point multiply,
 *  rounded away from zero.
 *
 *  @param[in,out]  buf     Pixel buffer pointer.
 *  @param          dim     Pixel buffer row dimension in bytes.
 *  @param          width   Image width in pixels.
 *  @param          height  Image height in pixels.
 *  @param          value   The value to interpolate with,
 *                          in the range 0 &ndash; 0xff.
 *  @param          alpha8  The Q.8 fixed-point blend factor,
 *                          in the range 0 &ndash; 0x100.
 *  @return                 A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                     int height, unsigned value, unsigned alpha8);

/**
 *  Copy all pixels.
 *  Computes dst[i] = src[i].
 *
 *  @param[out]  dst      Destination buffer pointer.
 *  @param       dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]   src      Source buffer pointer.
 *  @param       src_dim  Source buffer row dimension in bytes.
 *  @param       width    Image width in pixels.
 *  @param       height   Image height in pixels.
 *  @return               A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_copy_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height);

/**
 *  Saturated addition.
 *  Computes dst[i] += src[i]. The result is saturated.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_add_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height);

/**
 *  Average value.
 *  Computes dst[i] = (dst[i] + src[i] + 1) / 2.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_avg_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height);

/**
 *  Saturated subtraction.
 *  Computes dst[i] -= src[i]. The result is saturated.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_sub_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height);

/**
 *  Halved subtraction.
 *  Computes dst[i] = (dst[i] - src[i] + 0x100) / 2.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_subh_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height);

/**
 *  Absolute-value subtraction.
 *  Computes dst[i] = abs(dst[i] - src[i]).
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_suba_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height);

/**
 *  Linear interpolation.
 *  Computes dst[i] += alpha8*(src[i] - dst[i]),
 *  where the multiplication is a rounded 8-bit fixed-point multiply.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @param          alpha8   The Q.8 fixed-point blend factor,
 *                           in the range 0 &ndash; 0x100.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lerp_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height, unsigned alpha8);

/**
 *  Linear interpolation with non-zero update, i.e., rounded towards src.
 *  Computes dst[i] += alpha8*(src[i] - dst[i]),
 *  where the multiplication is an 8-bit fixed-point multiply,
 *  rounded away from zero.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @param          alpha8   The Q.8 fixed-point blend factor,
 *                           in the range 0 &ndash; 0x100.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lerpn_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, unsigned alpha8);

/**
 *  Linear interpolation with inverted second operand,
 *  or generalized subtraction.
 *  Computes dst[i] += alpha8*(0xff - src[i] - dst[i]),
 *  where the multiplication is a rounded 8-bit fixed-point multiply.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @param          alpha8   The Q.8 fixed-point blend factor,
 *                           in the range 0 &ndash; 0x100.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_lerpi_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, unsigned alpha8);

/**
 *  L1 norm.
 *  Computes dst[i] = (abs(dst[i]) + abs(src[i]) + 1) / 2.
 *
 *  @param[in,out]  dst      Destination buffer pointer.
 *  @param          dst_dim  Destination buffer row dimension in bytes.
 *  @param[in]      src      Source buffer pointer.
 *  @param          src_dim  Source buffer row dimension in bytes.
 *  @param          width    Image width in pixels.
 *  @param          height   Image height in pixels.
 *  @return                  A negative error code on error, zero otherwise.
 */
RAPP_EXPORT int
rapp_pixop_norm_u8(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_PIXOP_H */
/** @} */
