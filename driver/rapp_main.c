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
 *  @file   rapp_main.c
 *  @brief  RAPP library initialization.
 */

#include "rapp_main.h"
#include "rapp_api.h"
#include "rapp_util.h"
#include "rapp_error.h"

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int rc_initialized = 0;

/**
 *  Initialize the library. Empty for now, except for verification that
 *  it hasn't been called before, not without a matching call to
 *  rapp_terminate.
 */
RAPP_API(void, rapp_initialize, (void))
{
    if (RAPP_INITIALIZED()) {
        return;
    }

    rc_initialized = 1;
}

/**
 *  Finish working with the library. Empty for now, except for
 *  book-keeping that there has been a call.
 */
RAPP_API(void, rapp_terminate, (void))
{
    rc_initialized = 0;
}
