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
 *  @file   rapp_filter.c
 *  @brief  RAPP fixed filters.
 */

#include "rappcompute.h"    /* RAPP Compute API */
#include "rapp_api.h"       /* API symbol macro */
#include "rapp_util.h"      /* Validation       */
#include "rapp_error.h"     /* Error codes      */
#include "rapp_error_int.h" /* Error handling   */
#include "rapp_filter.h"    /* Fixed-filter API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

RAPP_API(int, rapp_filter_diff_1x2_horz_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -RC_ALIGNMENT, rc_align(width)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                height,
                                src, src_dim, width + RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_diff_1x2_horz_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_diff_1x2_horz_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -RC_ALIGNMENT, rc_align(width)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                height,
                                src, src_dim, width + RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_diff_1x2_horz_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_diff_2x1_vert_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim, rc_align(width)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform convolution */
    rc_filter_diff_2x1_vert_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_diff_2x1_vert_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim, rc_align(width)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width, height,
                                src, src_dim, width, height);
    }

    /* Perform convolution */
    rc_filter_diff_2x1_vert_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_diff_2x2_magn_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(width)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                height,
                                src, src_dim, width + RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_diff_2x2_magn_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_sobel_3x3_horz_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_sobel_3x3_horz_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_sobel_3x3_horz_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_sobel_3x3_horz_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_sobel_3x3_vert_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_sobel_3x3_vert_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_sobel_3x3_vert_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_sobel_3x3_vert_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_sobel_3x3_magn_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_sobel_3x3_magn_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_gauss_3x3_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_gauss_3x3_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_laplace_3x3_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_laplace_3x3_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_laplace_3x3_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_laplace_3x3_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_highpass_3x3_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_highpass_3x3_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}

RAPP_API(int, rapp_filter_highpass_3x3_abs_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(dst, dst_dim, src, src_dim, height,
                                       0, rc_align(width),
                                       -src_dim - RC_ALIGNMENT,
                                       rc_align(src_dim + width + 1)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_U8(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_U8(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        return rapp_error_u8_u8(dst, dst_dim, width,                  height,
                                src, src_dim, width + 2*RC_ALIGNMENT, height);
    }

    /* Perform convolution */
    rc_filter_highpass_3x3_abs_u8(dst, dst_dim, src, src_dim, width, height);

    return RAPP_OK;
}
