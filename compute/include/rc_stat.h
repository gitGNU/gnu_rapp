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
 *  @file   rc_stat.h
 *  @brief  RAPP Compute layer statistics.
 */

#ifndef RC_STAT_H
#define RC_STAT_H

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
 *  Binary pixel sum.
 *
 *  @param[in] buf     Input pixel buffer.
 *  @param     dim     Row dimension of the input buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The computed sum of all pixels.
 */
RC_EXPORT uint32_t
rc_stat_sum_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel sum.
 *
 *  @param[in] buf     Input pixel buffer.
 *  @param     dim     Row dimension of the input buffer.
 *  @param     width   Image width in pixels.
 *  @param     height  Image height in pixels.
 *  @return            The computed sum of all pixels.
 */
RC_EXPORT uint32_t
rc_stat_sum_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel sum and squared sum.
 *
 *  @param[in]  buf     Input pixel buffer.
 *  @param      dim     Row dimension of the input buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @param[out] sum     The computed pixel sum and squared sum.
 */
RC_EXPORT void
rc_stat_sum2_u8(const uint8_t *buf, int dim,
                int width, int height, uintmax_t sum[2]);

/**
 *  8-bit pixel cross sums.
 *
 *  @param[in]  src1      First source pixel buffer.
 *  @param      src1_dim  Row dimension of the first source buffer.
 *  @param[in]  src2      Second source pixel buffer.
 *  @param      src2_dim  Row dimension of the first second buffer.
 *  @param      width     Image width in pixels.
 *  @param      height    Image height in pixels.
 *  @param[out] sum       The computed pixel sums and squared sums
 *                        and cross sum.
 */
RC_EXPORT void
rc_stat_xsum_u8(const uint8_t *restrict src1, int src1_dim,
                const uint8_t *restrict src2, int src2_dim,
                int width, int height, uintmax_t sum[5]);

/**
 *  Binary pixel min.
 *
 *  @param[in]  buf     Input pixel buffer.
 *  @param      dim     Row dimension of the input buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @return             The minimum pixel value in image.
 */
RC_EXPORT int
rc_stat_min_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  Binary pixel max.
 *
 *  @param[in]  buf     Input pixel buffer.
 *  @param      dim     Row dimension of the input buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @return             The maximum pixel value in image.
 */
RC_EXPORT int
rc_stat_max_bin(const uint8_t *buf, int dim, int width, int height);
/* @cond */
#ifdef RAPP_FORCE_EXPORT
/* Used for internal references when tuning. */
int
rc_stat_max_bin__internal(const uint8_t *buf, int dim, int width, int height);
#else
#define rc_stat_max_bin__internal rc_stat_max_bin
#endif
/* @endcond */

/**
 *  8-bit pixel min.
 *
 *  @param[in]  buf     Input pixel buffer.
 *  @param      dim     Row dimension of the input buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @return             The minimum pixel value in image.
 */
RC_EXPORT int
rc_stat_min_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  8-bit pixel max.
 *
 *  @param[in]  buf     Input pixel buffer.
 *  @param      dim     Row dimension of the input buffer.
 *  @param      width   Image width in pixels.
 *  @param      height  Image height in pixels.
 *  @return             The maximum pixel value in image.
 */
RC_EXPORT int
rc_stat_max_u8(const uint8_t *buf, int dim, int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RC_STAT_H */
