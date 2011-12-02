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
 *  @file   rapp_main.h
 *  @brief  RAPP library initialization.
 */

/**
 *  @defgroup grp_init Library Initialization
 *  Before using any RAPP library function or data (with the notable
 *  exception of rapp_error()), the library must be initialized.
 *  This is done by calling the rapp_initialize()
 *  function once, typically from the main thread when the
 *  application starts. When finished using the library, the
 *  function rapp_terminate() should be called once. This is normally
 *  done from the main thread before the application terminates.
 *
 *  Note that even though the operations in the RAPP library are
 *  thread-safe, the rapp_initialize() / rapp_terminate() functions
 *  themselves are not.
 *
 *  <p>@ref grp_info "Next section: Build Information"</p>
 *
 *  @{
 */

#ifndef RAPP_MAIN_H
#define RAPP_MAIN_H

#include "rapp_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Initialize the RAPP library.
 *  This function must be called at least once for each process
 *  before using any RAPP library function. This function is not
 *  thread-safe.
 */
RAPP_EXPORT void
rapp_initialize(void);

/**
 *  Terminate the RAPP library.
 *  This function must be called at least once for each process
 *  after using the RAPP library. This function is not thread-safe.
 */
RAPP_EXPORT void
rapp_terminate(void);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_MAIN_H */
/** @} */
