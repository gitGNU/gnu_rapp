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
 *  @file   rc_bitblt_wa.c
 *  @brief  Bitblit operations on aligned words.
 */

#include <string.h>        /* memcpy()                 */
#include "rc_impl_cfg.h"   /* Implementation cfg       */
#include "rc_word.h"       /* Word operations          */
#include "rc_util.h"       /* RC_DIV_CEIL()            */
#include "rc_bitblt_rop.h" /* Bitblit word raster ops  */
#include "rc_bitblt_wa.h"  /* Bitblit aligned word API */


/*
 * -------------------------------------------------------------
 *  Template macros
 * -------------------------------------------------------------
 */

/**
 *  The aligned bitblt template.
 */
#define RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, \
                           width, height, rop, unroll) \
do {                                                   \
    int tot = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);      \
    int len = tot / (unroll);                          \
    int rem = tot % (unroll);                          \
    int y;                                             \
    for (y = 0; y < (height); y++) {                   \
        int i = y*(src_dim);                           \
        int j = y*(dst_dim);                           \
        int x;                                         \
                                                       \
        /* Blit all unrolled words */                  \
        for (x = 0; x < len; x++) {                    \
            RC_BITBLT_ITER(dst, src, i, j, rop);       \
            if ((unroll) >= 2) {                       \
                RC_BITBLT_ITER(dst, src, i, j, rop);   \
            }                                          \
            if ((unroll) == 4) {                       \
                RC_BITBLT_ITER(dst, src, i, j, rop);   \
                RC_BITBLT_ITER(dst, src, i, j, rop);   \
            }                                          \
        }                                              \
                                                       \
        /* Blit all remaining words */                 \
        for (x = 0; x < rem; x++) {                    \
            RC_BITBLT_ITER(dst, src, i, j, rop);       \
        }                                              \
    }                                                  \
} while (0)

/**
 *  An aligned bitblt iteration.
 */
#define RC_BITBLT_ITER(dst, src, i, j, rop) \
do {                                        \
    rc_word_t sw, dw;                       \
    sw = RC_WORD_LOAD(&(src)[i]);           \
    dw = RC_WORD_LOAD(&(dst)[j]);           \
    dw = rop(dw, sw);                       \
    RC_WORD_STORE(&(dst)[j], dw);           \
    (i) += RC_WORD_SIZE;                    \
    (j) += RC_WORD_SIZE;                    \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  There used to be "#if RC_IMPL(rc_bitblt_wa_copy_bin, 0)" around
 *  this function, but as long as there are no other implementations and
 *  it is referred to from other functions (rc_integral_sum_bin_*),
 *  it's better to not conditionalize it. Having it unconditional will
 *  be required when e.g. rc_integral_sum_bin_* sprouts tuned
 *  implementations, but until then, it just enables the benchmark to
 *  use RTLD_NOW.  Tuned SIMD implementations of this function may
 *  reasonably yield improved performance for systems where the system
 *  memcpy does not use SIMD instructions. When tuned, there must be an
 *  implementation for each tuning case where it's referred to by other
 *  functions.
 */
void
rc_bitblt_wa_copy_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    int size = RC_DIV_CEIL(width, 8*RC_WORD_SIZE)*RC_WORD_SIZE;
    int y, i, j;

    for (y = 0, i = 0, j = 0;
         y < height;
         y++, i += src_dim, j += dst_dim)
    {
        memcpy(&dst[j], &src[i], size);
    }
}


#if RC_IMPL(rc_bitblt_wa_not_bin, 1)
void
rc_bitblt_wa_not_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NOT,
                       RC_UNROLL(rc_bitblt_wa_not_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_and_bin, 1)
void
rc_bitblt_wa_and_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_AND,
                       RC_UNROLL(rc_bitblt_wa_and_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_or_bin, 1)
void
rc_bitblt_wa_or_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_OR,
                       RC_UNROLL(rc_bitblt_wa_or_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_xor_bin, 1)
void
rc_bitblt_wa_xor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_XOR,
                       RC_UNROLL(rc_bitblt_wa_xor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_nand_bin, 1)
void
rc_bitblt_wa_nand_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NAND,
                       RC_UNROLL(rc_bitblt_wa_nand_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_nor_bin, 1)
void
rc_bitblt_wa_nor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NOR,
                       RC_UNROLL(rc_bitblt_wa_nor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_xnor_bin, 1)
void
rc_bitblt_wa_xnor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_XNOR,
                       RC_UNROLL(rc_bitblt_wa_xnor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_andn_bin, 1)
void
rc_bitblt_wa_andn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_ANDN,
                       RC_UNROLL(rc_bitblt_wa_andn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_orn_bin, 1)
void
rc_bitblt_wa_orn_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_ORN,
                       RC_UNROLL(rc_bitblt_wa_orn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_nandn_bin, 1)
void
rc_bitblt_wa_nandn_bin(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NANDN,
                       RC_UNROLL(rc_bitblt_wa_nandn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wa_norn_bin, 1)
void
rc_bitblt_wa_norn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NORN,
                       RC_UNROLL(rc_bitblt_wa_norn_bin));
}
#endif
