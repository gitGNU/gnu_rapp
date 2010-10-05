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
 *  @file   rc_bitblt_wm.h
 *  @brief  Bitblit operations on misaligned words.
 *
 *  @section Misaligned Bitblt Operations
 *  The following restrictions apply:
 *    - The destination buffer must be word-aligned.
 *    - The source buffer must be misaligned if the bit offset is 0.
 *    - Both row dimensions must be word-aligned.
 *    - The bit offset must be in the range 0 - 7.
 */

#ifndef RC_BITBLT_WM_H
#define RC_BITBLT_WM_H

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
 *  Misaligned bitblit COPY operation.
 *  Computes dst = src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_copy_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset);

/**
 *  Misaligned bitblit NOT operation.
 *  Computes dst = ~src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_not_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset);

/**
 *  Misaligned bitblit AND operation.
 *  Computes dst = dst & src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_and_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset);

/**
 *  Misaligned bitblit OR operation.
 *  Computes dst = dst | src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_or_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, int offset);

/**
 *  Misaligned bitblit XOR operation.
 *  Computes dst = dst ^ src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_xor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset);

/**
 *  Misaligned bitblit NAND operation.
 *  Computes dst = ~(dst & src).
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_nand_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset);

/**
 *  Misaligned bitblit NOR operation.
 *  Computes dst = ~(dst | src).
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_nor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset);

/**
 *  Misaligned bitblit XNOR operation.
 *  Computes dst = ~(dst ^ src).
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_xnor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset);

/**
 *  Misaligned bitblit ANDN operation.
 *  Computes dst = dst & ~src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_andn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset);

/**
 *  Misaligned bitblit ORN operation.
 *  Computes dst = dst | ~src.
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_orn_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset);

/**
 *  Misaligned bitblit NANDN operation.
 *  Computes dst = ~(dst & ~src).
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_nandn_bin(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height, int offset);

/**
 *  Misaligned bitblit NORN operation.
 *  Computes dst = ~(dst | ~src).
 *
 *  @param[in,out] dst      Destination buffer.
 *  @param         dst_dim  Row dimension of the destination buffer.
 *  @param[in]     src      Source buffer.
 *  @param         src_dim  Row dimension of the destination buffer.
 *  @param         width    Image width in pixels.
 *  @param         height   Image height in pixels.
 *  @param         offset   Source buffer bit offset.
 */
RC_EXPORT void
rc_bitblt_wm_norn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset);

#ifdef __cplusplus
};
#endif

#endif /* RC_BITBLT_WM_H */
