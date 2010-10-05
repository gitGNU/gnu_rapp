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
 *  @file   rc_integral_bin.c
 *  @brief  Integral image functions for binary images.
 */
#include <stdio.h>

#include "rc_word.h"         /* Word operations  */
#include "rc_stat.h"         /* Image statistics */
#include "rc_bitblt_wa.h"    /* Aligned bitblits */
#include "rc_integral_bin.h"

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Binary integral image template.
 */
#define RC_INTEGRAL_SUM_BIN(dst, dst_dim, src, src_dim, width, height)       \
do {                                                                         \
    int dim  = (dst_dim) / sizeof *(dst);    /* Dst dim in pixels          */\
    int full = (width) / (8 * RC_WORD_SIZE); /* Number of full src words   */\
    int tail = (width) % (8 * RC_WORD_SIZE); /* Number of remaining pixels */\
    int y;                                                                   \
                                                                             \
    /* Process all rows */                                                   \
    for (y = 0; y < (height); y++) {                                         \
        int i  = y  * (src_dim);       /* Src image index                  */\
        int j2 = y  *  dim;            /* Dst image index                  */\
        int j1 = j2 -  dim;            /* Dst image index for previous row */\
                                                                             \
        if (rc_stat_max_bin__internal(&(src)[i], 0, width, 1) == 0) {        \
            /* Empty src row - duplicate the previous integral row */        \
            rc_bitblt_wa_copy_bin((uint8_t*)&dst[j2], dst_dim,               \
                                  (const uint8_t*)&dst[j1],                  \
                                  dst_dim, 8 * sizeof *(dst) * (width), 1);  \
        }                                                                    \
        else {                                                               \
            uint32_t upleft = (dst)[j1 - 1]; /* Up-left pixel integral sum */\
            uint32_t left   = (dst)[j2 - 1]; /* Left pixel integral sum    */\
            int      k;                                                      \
            /* Handle all full src words */                                  \
            for (k = 0; k < full; k++, i += RC_WORD_SIZE) {                  \
                RC_INTEGRAL_SUM_WORD(dst, &src[i], 8 * RC_WORD_SIZE,         \
                                     upleft, left, j1, j2);                  \
            }                                                                \
            /* Handle the remaining partial src word */                      \
            RC_INTEGRAL_SUM_WORD(dst, &src[i], tail, upleft, left, j1, j2);  \
        }                                                                    \
    }                                                                        \
} while (0)

/**
 *  Integrate a binary image word.
 */
#define RC_INTEGRAL_SUM_WORD(dst, src, len, upleft, left, j1, j2)            \
do {                                                                         \
    rc_word_t word = RC_WORD_LOAD(src);                                      \
    int       b;                                                             \
    for (b = 0; b < len; b++, j1++, j2++) {                                  \
        uint32_t pix = RC_WORD_EXTRACT(word, b, 1); /* The src pixel value */\
        uint32_t up  = (dst)[j1];           /* The integral value above us */\
        uint32_t cur = pix + left + (up) - (upleft); /* New integral value */\
        (dst)[j2] = cur;                    /* Store the new value in dst  */\
        (left)    = cur;                    /* Update the sliding 'left'   */\
        (upleft)  = up;                     /* Update the sliding 'upleft' */\
     }                                                                       \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Integral image: binary source to 8-bit integral.
 */
void
rc_integral_sum_bin_u8(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_INTEGRAL_SUM_BIN(dst, dst_dim, src, src_dim, width, height);
}

/**
 *  Integral image: binary source to 16-bit integral.
 */
void
rc_integral_sum_bin_u16(uint16_t *restrict dst, int dst_dim,
                        const uint8_t *restrict src, int src_dim,
                        int width, int height)
{
    RC_INTEGRAL_SUM_BIN(dst, dst_dim, src, src_dim, width, height);
}

/**
 *  Integral image: binary source to 32-bit integral.
 */
void
rc_integral_sum_bin_u32(uint32_t *restrict dst, int dst_dim,
                        const uint8_t *restrict src, int src_dim,
                        int width, int height)
{
    RC_INTEGRAL_SUM_BIN(dst, dst_dim, src, src_dim, width, height);
}
