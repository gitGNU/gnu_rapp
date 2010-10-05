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
 *  @file   rapp_gather_bin.c
 *  @brief  RAPP binary pixel gather.
 */

#include "rappcompute.h"     /* RAPP Compute API  */
#include "rapp_api.h"        /* API symbol macro  */
#include "rapp_util.h"       /* Validation        */
#include "rapp_error.h"      /* Error codes       */
#include "rapp_error_int.h"  /* Error handling    */
#include "rapp_gather_bin.h" /* Binary gather API */

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Gather binary pixels designated by a binary mask.
 */
RAPP_API(int, rapp_gather_bin,
         (uint8_t *restrict pack,
          const uint8_t *restrict src, int src_dim,
          const uint8_t *restrict map, int map_dim,
          int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate arguments */
    if (!RAPP_VALIDATE_RESTRICT_PLUS(pack, 0, src, src_dim, height,
                                     /* Length unknown, assume 1 */
                                     1, rc_align((width + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_PLUS(pack, 0, map, map_dim, height,
                                     /* Length unknown, assume 1 */
                                     1, rc_align((width + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(pack, RC_ALIGNMENT, 1, 1)) {
        return rapp_error_bin(pack, RC_ALIGNMENT, 1, 1);
    }
    if (!RAPP_VALIDATE_BIN(src, src_dim, width, height) ||
        !RAPP_VALIDATE_BIN(map, map_dim, width, height))
    {
        /* Return the error code */
        return rapp_error_bin_bin(src, src_dim, width, height,
                                  map, map_dim, width, height);
    }

    /* Perform operation */
    return rc_gather_bin(pack, src, src_dim, map, map_dim, width, height);
}
