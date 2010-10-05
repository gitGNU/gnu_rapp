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
 *  @file   rapp_ref_bitblt.h
 *  @brief  RAPP bitblt operations, reference implementation.
 */

#ifndef RAPP_REF_BITBLT_H
#define RAPP_REF_BITBLT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_bitblt_copy_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height);
void
rapp_ref_bitblt_not_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_and_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_or_bin(uint8_t *dst, int dst_dim, int dst_off,
                       const uint8_t *src, int src_dim, int src_off,
                       int width, int height);
void
rapp_ref_bitblt_xor_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_nand_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height);
void
rapp_ref_bitblt_nor_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_xnor_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_andn_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height);
void
rapp_ref_bitblt_orn_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height);
void
rapp_ref_bitblt_nandn_bin(uint8_t *dst, int dst_dim, int dst_off,
                          const uint8_t *src, int src_dim, int src_off,
                          int width, int height);
void
rapp_ref_bitblt_norn_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_REF_BITBLT_H */
