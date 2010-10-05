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
 *  @file   rc_bitblt_vm.c
 *  @brief  Bitblit operations on misaligned vectors.
 */

#include "rc_impl_cfg.h"   /* Implementation cfg            */
#include "rc_vector.h"     /* Vector operations             */
#include "rc_util.h"       /* RC_DIV_CEIL()                 */
#include "rc_bitblt_rop.h" /* Bitblit vector raster ops     */
#include "rc_bitblt_vm.h"  /* Bitblit misaligned vector API */

/*
 * -------------------------------------------------------------
 *  Template macros
 * -------------------------------------------------------------
 */

/**
 *  The misaligned bitblt template.
 */
#define RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim,           \
                           width, height, rop, unroll)           \
do {                                                             \
    int tot = RC_DIV_CEIL(width, 8*RC_VEC_SIZE);                 \
    int len = tot / (unroll);       /* Fully unrolled    */      \
    int rem = tot % (unroll);       /* Remaining vectors */      \
    int y;                                                       \
                                                                 \
    RC_VEC_DECLARE();                                            \
    for (y = 0; y < (height); y++) {                             \
        const uint8_t *ptr;                                      \
        rc_vec_t       v1, v2, v3;                               \
        int            i0 = y*(src_dim);                         \
        int            i  = 0;                                   \
        int            j  = y*(dst_dim);                         \
        int            x;                                        \
                                                                 \
        /* Set up the misaligned vector loads */                 \
        RC_VEC_LDINIT(v1, v2, v3, ptr, &(src)[i0]);              \
                                                                 \
        /* Blit all unrolled vectors */                          \
        for (x = 0; x < len; x++) {                              \
            RC_BITBLT_ITER(dst, ptr, i, j, v1, v2, v3, rop);     \
            if ((unroll) >= 2) {                                 \
                RC_BITBLT_ITER(dst, ptr, i, j, v1, v2, v3, rop); \
            }                                                    \
            if ((unroll) == 4) {                                 \
                RC_BITBLT_ITER(dst, ptr, i, j, v1, v2, v3, rop); \
                RC_BITBLT_ITER(dst, ptr, i, j, v1, v2, v3, rop); \
            }                                                    \
        }                                                        \
                                                                 \
        /* Blit all remaining vectors */                         \
        for (x = 0; x < rem; x++) {                              \
            RC_BITBLT_ITER(dst, ptr, i, j, v1, v2, v3, rop);     \
        }                                                        \
    }                                                            \
    RC_VEC_CLEANUP();                                            \
} while (0)

/**
 *  A misaligned bitblt iteration.
 */
#define RC_BITBLT_ITER(dst, src, i, j, v1, v2, v3, rop) \
do {                                                    \
    rc_vec_t sv, dv;                                    \
    RC_VEC_LOADU(sv, v1, v2, v3, &(src)[i]);            \
    RC_VEC_LOAD(dv, &(dst)[j]);                         \
    rop(dv, dv, sv);                                    \
    RC_VEC_STORE(&(dst)[j], dv);                        \
    (i) += RC_VEC_SIZE;                                 \
    (j) += RC_VEC_SIZE;                                 \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

#if RC_IMPL(rc_bitblt_vm_copy_bin, 1)
void
rc_bitblt_vm_copy_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_COPY,
                       RC_UNROLL(rc_bitblt_vm_copy_bin));
}
#endif


#ifdef RC_BITBLT_ROP_NOT
#if RC_IMPL(rc_bitblt_vm_not_bin, 1)
void
rc_bitblt_vm_not_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NOT,
                       RC_UNROLL(rc_bitblt_vm_not_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_AND
#if RC_IMPL(rc_bitblt_vm_and_bin, 1)
void
rc_bitblt_vm_and_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_AND,
                       RC_UNROLL(rc_bitblt_vm_and_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_OR
#if RC_IMPL(rc_bitblt_vm_or_bin, 1)
void
rc_bitblt_vm_or_bin(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_OR,
                       RC_UNROLL(rc_bitblt_vm_or_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_XOR
#if RC_IMPL(rc_bitblt_vm_xor_bin, 1)
void
rc_bitblt_vm_xor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_XOR,
                       RC_UNROLL(rc_bitblt_vm_xor_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_NAND
#if RC_IMPL(rc_bitblt_vm_nand_bin, 1)
void
rc_bitblt_vm_nand_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NAND,
                       RC_UNROLL(rc_bitblt_vm_nand_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_NOR
#if RC_IMPL(rc_bitblt_vm_nor_bin, 1)
void
rc_bitblt_vm_nor_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NOR,
                       RC_UNROLL(rc_bitblt_vm_nor_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_XNOR
#if RC_IMPL(rc_bitblt_vm_xnor_bin, 1)
void
rc_bitblt_vm_xnor_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_XNOR,
                       RC_UNROLL(rc_bitblt_vm_xnor_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_ANDN
#if RC_IMPL(rc_bitblt_vm_andn_bin, 1)
void
rc_bitblt_vm_andn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_ANDN,
                       RC_UNROLL(rc_bitblt_vm_andn_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_ORN
#if RC_IMPL(rc_bitblt_vm_orn_bin, 1)
void
rc_bitblt_vm_orn_bin(uint8_t *restrict dst, int dst_dim,
                     const uint8_t *restrict src, int src_dim,
                     int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_ORN,
                       RC_UNROLL(rc_bitblt_vm_orn_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_NANDN
#if RC_IMPL(rc_bitblt_vm_nandn_bin, 1)
void
rc_bitblt_vm_nandn_bin(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NANDN,
                       RC_UNROLL(rc_bitblt_vm_nandn_bin));
}
#endif
#endif


#ifdef RC_BITBLT_ROP_NORN
#if RC_IMPL(rc_bitblt_vm_norn_bin, 1)
void
rc_bitblt_vm_norn_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_BITBLT_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                       RC_BITBLT_ROP_NORN,
                       RC_UNROLL(rc_bitblt_vm_norn_bin));
}
#endif
#endif
