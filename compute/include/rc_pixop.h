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
 *  @file   rc_pixop.h
 *  @brief  RAPP Compute layer pixelwise operations
 */

#ifndef RC_PIXOP_H
#define RC_PIXOP_H

#include <stdint.h>
#include "rc_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Set all pixels to a constant value.
 *
 *  @param[out] buf     Pixel buffer process.
 *  @param      dim     Row dimension of the pixel buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @param      value   The value to set, 0-255.
 */
RC_EXPORT void
rc_pixop_set_u8(uint8_t *buf, int dim, int width, int height, unsigned value);

/**
 *  Negate all pixels.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 */
RC_EXPORT void
rc_pixop_not_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Flip the sign bit.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 */
RC_EXPORT void
rc_pixop_flip_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Lookup-table transformation.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param[in]     lut     8-bit lookup table to use.
 */
RC_EXPORT void
rc_pixop_lut_u8(uint8_t *restrict buf, int dim,
                int width, int height, const uint8_t *restrict lut);
/**
 *  Absolute value.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 */
RC_EXPORT void
rc_pixop_abs_u8(uint8_t *buf, int dim, int width, int height);

/**
 *  Add signed constant.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         value   The signed value to add.
 */
RC_EXPORT void
rc_pixop_addc_u8(uint8_t *buf, int dim, int width, int height, int value);

/**
 *  Linear interpolation with a constant.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         value   Value to interpolate with, 0-255.
 *  @param         alpha8  Q.8 blend factor, 0-0x100.
 */
RC_EXPORT void
rc_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                  int height, unsigned value, unsigned alpha8);

/**
 *  Linear interpolation with a constant, rounded towards the constant value.
 *
 *  @param[in,out] buf     Pixel buffer process.
 *  @param         dim     Row dimension of the pixel buffer.
 *  @param         width   Image width in pixels.
 *  @param         height  Image height in pixels.
 *  @param         value   Value to interpolate with, 0-255.
 *  @param         alpha8  Q.8 blend factor, 0-0x100.
 */
RC_EXPORT void
rc_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                   int height, unsigned value, unsigned alpha8);

/*
 * -------------------------------------------------------------
 *  Double-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Saturated addition.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_add_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height);

/**
 *  Average value.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_avg_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height);

/**
 *  Saturated subtraction.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_sub_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height);

/**
 *  Halved subtraction.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_subh_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);
/**
 *  Absolute-value subtraction.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_suba_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);

/**
 *  Linear interpolation.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         alpha8   Q.8 blend factor, 0-0x100.
 */
RC_EXPORT void
rc_pixop_lerp_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height, unsigned alpha8);

/**
 *  Linear interpolation with non-zero update, i.e. rounded towards src.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         alpha8   Q.8 blend factor, 0-0x100.
 */
RC_EXPORT void
rc_pixop_lerpn_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, unsigned alpha8);

/**
 *  Linear interpolation with inverted second operand.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         alpha8   Q.8 blend factor, 0-0x100.
 */
RC_EXPORT void
rc_pixop_lerpi_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, unsigned alpha8);
/**
 *  L1 norm.
 *
 *  @param[in,out] dst      Destination pixel buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source pixel buffer.
 *  @param         src_dim  Row dimension of the source buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 */
RC_EXPORT void
rc_pixop_norm_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RC_PIXOP_H */
