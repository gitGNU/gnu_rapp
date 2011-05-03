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
 *  @file   rapp_reduce_bin.c
 *  @brief  RAPP binary 2x expansion.
 */

#include "rappcompute.h"     /* RAPP Compute API        */
#include "rapp_api.h"        /* API symbol macro        */
#include "rapp_util.h"       /* Validation              */
#include "rapp_error.h"      /* Error codes             */
#include "rapp_error_int.h"  /* Error handling          */
#include "rapp_expand_bin.h" /* Binary 2x expansion API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary 1x2 expansion.
 */
RAPP_API(int, rapp_expand_1x2_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, src, src_dim, height,
                                     rc_align((2*width + 7) / 8),
                                     rc_align((width + 7) / 8)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, 2*width, height) ||
        !RAPP_VALIDATE_BIN(src, src_dim,   width, height))
    {
        return rapp_error_bin_bin(dst, dst_dim, 2*width, height,
                                  src, src_dim,   width, height);
    }

    /* Perform operation */
    rc_expand_1x2_bin(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Binary 2x1 expansion.
 */
RAPP_API(int, rapp_expand_2x1_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, src, src_dim, height,
                                     height*dst_dim +
                                     rc_align((width + 7) / 8),
                                     rc_align((width + 7) / 8)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, 2*height) ||
        !RAPP_VALIDATE_BIN(src, src_dim, width,   height))
    {
        return rapp_error_bin_bin(dst, dst_dim, width, 2*height,
                                  src, src_dim, width,   height);
    }

    /* Perform operation */
    rc_expand_2x1_bin(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Binary 2x2 expansion.
 */
RAPP_API(int, rapp_expand_2x2_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, src, src_dim, height,
                                     height*dst_dim +
                                     rc_align((2*width + 7) / 8),
                                     rc_align((width + 7) / 8)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, 2*width, 2*height) ||
        !RAPP_VALIDATE_BIN(src, src_dim,   width,   height))
    {
        return rapp_error_bin_bin(dst, dst_dim, 2*width, 2*height,
                                  src, src_dim,   width,   height);
    }

    /* Perform operation */
    rc_expand_2x2_bin(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}
