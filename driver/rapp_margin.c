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
 *  @file   rapp_margin.c
 *  @brief  RAPP binary logical margins.
 */

#include "rappcompute.h"    /* RAPP Compute API  */
#include "rapp_api.h"       /* API symbol macro  */
#include "rapp_util.h"      /* Validation        */
#include "rapp_error.h"     /* Error codes       */
#include "rapp_error_int.h" /* Error handling    */
#include "rapp_margin.h"    /* Binary margin API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Compute the horizontal logical margin.
 */
RAPP_API(int, rapp_margin_horz_bin,
         (uint8_t *restrict margin, const uint8_t *restrict src,
          int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(src, dim, margin, 0, height,
                                (width + 7) / 8))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(margin, dim, width, 1) ||
        !RAPP_VALIDATE_BIN(src,    dim, width, height))
    {
        return rapp_error_bin_bin(margin, dim, width, 1,
                                  src,    dim, width, height);
    }

    /* Perform operation */
    rc_pixop_set_u8(margin, 0, (width + 7) / 8, 1, 0);
    rc_margin_horz_bin(margin, src, dim, width, height);

    return RAPP_OK;
}


/**
 *  Compute the vertical logical margin.
 */
RAPP_API(int, rapp_margin_vert_bin,
         (uint8_t *restrict margin, const uint8_t *restrict src,
          int dim, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(src, dim, margin, 0, height,
                                     rc_align((width + 7) / 8),
                                     rc_align((height + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(margin, RC_ALIGNMENT, 1,     height) ||
        !RAPP_VALIDATE_BIN(src,    dim,          width, height))
    {
        return rapp_error_bin_bin(margin, RC_ALIGNMENT, 1,     height,
                                  src,    dim,          width, height);
    }

    /* Perform operation */
    rc_pixop_set_u8(margin, 0, (height + 7) / 8, 1, 0);
    rc_margin_vert_bin(margin, src, dim, width, height);

    return RAPP_OK;
}
