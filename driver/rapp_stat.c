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
 *  @file   rapp_stat.c
 *  @brief  RAPP image statistics.
 */

#include "rappcompute.h"    /* RAPP Compute API  */
#include "rapp_api.h"       /* API symbol macro  */
#include "rapp_util.h"      /* Validation        */
#include "rapp_error.h"     /* Error codes       */
#include "rapp_error_int.h" /* Error handling    */
#include "rapp_stat.h"      /* Statistics API    */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary pixel sum.
 */
RAPP_API(int32_t, rapp_stat_sum_bin,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }

    /* Perform operation */
    return (int32_t)rc_stat_sum_bin(buf, dim, width, height);
}

/**
 *  8-bit pixel sum.
 */
RAPP_API(int32_t, rapp_stat_sum_u8,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    return (int32_t)rc_stat_sum_u8(buf, dim, width, height);
}

/**
 *  8-bit pixel sum and squared sum.
 */
RAPP_API(int, rapp_stat_sum2_u8,
         (const uint8_t *buf, int dim,
          int width, int height, uintmax_t sum[2]))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (!sum) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    rc_stat_sum2_u8(buf, dim, width, height, sum);

    return RAPP_OK;
}

/**
 *  8-bit pixel cross sums.
 */
RAPP_API(int, rapp_stat_xsum_u8,
         (const uint8_t *restrict src1, int src1_dim,
          const uint8_t *restrict src2, int src2_dim,
          int width, int height, uintmax_t sum[5]))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(src1, src1_dim, width, height) ||
        !RAPP_VALIDATE_U8(src2, src2_dim, width, height))
    {
        return rapp_error_u8_u8(src1, src1_dim, width, height,
                                src2, src2_dim, width, height);
    }
    if (!sum) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    rc_stat_xsum_u8(src1, src1_dim, src2, src2_dim, width, height, sum);

    return RAPP_OK;
}

RAPP_API(int, rapp_stat_min_bin,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_stat_min_bin(buf, dim, width, height);
}

RAPP_API(int, rapp_stat_max_bin,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_stat_max_bin(buf, dim, width, height);
}

RAPP_API(int, rapp_stat_min_u8,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_stat_min_u8(buf, dim, width, height);
}

RAPP_API(int, rapp_stat_max_u8,
         (const uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_stat_max_u8(buf, dim, width, height);
}
