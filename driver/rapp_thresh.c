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
 *  @file   rapp_thresh.c
 *  @brief  RAPP thresholding to binary.
 */

#include "rappcompute.h"    /* RAPP Compute API    */
#include "rapp_api.h"       /* API symbol macro    */
#include "rapp_util.h"      /* Validation, CLAMP() */
#include "rapp_error.h"     /* Error codes         */
#include "rapp_error_int.h" /* Error handling      */
#include "rapp_thresh.h"    /* Thresholding API    */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Single thresholding greater-than.
 */
RAPP_API(int, rapp_thresh_gt_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int thresh))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8 (src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_u8(dst, dst_dim, src, src_dim, width, height);
    }

    /* Reduce threshold range */
    thresh = CLAMP(thresh, -1, 0xff);

    /* Perform operation */
    switch (thresh) {
        case -1:
            /* Handle the always-true case */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0xff);
            break;

        case 0x7f:
            /* Use the faster type conversion */
            rc_type_u8_to_bin(dst, dst_dim, src, src_dim, width, height);
            break;

        case 0xff:
            /* Handle the always-false case */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0);
            break;

        default:
            /* Perform thresholding */
            rc_thresh_gt_u8(dst, dst_dim, src, src_dim,
                            width, height, thresh);
    }

    return RAPP_OK;
}


/**
 *  Single thresholding less-than.
 */
RAPP_API(int, rapp_thresh_lt_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int thresh))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8 (src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_u8(dst, dst_dim, src, src_dim, width, height);
    }

    /* Reduce threshold range */
    thresh = CLAMP(thresh, 0, 0x100);

    /* Perform operation */
    switch (thresh) {
        case 0:
            /* Handle the always-false case */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0);
            break;

        case 0x80:
            /* Use the faster type conversion */
            rc_type_u8_to_bin(dst, dst_dim, src, src_dim, width, height);
            rc_pixop_not_u8(dst, dst_dim, (width + 7) / 8, height);
            break;

        case 0x100:
            /* Handle the always-true case */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0xff);
            break;

        default:
            /* Perform thresholding */
            rc_thresh_lt_u8(dst, dst_dim, src, src_dim,
                            width, height, thresh);
    }

    return RAPP_OK;
}


/**
 *  Double thresholding greater-than AND less-than.
 */
RAPP_API(int, rapp_thresh_gtlt_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int low, int high))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8 (src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_u8(dst, dst_dim, src, src_dim, width, height);
    }

    /* Reduce threshold range */
    low  = CLAMP(low, -1, 0xff);
    high = CLAMP(high, 0, 0x100);

    /* Perform operation */
    switch (low) {
        case -1:
            /* Low threshold always true - threshold high */
            rapp_thresh_lt_u8(dst, dst_dim, src, src_dim,
                              width, height, high);
            break;

        case 0xff:
            /* Low threshold always false - clear */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0);
            break;

        default:
            switch (high) {
                case 0:
                    /* High threshold always false - clear */
                    rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0);
                    break;

                case 0x100:
                    /* High threshold always true - threshold low */
                    rapp_thresh_gt_u8(dst, dst_dim, src, src_dim,
                                      width, height, low);
                    break;

                default:
                    if (low >= high - 1) {
                        /* Handle the always-false case - clear */
                        rc_pixop_set_u8(dst, dst_dim,
                                        (width + 7)/8, height, 0);
                    }
                    else {
                        /* Perform double thresholding */
                        rc_thresh_gtlt_u8(dst, dst_dim, src, src_dim,
                                          width, height, low, high);
                    }
            }
    }

    return RAPP_OK;
}


/**
 *  Double thresholding less-than OR greater-than.
 */
RAPP_API(int, rapp_thresh_ltgt_u8,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int low, int high))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, src, src_dim, height, width)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_U8 (src, src_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_u8(dst, dst_dim, src, src_dim, width, height);
    }

    /* Reduce threshold range */
    low  = CLAMP(low,   0, 0x100);
    high = CLAMP(high, -1, 0xff);

    /* Perform operation */
    switch (low) {
        case 0:
            /* Low threshold always false - threshold high */
            rapp_thresh_gt_u8(dst, dst_dim, src, src_dim,
                              width, height, high);
            break;

        case 0xff:
            /* Low threshold always true - set */
            rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0xff);
            break;

        default:
            switch (high) {
                case -1:
                    /* High threshold always true - set */
                    rc_pixop_set_u8(dst, dst_dim, (width + 7)/8, height, 0xff);
                    break;

                case 0xff:
                    /* High threshold always false - threshold low */
                    rapp_thresh_lt_u8(dst, dst_dim, src, src_dim,
                                      width, height, low);
                    break;

                default:
                    if (low > high) {
                        /* Handle the always-true case - set */
                        rc_pixop_set_u8(dst, dst_dim,
                                        (width + 7)/8, height, 0xff);
                    }
                    else {
                        /* Perform double thresholding */
                        rc_thresh_ltgt_u8(dst, dst_dim, src, src_dim,
                                          width, height, low, high);
                    }
            }
    }

    return RAPP_OK;
}
