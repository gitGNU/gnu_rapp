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
 *  @file   rapp_info.h
 *  @brief  RAPP build information string.
 */

/**
 *  @defgroup grp_info Build Information
 *  The build information string contains a description of
 *  a particular RAPP build.
 *  The value of #rapp_info is undefined before rapp_initialize()
 *  has been called.
 *  <!-- By mentioning that undefinedness, we make future
 *  initialization-time-switching between tuned alternatives, where the
 *  alternative is mentioned in rapp_info, API compliant with this
 *  version. -->
 *
 *  <p>@ref grp_memory "Next section: Memory Allocation"</p>
 *
 *  @{
 */

#ifndef RAPP_INFO_H
#define RAPP_INFO_H

#include "rapp_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  The RAPP build information string.
 */
extern const RAPP_EXPORT char *rapp_info;

#ifdef __cplusplus
};
#endif

#endif /* RAPP_INFO_H */
/** @} */
