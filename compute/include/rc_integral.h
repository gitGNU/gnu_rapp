/*  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
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

#ifndef RC_INTEGRAL_H
#define RC_INTEGRAL_H

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
 *  Integral image: 8-bit source to 16-bit integral.
 */
RC_EXPORT void
rc_integral_sum_u8_u16(uint16_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height);
/**
 *  Integral image: 8-bit source to 32-bit integral.
 */
RC_EXPORT void
rc_integral_sum_u8_u32(uint32_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height);

/**
 *  Integral histogram: 8-bit bin indices to 8-bit integral.
 */
RC_EXPORT void
rc_integral_hist_u8(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict bin, int bin_dim,
                    int width, int height, int num_bins);

/**
 *  Integral histogram: 8-bit bin indices to 16-bit integral.
 */
RC_EXPORT void
rc_integral_hist_u16(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict bin, int bin_dim,
                     int width, int height, int num_bins);


/**
 *  Integral histogram: 8-bit bin indices to 32-bit integral.
 */
RC_EXPORT void
rc_integral_hist_u32(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict bin, int bin_dim,
                     int width, int height, int num_bins);

#ifdef __cplusplus
};
#endif

#endif /* RC_INTEGRAL_H */
