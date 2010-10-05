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
 *  @file   rapp_pixop.c
 *  @brief  RAPP pixelwise operations.
 */

#include <stdlib.h>         /* abs()            */
#include "rappcompute.h"    /* RAPP Compute API */
#include "rapp_api.h"       /* API symbol macro */
#include "rapp_util.h"      /* Validation       */
#include "rapp_error.h"     /* Error codes      */
#include "rapp_error_int.h" /* Error handling   */
#include "rapp_pixop.h"     /* Pixop API        */


/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Set all pixels to a constant value.
 */
RAPP_API(int, rapp_pixop_set_u8,
         (uint8_t *buf, int dim, int width, int height, unsigned value))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (value > 0xff) {
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    rc_pixop_set_u8(buf, dim, width, height, value);

    return RAPP_OK;
}


/**
 *  Negate all pixels.
 */
RAPP_API(int, rapp_pixop_not_u8,
         (uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    rc_pixop_not_u8(buf, dim, width, height);

    return RAPP_OK;
}


/**
 *  Flip the sign bit.
 */
RAPP_API(int, rapp_pixop_flip_u8,
         (uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    rc_pixop_flip_u8(buf, dim, width, height);

    return RAPP_OK;
}


/**
 *  Lookup-table transformation.
 */
RAPP_API(int, rapp_pixop_lut_u8,
         (uint8_t *restrict buf, int dim,
          int width, int height, const uint8_t *restrict lut))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (!lut) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    rc_pixop_lut_u8(buf, dim, width, height, lut);

    return RAPP_OK;
}


/**
 *  Absolute value.
 */
RAPP_API(int, rapp_pixop_abs_u8,
         (uint8_t *buf, int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }

    /* Perform operation */
    rc_pixop_abs_u8(buf, dim, width, height);

    return RAPP_OK;
}


/**
 *  Add signed constant.
 */
RAPP_API(int, rapp_pixop_addc_u8,
         (uint8_t *buf, int dim, int width, int height, int value))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (abs(value) > 0xff) {
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    rc_pixop_addc_u8(buf, dim, width, height, value);

    return RAPP_OK;
}


/**
 *  Linear interpolation with a constant.
 */
RAPP_API(int, rapp_pixop_lerpc_u8,
         (uint8_t *buf, int dim, int width,
          int height, unsigned value, unsigned alpha8))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (value > 0xff || alpha8 > 0x100) {
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    switch (alpha8) {
        case 0:
            /* alpha=0.0 - no operation */
            break;

        case 0x100:
            /* alpha=1.0 - set to constant */
            rc_pixop_set_u8(buf, dim, width, height, value);
            break;

        default:
            /* Compute the linear interpolation */
            rc_pixop_lerpc_u8(buf, dim, width, height, value, alpha8);
    }

    return RAPP_OK;
}


/**
 *  Linear interpolation with a constant, rounded towards the constant value.
 */
RAPP_API(int, rapp_pixop_lerpnc_u8,
         (uint8_t *buf, int dim, int width,
          int height, unsigned value, unsigned alpha8))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_U8(buf, dim, width, height)) {
        return rapp_error_u8(buf, dim, width, height);
    }
    if (value > 0xff || alpha8 > 0x100) {
        return RAPP_ERR_PARM_RANGE;
    }

    switch (alpha8) {
        case 0:
            /* alpha=0.0 - no operation */
            break;

        case 0x100:
            /* alpha=1.0 - set to constant */
            rc_pixop_set_u8(buf, dim, width, height, value);
            break;

        default:
            /* Compute the linear interpolation */
            rc_pixop_lerpnc_u8(buf, dim, width, height, value, alpha8);
    }

    return RAPP_OK;
}


/*
 * -------------------------------------------------------------
 *  Double-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Copy all pixels.
 */
RAPP_API(int, rapp_pixop_copy_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_bitblt_va_copy_bin(dst, dst_dim, src, src_dim, 8*width, height);

    return RAPP_OK;
}


/**
 *  Saturated addition.
 */
RAPP_API(int, rapp_pixop_add_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_add_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Average value.
 */
RAPP_API(int, rapp_pixop_avg_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_avg_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Saturated subtraction.
 */
RAPP_API(int, rapp_pixop_sub_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_sub_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Halved subtraction.
 */
RAPP_API(int, rapp_pixop_subh_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_subh_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Absolute-value subtraction.
 */
RAPP_API(int, rapp_pixop_suba_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_suba_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}


/**
 *  Linear interpolation.
 */
RAPP_API(int, rapp_pixop_lerp_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, unsigned alpha8))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }
    if (alpha8 > 0x100) {
        /* Return the error code */
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    switch (alpha8) {
        case 0:
            /* alpha=0.0 - no operation */
            break;

        case 0x100:
            /* alpha=1.0 - copy src */
            rc_bitblt_va_copy_bin(dst, dst_dim, src, src_dim, 8*width, height);
            break;

        default:
            /* Compute the linear interpolation */
            rc_pixop_lerp_u8(dst, dst_dim, src, src_dim,
                             width, height, alpha8);
    }

    return RAPP_OK;
}


/**
 *  Linear interpolation with non-zero update, i.e. rounded towards src.
 */
RAPP_API(int, rapp_pixop_lerpn_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, unsigned alpha8))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }
    if (alpha8 > 0x100) {
        /* Return the error code */
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    switch (alpha8) {
        case 0:
            /* alpha=0.0 - no operation */
            break;

        case 0x100:
            /* alpha=1.0 - copy src */
            rc_bitblt_va_copy_bin(dst, dst_dim, src, src_dim, 8*width, height);
            break;

        default:
            /* Compute the linear interpolation */
            rc_pixop_lerpn_u8(dst, dst_dim, src, src_dim,
                              width, height, alpha8);
    }

    return RAPP_OK;
}


/**
 *  Linear interpolation with inverted second operand.
 */
RAPP_API(int, rapp_pixop_lerpi_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, unsigned alpha8))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }
    if (alpha8 > 0x100) {
        /* Return the error code */
        return RAPP_ERR_PARM_RANGE;
    }

    /* Perform operation */
    switch (alpha8) {
        case 0:
            /* alpha=0.0 - no operation */
            break;

        case 0x100:
            /* alpha=1.0 - invert */
            rc_bitblt_va_not_bin(dst, dst_dim, src, src_dim, 8*width, height);
            break;

        default:
            /* Compute the linear interpolation with inverted src */
            rc_pixop_lerpi_u8(dst, dst_dim, src, src_dim,
                              width, height, alpha8);
    }

    return RAPP_OK;
}


/**
 *  L1 norm.
 */
RAPP_API(int, rapp_pixop_norm_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(dst, dst_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform operation */
    rc_pixop_norm_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}
