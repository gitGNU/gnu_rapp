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
 *  @file   rapp_rasterize.c
 *  @brief  RAPP chain code line rasterization.
 */

#include "rappcompute.h"     /* RAPP Compute API  */
#include "rapp_api.h"        /* API symbol macro  */
#include "rapp_util.h"       /* Validation        */
#include "rapp_error.h"      /* Error codes       */
#include "rapp_error_int.h"
#include "rapp_rasterize.h"  /* Rasterization API */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Rasterize a 4-connectivity chain code.
 */
RAPP_API(int, rapp_rasterize_4conn,
         (char *line, int x0, int y0, int x1, int y1))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!line) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }

    return rc_rasterize_4conn(line, x0, y0, x1, y1);
}


/**
 *  Rasterize an 8-connectivity chain code.
 */
RAPP_API(int, rapp_rasterize_8conn,
         (char *line, int x0, int y0, int x1, int y1))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!line) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_NULL;
    }

    return rc_rasterize_8conn(line, x0, y0, x1, y1);
}
