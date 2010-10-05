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
 *  @file   rapp_rotate.c
 *  @brief  RAPP 8-bit image rotation.
 */

#include "rappcompute.h"    /* RAPP Compute API    */
#include "rapp_api.h"       /* API symbol macro    */
#include "rapp_util.h"      /* Validation          */
#include "rapp_error.h"     /* Error codes         */
#include "rapp_error_int.h" /* Error handling      */
#include "rapp_rotate.h"    /* 8-bit rotation API  */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The block side length in pixels of the block subdivision.
 */
#define RAPP_ROTATE_BLOCK 64


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Rotate an 8-bit image 90 degrees in clockwise direction.
 */
RAPP_API(int, rapp_rotate_cw_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    int xblock = width  / RAPP_ROTATE_BLOCK; /* Num blocks in x dir   */
    int yblock = height / RAPP_ROTATE_BLOCK; /* Num blocks in y dir   */
    int xrem   = width  % RAPP_ROTATE_BLOCK; /* Partial block, x dir  */
    int yrem   = height % RAPP_ROTATE_BLOCK; /* Partial block, y dir  */
    int step   = RAPP_ROTATE_BLOCK*dst_dim;  /* Dst buffer block step */
    int yb, y;

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, 0, src, src_dim, height,
                                     dst_dim*(width - 1) + rc_align(height),
                                     rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, height, width) ||
        !RAPP_VALIDATE_U8(src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, height, width,
                                src, src_dim, width, height);
    }

    /* Rotate the first row of partial blocks */
    if (yrem) {
        int i = 0;
        int j = yblock*RAPP_ROTATE_BLOCK;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RAPP_ROTATE_BLOCK, j += step) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_cw_u8(&dst[j], dst_dim,
                            &src[i], src_dim,
                            RAPP_ROTATE_BLOCK, yrem);
        }
        if (xrem) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_cw_u8(&dst[j], dst_dim,
                            &src[i], src_dim,
                            xrem, yrem);
        }
    }

    /* Rotate all full blocks */
    for (yb = 0, y = 0; yb < yblock; yb++, y += RAPP_ROTATE_BLOCK) {
        int i = (yrem + y)*src_dim;
        int j = (yblock - 1)*RAPP_ROTATE_BLOCK - y;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RAPP_ROTATE_BLOCK, j += step) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_cw_u8(&dst[j], dst_dim,
                            &src[i], src_dim,
                            RAPP_ROTATE_BLOCK,
                            RAPP_ROTATE_BLOCK);
        }
        if (xrem) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_cw_u8(&dst[j], dst_dim,
                            &src[i], src_dim,
                            xrem, RAPP_ROTATE_BLOCK);
        }
    }

    return RAPP_OK;
}


/**
 *  Rotate an 8-bit image 90 degrees in counter-clockwise direction.
 */
RAPP_API(int, rapp_rotate_ccw_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    int xblock = width  / RAPP_ROTATE_BLOCK;         /* Num blocks in x dir   */
    int yblock = height / RAPP_ROTATE_BLOCK;         /* Num blocks in y dir   */
    int xrem   = width  % RAPP_ROTATE_BLOCK;         /* Partial block, x dir  */
    int yrem   = height % RAPP_ROTATE_BLOCK;         /* Partial block, y dir  */
    int step   = RAPP_ROTATE_BLOCK*dst_dim;          /* Dst buffer block step */
    int end    = width*dst_dim - step;               /* Dst buf block offset  */
    int adj    = (RAPP_ROTATE_BLOCK - xrem)*dst_dim; /* Dst offset adjustment */
    int yb, y;

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, 0, src, src_dim, height,
                                     dst_dim*(width - 1) + rc_align(height),
                                     rc_align(width)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, height, width) ||
        !RAPP_VALIDATE_U8(src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, height, width,
                                src, src_dim, width, height);
    }

    /* Rotate all full blocks */
    for (yb = 0, y = 0; yb < yblock; yb++, y += RAPP_ROTATE_BLOCK) {
        int i = y*src_dim;
        int j = y + end;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RAPP_ROTATE_BLOCK, j -= step) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_ccw_u8(&dst[j], dst_dim,
                             &src[i], src_dim,
                             RAPP_ROTATE_BLOCK,
                             RAPP_ROTATE_BLOCK);
        }
        if (xrem) {
            j += adj;
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_ccw_u8(&dst[j], dst_dim,
                             &src[i], src_dim,
                             xrem, RAPP_ROTATE_BLOCK);
        }
    }

    /* Rotate the last row of partial blocks */
    if (yrem) {
        int i = RAPP_ROTATE_BLOCK*yblock*src_dim;
        int j = RAPP_ROTATE_BLOCK*yblock + end;
        int xb;

        for (xb = 0; xb < xblock; xb++, i += RAPP_ROTATE_BLOCK, j -= step) {
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_ccw_u8(&dst[j], dst_dim,
                             &src[i], src_dim,
                             RAPP_ROTATE_BLOCK, yrem);
        }
        if (xrem) {
            j += adj;
            assert(j >= 0 && j < width*dst_dim);
            rc_rotate_ccw_u8(&dst[j], dst_dim,
                             &src[i], src_dim,
                             xrem, yrem);
        }
    }

    return RAPP_OK;
}
