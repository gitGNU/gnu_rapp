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
 *  @file   rapp_integral.c
 *  @brief  RAPP integral image.
 */

#include "rappcompute.h"    /* RAPP Compute API    */
#include "rapp_util.h"      /* Validation          */
#include "rapp_error.h"     /* Error codes         */
#include "rapp_error_int.h" /* Error handling      */
#include "rapp_api.h"
#include "rapp_integral.h"


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Integral image: binary source to 8-bit integral.
 */
RAPP_API(int, rapp_integral_sum_bin_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -RC_ALIGNMENT - src_dim,
                                       rc_align((width + 7) / 8),
                                       0, rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(src, src_dim, width, height) ||
        !RAPP_VALIDATE_U8 (dst, dst_dim, width, height))
    {
        return rapp_error_bin_u8(src, src_dim,
                                 dst, dst_dim, width, height);
    }

    /* Perform operation */
    rc_integral_sum_bin_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  Integral image: binary source to 16-bit integral.
 */
RAPP_API(int, rapp_integral_sum_bin_u16,
         (uint16_t *restrict dst, int dst_dim,
         const uint8_t *restrict src, int src_dim,
         int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -RC_ALIGNMENT - src_dim,
                                       rc_align((width + 7) / 8),
                                       0, rc_align(2*width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(src, src_dim, width, height) ||
        !RAPP_VALIDATE_U16(dst, dst_dim, width, height))
    {
        return rapp_error_bin_u16(src, src_dim,
                                  dst, dst_dim, width, height);
    }

    /* Perform operation */
    rc_integral_sum_bin_u16(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  Integral image: binary source to 32-bit integral.
 */
RAPP_API(int, rapp_integral_sum_bin_u32,
         (uint32_t *restrict dst, int dst_dim,
         const uint8_t *restrict src, int src_dim,
         int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -RC_ALIGNMENT - src_dim,
                                       rc_align((width + 7) / 8),
                                       0, rc_align(4*width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(src, src_dim, width, height) ||
        !RAPP_VALIDATE_U32(dst, dst_dim, width, height))
    {
        return rapp_error_bin_u32(src, src_dim,
                                  dst, dst_dim, width, height);
    }

    /* Perform operation */
    rc_integral_sum_bin_u32(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  Integral image: 8-bit source to 16-bit integral.
 */
RAPP_API(int, rapp_integral_sum_u8_u16,
         (uint16_t *restrict dst, int dst_dim,
         const uint8_t *restrict src, int src_dim,
         int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -RC_ALIGNMENT - src_dim,
                                       rc_align(width),
                                       0, rc_align(2*width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(src, src_dim, width, height) ||
        !RAPP_VALIDATE_U16(dst, dst_dim, width, height))
    {
        return rapp_error_u8_u16(src, src_dim, width, height,
                                 dst, dst_dim, width, height);
    }

    /* Perform operation */
    rc_integral_sum_u8_u16(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  Integral image: 8-bit source to 32-bit integral.
 */
RAPP_API(int, rapp_integral_sum_u8_u32,
         (uint32_t *restrict dst, int dst_dim,
         const uint8_t *restrict src, int src_dim,
         int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -RC_ALIGNMENT - src_dim,
                                       rc_align(width),
                                       0, rc_align(4*width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(src, src_dim, width, height) ||
        !RAPP_VALIDATE_U32(dst, dst_dim, width, height))
    {
        return rapp_error_u8_u32(src, src_dim, width, height,
                                 dst, dst_dim, width, height);
    }

    /* Perform operation */
    rc_integral_sum_u8_u32(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}
