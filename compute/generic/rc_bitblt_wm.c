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
 *  @file   rc_bitblt_wm.c
 *  @brief  Bitblit operations on misaligned words.
 */

#include "rc_impl_cfg.h"   /* Implementation cfg          */
#include "rc_word.h"       /* Word operations             */
#include "rc_util.h"       /* RC_DIV_CEIL()               */
#include "rc_bitblt_rop.h" /* Bitblit word raster ops     */
#include "rc_bitblt_wm.h"  /* Bitblit misaligned word API */


/*
 * -------------------------------------------------------------
 *  Template macros
 * -------------------------------------------------------------
 */

/**
 *  The misaligned bitblt template.
 */
#define RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim,               \
                           width, height, offset, rop, unroll)       \
do {                                                                 \
    int tot = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);                    \
    int len = tot / (unroll);   /* Fully unrolled       */           \
    int rem = tot % (unroll);   /* Remaining words      */           \
    int sh1, sh2;               /* Alignment bit shifts */           \
    int y;                                                           \
                                                                     \
    /* Compute the alignment bit shifts */                           \
    sh1 = 8*((uintptr_t)(src) % RC_WORD_SIZE) + (offset);            \
    sh2 = 8*RC_WORD_SIZE - sh1;                                      \
                                                                     \
    /* Align the source pointer */                                   \
    (src) = (const uint8_t *restrict)((intptr_t)(src) &              \
                                      ~(RC_WORD_SIZE - 1));          \
    for (y = 0; y < (height); y++) {                                 \
        rc_word_t prev;                                              \
        int       i = y*(src_dim);                                   \
        int       j = y*(dst_dim);                                   \
        int       x;                                                 \
                                                                     \
        /* Initialize the word cache with the first src word */      \
        prev = RC_WORD_LOAD(&(src)[i]);                              \
        i   += RC_WORD_SIZE;                                         \
                                                                     \
        /* Blit all unrolled words */                                \
        for (x = 0; x < len; x++) {                                  \
            RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop);     \
            if ((unroll) >= 2) {                                     \
                RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop); \
            }                                                        \
            if ((unroll) == 4) {                                     \
                RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop); \
                RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop); \
            }                                                        \
        }                                                            \
                                                                     \
        /* Blit all remaining words */                               \
        for (x = 0; x < rem; x++) {                                  \
            RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop);     \
        }                                                            \
    }                                                                \
} while (0)

/**
 *  A misaligned bitblt iteration.
 */
#define RC_BITBLT_ITER(dst, src, i, j, prev, sh1, sh2, rop) \
do {                                                        \
    rc_word_t sw, aw, dw;                                   \
    sw = RC_WORD_LOAD(&(src)[i]);                           \
    dw = RC_WORD_LOAD(&(dst)[j]);                           \
    aw = RC_WORD_SHL(prev, sh1) |                           \
         RC_WORD_SHR(sw,   sh2);                            \
    dw = rop(dw, aw);                                       \
    RC_WORD_STORE(&(dst)[j], dw);                           \
    (prev) = sw;                                            \
    (i)   += RC_WORD_SIZE;                                  \
    (j)   += RC_WORD_SIZE;                                  \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

#if RC_IMPL(rc_bitblt_wm_copy_bin, 1)
void
rc_bitblt_wm_copy_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_COPY,
                       RC_UNROLL(rc_bitblt_wm_copy_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_not_bin, 1)
void
rc_bitblt_wm_not_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_NOT,
                       RC_UNROLL(rc_bitblt_wm_not_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_and_bin, 1)
void
rc_bitblt_wm_and_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_AND,
                       RC_UNROLL(rc_bitblt_wm_and_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_or_bin, 1)
void
rc_bitblt_wm_or_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_OR,
                       RC_UNROLL(rc_bitblt_wm_or_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_xor_bin, 1)
void
rc_bitblt_wm_xor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_XOR,
                       RC_UNROLL(rc_bitblt_wm_xor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_nand_bin, 1)
void
rc_bitblt_wm_nand_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_NAND,
                       RC_UNROLL(rc_bitblt_wm_nand_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_nor_bin, 1)
void
rc_bitblt_wm_nor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_NOR,
                       RC_UNROLL(rc_bitblt_wm_nor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_xnor_bin, 1)
void
rc_bitblt_wm_xnor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_XNOR,
                       RC_UNROLL(rc_bitblt_wm_xnor_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_andn_bin, 1)
void
rc_bitblt_wm_andn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_ANDN,
                       RC_UNROLL(rc_bitblt_wm_andn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_orn_bin, 1)
void
rc_bitblt_wm_orn_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_ORN,
                       RC_UNROLL(rc_bitblt_wm_orn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_nandn_bin, 1)
void
rc_bitblt_wm_nandn_bin(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_NANDN,
                       RC_UNROLL(rc_bitblt_wm_nandn_bin));
}
#endif


#if RC_IMPL(rc_bitblt_wm_norn_bin, 1)
void
rc_bitblt_wm_norn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height, int offset)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       offset, RC_BITBLT_ROP_NORN,
                       RC_UNROLL(rc_bitblt_wm_norn_bin));
}
#endif
