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

/**
 *  @file   rc_integral.c
 *  @brief  Integral image functions.
 */

#include "rc_platform.h"
#include "rc_integral.h"


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  8-bit integral image template.
 */
#define RC_INTEGRAL_SUM_U8(dst, dst_dim, src, src_dim, width, height)       \
do {                                                                        \
    int dim_ = (dst_dim) / sizeof *(dst);                                   \
    int x_, y_;                                                             \
    for (y_ = 0; y_ < (height); y_++) {                                     \
        int i_  = y_  * (src_dim);   /* Src image index                  */ \
        int j2_ = y_  *  dim_;       /* Dst image index                  */ \
        int j1_ = j2_ -  dim_;       /* Dst image index for previous row */ \
                                                                            \
        uint32_t upleft_ = (dst)[j1_ - 1]; /* Up-left pixel integral sum */ \
        uint32_t left_   = (dst)[j2_ - 1]; /* Left pixel integral sum    */ \
                                                                            \
        for (x_ = 0; x_ < (width); x_++, i_++, j1_++, j2_++) {              \
            uint32_t up_  = (dst)[j1_];  /* The value above us           */ \
            uint32_t val_ = left_ + (src)[i_] + up_ - upleft_;              \
            (dst)[j2_] = val_;           /* Store the new value in dst   */ \
            left_      = val_;           /* Update the sliding 'left'    */ \
            upleft_    = up_;            /* Update the sliding 'upleft'  */ \
        }                                                                   \
    }                                                                       \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Integral image: 8-bit source to 16-bit integral.
 */
void
rc_integral_sum_u8_u16(uint16_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_INTEGRAL_SUM_U8(dst, dst_dim, src, src_dim, width, height);
}

/**
 *  Integral image: 8-bit source to 32-bit integral.
 */
void
rc_integral_sum_u8_u32(uint32_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_INTEGRAL_SUM_U8(dst, dst_dim, src, src_dim, width, height);
}
