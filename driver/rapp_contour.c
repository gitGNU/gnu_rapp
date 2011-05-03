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
 *  @file   rapp_contour.c
 *  @brief  RAPP contour chain code generation.
 */

#include "rappcompute.h"    /* RAPP Compute API */
#include "rapp_api.h"       /* API symbol macro */
#include "rapp_util.h"      /* Validation       */
#include "rapp_error.h"     /* Error codes      */
#include "rapp_error_int.h" /* Error handling   */
#include "rapp_contour.h"   /* Contour API      */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Validate the contour buffer.
 */
#define RAPP_CONTOUR_VALD(contour, len) \
    ((!contour && len == 0) || (contour && len > 0))


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  4-connectivity contour chain code.
 */
RAPP_API(int, rapp_contour_4conn_bin,
         (unsigned origin[2], char *restrict contour, int len,
          const uint8_t *restrict buf, int dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (contour != NULL &&
        !RAPP_VALIDATE_RESTRICT_PLUS(buf, dim, contour, 0, height,
                                     rc_align((width + 7) / 8), len))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!origin) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }
    if (!RAPP_CONTOUR_VALD(contour, len)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_RANGE;
    }
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return rapp_error_bin(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_contour_4conn_bin(origin, contour, len, buf, dim, width, height);
}

/**
 *  8-connectivity contour chain code.
 */
RAPP_API(int, rapp_contour_8conn_bin,
         (unsigned origin[2], char *restrict contour, int len,
          const uint8_t *restrict buf, int dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (contour != NULL &&
        !RAPP_VALIDATE_RESTRICT_PLUS(buf, dim, contour, 0, height,
                                     rc_align((width + 7) / 8), len))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!origin) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }
    if (!RAPP_CONTOUR_VALD(contour, len)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_RANGE;
    }
    if (!RAPP_VALIDATE_BIN(buf, dim, width, height)) {
        return rapp_error_bin(buf, dim, width, height);
    }

    /* Perform operation */
    return rc_contour_8conn_bin(origin, contour, len, buf, dim, width, height);
}
