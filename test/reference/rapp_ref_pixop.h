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
 *  @file   rapp_ref_pixop.h
 *  @brief  RAPP pixelwise operations, reference implementation.
 */

#ifndef RAPP_REF_PIXOP_H
#define RAPP_REF_PIXOP_H

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
rapp_ref_pixop_set_u8(uint8_t *buf, int dim,
                      int width, int height, unsigned value);

void
rapp_ref_pixop_not_u8(uint8_t *buf, int dim, int width, int height);

void
rapp_ref_pixop_flip_u8(uint8_t *buf, int dim, int width, int height);

void
rapp_ref_pixop_lut_u8(uint8_t *buf, int dim,
                      int width, int height, const uint8_t *lut);

void
rapp_ref_pixop_abs_u8(uint8_t *buf, int dim, int width, int height);

void
rapp_ref_pixop_addc_u8(uint8_t *buf, int dim,
                       int width, int height, int value);

void
rapp_ref_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                        int height, unsigned value, unsigned alpha8);

void
rapp_ref_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                         int height, unsigned value, unsigned alpha8);

void
rapp_ref_pixop_copy_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height);

void
rapp_ref_pixop_add_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height);

void
rapp_ref_pixop_avg_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height);

void
rapp_ref_pixop_sub_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height);

void
rapp_ref_pixop_subh_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height);

void
rapp_ref_pixop_suba_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height);

void
rapp_ref_pixop_lerp_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, unsigned alpha8);

void
rapp_ref_pixop_lerpn_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, unsigned alpha8);

void
rapp_ref_pixop_lerpi_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, unsigned alpha8);

void
rapp_ref_pixop_norm_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_REF_PIXOP_H */
