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
 *  @file   rc_type.c
 *  @brief  RAPP Compute layer type conversions, vector implementation.
 */

#include "rc_impl_cfg.h"   /* Implementation config */
#include "rc_vector.h"     /* Vector operations     */
#include "rc_thresh_tpl.h" /* Thresholding template */
#include "rc_type.h"       /* Type conversion API   */


/*
 * -------------------------------------------------------------
 *  Comparison macros
 * -------------------------------------------------------------
 */

/**
 *  Conversion u8 to binary macro for thresholding template.
 */
#define RC_TYPE_U8_TO_BIN(dstv, srcv, arg1, arg2) \
    ((dstv) = (srcv))


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Conversion u8 to binary.
 */
#if    RC_IMPL(rc_type_u8_to_bin, 1)
#ifdef RC_THRESH_TEMPLATE
void
rc_type_u8_to_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, 0, 0, RC_TYPE_U8_TO_BIN,
                       RC_UNROLL(rc_type_u8_to_bin));
}
#endif
#endif
