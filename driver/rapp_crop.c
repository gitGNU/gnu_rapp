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
 *  @file   rapp_crop.c
 *  @brief  RAPP binary image cropping.
 */

#include "rappcompute.h"    /* RAPP Compute API    */
#include "rapp_api.h"       /* API symbol macro    */
#include "rapp_util.h"      /* Validation          */
#include "rapp_error.h"     /* Error codes         */
#include "rapp_error_int.h" /* Error handling      */
#include "rapp_crop.h"      /* Binary cropping API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Find the first non-zero pixel in traversal order.
 */
RAPP_API(int, rapp_crop_seek_bin,
         (const uint8_t *buf, int dim,
          int width, int height, unsigned pos[2]))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }
    if (!pos) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    return rc_crop_seek_bin(buf, dim, width, height, pos);
}


/**
 *  Compute the bounding box.
 */
RAPP_API(int, rapp_crop_box_bin,
         (const uint8_t *buf, int dim,
          int width, int height, unsigned box[4]))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }
    if (!box) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    return rc_crop_box_bin(buf, dim, width, height, box);
}
