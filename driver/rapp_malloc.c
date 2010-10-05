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
 *  @file   rapp_malloc.c
 *  @brief  RAPP aligned memory allocation.
 */

#include "rappcompute.h" /* RAPP Compute API */
#include "rapp_api.h"    /* API symbol macro */
#include "rapp_util.h"   /* Validation       */
#include "rapp_malloc.h" /* Malloc API       */

/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

const unsigned int rapp_alignment = RC_ALIGNMENT;


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

RAPP_API(size_t, rapp_align, (size_t size))
{
    if (!RAPP_INITIALIZED()) {
        return (size_t) -1;
    }

    return rc_align(size);
}

RAPP_API(void*, rapp_malloc, (size_t size, unsigned hint))
{
    if (!RAPP_INITIALIZED()) {
        return NULL;
    }

    return hint ? NULL : rc_malloc(size);
}

RAPP_API(void, rapp_free, (void *ptr))
{
    if (!RAPP_INITIALIZED()) {
        return;
    }

    rc_free(ptr);
}
