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
 *  @file   rapp_pixel.c
 *  @brief  RAPP pixel access functions.
 */

#include "rappcompute.h"  /* Pixel access macros */
#include "rapp_api.h"     /* API symbol macro    */
#include "rapp_util.h"    /* RAPP_OK             */
#include "rapp_error.h"   /* Error codes         */
#include "rapp_error_int.h"
#include "rapp_pixel.h"   /* Pixel access API    */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

RAPP_API(int, rapp_pixel_get_bin,
         (const uint8_t *buf, int dim, int off, int x, int y))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (y != 0 && dim <= 0) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }
    if (off & ~7) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_OFFSET;
    }

    return RC_PIXEL_GET_BIN(buf, dim, off, x, y);
}

RAPP_API(int, rapp_pixel_set_bin,
         (uint8_t *buf, int dim, int off, int x, int y, int value))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (y != 0 && dim <= 0) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }
    if (off & ~7) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_OFFSET;
    }
    if (value & ~1) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_RANGE;
    }

    RC_PIXEL_SET_BIN(buf, dim, off, x, y, value);

    return RAPP_OK;
}

RAPP_API(int, rapp_pixel_get_u8,
         (const uint8_t *buf, int dim, int x, int y))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (y != 0 && dim <= 0) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RC_PIXEL_GET_U8(buf, dim, x, y);
}

RAPP_API(int, rapp_pixel_set_u8,
         (uint8_t *buf, int dim, int x, int y, int value))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (y != 0 && dim <= 0) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }
    if (value & ~0xff) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_RANGE;
    }

    RC_PIXEL_SET_U8(buf, dim, x, y, value);

    return RAPP_OK;
}
