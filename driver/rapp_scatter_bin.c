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
 *  @file   rapp_scatter_bin.c
 *  @brief  RAPP binary pixel scatter.
 */

#include "rappcompute.h"      /* RAPP Compute API   */
#include "rapp_api.h"         /* API symbol macro   */
#include "rapp_util.h"        /* Validation         */
#include "rapp_error.h"       /* Error codes        */
#include "rapp_error_int.h"   /* Error handling     */
#include "rapp_scatter_bin.h" /* Binary scatter API */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Scatter binary pixels designated by a binary mask.
 */
RAPP_API(int, rapp_scatter_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict map, int map_dim,
          const uint8_t *restrict pack, int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(pack, 0, dst, dst_dim, height,
                                     /* Pack length unknown, validate 1 */
                                     1, rc_align((width + 7) / 8)))
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT(map, map_dim, dst, dst_dim, height,
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
    if (!RAPP_VALIDATE_BIN(pack, RC_ALIGNMENT, 1, 1)) {
        return rapp_error_bin(pack, RC_ALIGNMENT, 1, 1);
    }

    /* Perform operation */
    return rc_scatter_bin(dst, dst_dim, map, map_dim, pack, width, height);
}
