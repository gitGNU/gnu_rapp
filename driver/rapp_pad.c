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
 *  @file   rapp_pad.c
 *  @brief  RAPP 8-bit image padding.
 */

#include "rappcompute.h"    /* RAPP Compute API  */
#include "rapp_api.h"       /* API symbol macro   */
#include "rapp_util.h"      /* Validation        */
#include "rapp_error.h"     /* Error codes       */
#include "rapp_error_int.h" /* Error handling    */
#include "rapp_pad.h"       /* 8-bit padding API */


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rapp_pad_align(uint8_t **vbuf, int *vlen, uint8_t *buf, int width, int size);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Set all pixels up the nearest alignment boundary to value.
 */
RAPP_API(int, rapp_pad_align_u8,
         (uint8_t *buf, int dim, int width, int height, int value))
{
    int left  = (uintptr_t)buf % RC_ALIGNMENT;
    int right = RC_ALIGNMENT - (left + width - 1) % RC_ALIGNMENT - 1;

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_NOALIGN_U8(buf, dim, width, height)) {
        return rapp_error_noalign_u8(buf, dim, width, height);
    }

    assert( width + left + right <= dim);
    assert((width + left + right) % RC_ALIGNMENT == 0);

    /* Use the faster binary padding functions if possible */
    if (value == 0) {
        rc_pad_const_left_bin (buf, dim, 0, 8*width, height, 8*left,  0);
        rc_pad_const_right_bin(buf, dim, 0, 8*width, height, 8*right, 0);
    } else if (value == UINT8_MAX) {
        rc_pad_const_left_bin (buf, dim, 0, 8*width, height, 8*left,  1);
        rc_pad_const_right_bin(buf, dim, 0, 8*width, height, 8*right, 1);
    } else {
        rc_pad_const_left_u8 (buf, dim, width, height, left,  value);
        rc_pad_const_right_u8(buf, dim, width, height, right, value);
    }

    return RAPP_OK;
}


/**
 *  Pad an 8-bit image with a constant value.
 */
RAPP_API(int, rapp_pad_const_u8,
         (uint8_t *buf, int dim, int width, int height, int size, int value))
{
    uint8_t *vbuf; /* Vector-aligned buffer            */
    int      vlen; /* Vector-aligned padded row length */

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_NOALIGN_U8(buf, dim, width + 2*size, height)) {
        return rapp_error_noalign_u8(buf, dim, width + 2*size, height);
    }

    /* Compute the aligned top/bottom padding parameters  */
    rapp_pad_align(&vbuf, &vlen, buf, width, size);

    /* Pad the top edge */
    rc_pixop_set_u8(&vbuf[-size*dim], dim, vlen, size, value);

    /* Pad the left edge */
    rc_pad_const_left_u8(buf, dim, width, height, size, value);

    /* Pad the right edge */
    rc_pad_const_right_u8(buf, dim, width, height, size, value);

    /* Pad the bottom edge */
    rc_pixop_set_u8(&vbuf[height*dim], dim, vlen, size, value);

    return RAPP_OK;
}


/**
 *  Pad an 8-bit image by clamping the edge value.
 */
RAPP_API(int, rapp_pad_clamp_u8,
         (uint8_t *buf, int dim, int width, int height, int size))
{
    uint8_t *vbuf; /* Vector-aligned buffer            */
    int      vlen; /* Vector-aligned padded row length */
    int      last; /* Last row index                   */
    int      y, i;

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_NOALIGN_U8(buf, dim, width + 2*size, height)) {
        return rapp_error_noalign_u8(buf, dim, width + 2*size, height);
    }

    /* Compute the aligned top/bottom padding parameters  */
    rapp_pad_align(&vbuf, &vlen, buf, width, size);

    /* Pad the left edge */
    rc_pad_clamp_left_u8(buf, dim, width, height, size);

    /* Pad the right edge */
    rc_pad_clamp_right_u8(buf, dim, width, height, size);

    /* Pad the top edge */
    for (y = 0, i = -size*dim; y < size; y++, i += dim) {
        rc_bitblt_va_copy_bin(&vbuf[i], dim, vbuf, dim, 8*vlen, 1);
    }

    /* Pad the bottom edge */
    last = (height - 1)*dim;
    for (y = 0, i = height*dim; y < size; y++, i += dim) {
        rc_bitblt_va_copy_bin(&vbuf[i], dim, &vbuf[last], dim, 8*vlen, 1);
    }

    return RAPP_OK;
}

/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

/**
 *  Get the vector-aligned buffer and length in bytes
 *  for the top row including padding.
 */
static void
rapp_pad_align(uint8_t **vbuf, int *vlen, uint8_t *buf, int width, int size)
{
    /* Compute the aligned buffer parameters  */
    *vbuf = (uint8_t*)((intptr_t)&buf[-size] & ~(RC_ALIGNMENT - 1));
    *vlen = (uintptr_t)&buf[-size] % RC_ALIGNMENT + width + 2*size;

    /* The vector buffer is aligned */
    assert((uintptr_t)*vbuf % RC_ALIGNMENT == 0);
}
