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
 *  @file   rapp_fill.c
 *  @brief  RAPP connected-components seed fill.
 */

#include "rappcompute.h"    /* RAPP Compute API */
#include "rapp_api.h"       /* API symbol macro */
#include "rapp_util.h"      /* Validation       */
#include "rapp_error.h"     /* Error codes      */
#include "rapp_error_int.h" /* Error handling   */
#include "rapp_fill.h"      /* Seed fill API    */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_fill_driver(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict map, int map_dim,
                 int width, int height, int xseed, int yseed,
                 int (*fwd)(), int (*rev)());


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  4-connectivity connected-components seed fill.
 */
RAPP_API(int, rapp_fill_4conn_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict map, int map_dim,
          int width, int height, int xseed, int yseed))
{
    return rapp_fill_driver(dst, dst_dim, map, map_dim,
                            width, height, xseed, yseed,
                            &rc_fill_4conn_fwd_bin,
                            &rc_fill_4conn_rev_bin);
}


/**
 *  8-connectivity connected-components seed fill.
 */
RAPP_API(int, rapp_fill_8conn_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict map, int map_dim,
          int width, int height, int xseed, int yseed))
{
    return rapp_fill_driver(dst, dst_dim, map, map_dim,
                            width, height, xseed, yseed,
                            &rc_fill_8conn_fwd_bin,
                            &rc_fill_8conn_rev_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rapp_fill_driver(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict map, int map_dim,
                 int width, int height, int xseed, int yseed,
                 int (*fwd)(), int (*rev)())
{
    int row = yseed; /* Fill start row marker     */
    int inc = 0;     /* Filled rows per iteration */
    int cnt = 0;     /* Iteration counter         */

    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT(dst, dst_dim, map, map_dim, height,
                                (width + 7) / 8))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width, height) ||
        !RAPP_VALIDATE_BIN(map, map_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_bin(dst, dst_dim, width, height,
                                  map, map_dim, width, height);
    }
    if (xseed < 0 || xseed >= width ||
        yseed < 0 || yseed >= height)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_PARM_RANGE;
    }

    /* Clear the destination buffer */
    rc_pixop_set_u8(dst, dst_dim, rc_align((width + 7) / 8), height, 0);

    /* Check if the seed is in the map */
    if (!RC_PIXEL_GET_BIN(map, map_dim, 0, xseed, yseed)) {
        return RAPP_OK;
    }

    /* Set the seed pixel */
    RC_PIXEL_SET_BIN(dst, dst_dim, 0, xseed, yseed, 1);

    /* Perform seed fill until no change */
    do {
        /* Forward iteration */
        inc = (*fwd)(&dst[row*dst_dim], dst_dim,
                     &map[row*map_dim], map_dim,
                     width, height - row);

        /* Set the next start row as the last row processed */
        row += inc ? inc - 1 : 0;

        if (inc > 0 || cnt == 0) {
            /* Reverse iteration */
            inc = (*rev)(dst, dst_dim, map, map_dim, width, row + 1);

            /* Set the next start row */
            row += 1 - inc;
        }

        /* One more iteration pair */
        cnt++;
    } while (inc > 0);

    return RAPP_OK;
}
