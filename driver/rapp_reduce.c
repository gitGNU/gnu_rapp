/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_reduce.c
 *  @brief  RAPP 8-bit 2x reduction.
 */


#include "rappcompute.h"    /* RAPP Compute API    */
#include "rapp_api.h"       /* API symbol macro    */
#include "rapp_util.h"      /* Validation          */
#include "rapp_error.h"     /* Error codes         */
#include "rapp_error_int.h" /* Error handling      */
#include "rapp_reduce.h"    /* 8-bit reduction API */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  8-bit 1x2 (horizontal) reduction.
 */
RAPP_API(int, rapp_reduce_1x2_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, src, src_dim, height,
                                     rc_align(width/2), rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width/2, height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width,   height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width/2, height,
                                src, src_dim, width,   height);
    }

    /* Perform operation */
    rc_reduce_1x2_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  8-bit 2x1 (vertical) reduction.
 */
RAPP_API(int, rapp_reduce_2x1_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, 0, src, src_dim, height,
                                     dst_dim*(height/2 - 1) + rc_align(width),
                                     rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height/2) ||
        !RAPP_VALIDATE_U8(src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height/2,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_reduce_2x1_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

/**
 *  8-bit 2x2 reduction.
 */
RAPP_API(int, rapp_reduce_2x2_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, 0, src, src_dim, height,
                                     (dst_dim*(height/2 - 1) +
                                      rc_align(width/2)),
                                     rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width/2, height/2) ||
        !RAPP_VALIDATE_U8(src, src_dim, width,   height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width/2, height/2,
                                src, src_dim, width,   height);
    }

    /* Perform operation */
    rc_reduce_2x2_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}
