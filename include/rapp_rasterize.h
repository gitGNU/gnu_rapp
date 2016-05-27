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
 *  @file   rapp_rasterize.h
 *  @brief  RAPP chain code line rasterization.
 */

/**
 *  @defgroup grp_rasterize Chain Code Line Rasterization
 *
 *  <p>@ref conditional "Next section: Conditional Operations"</p>
 *
 *  @{
 */

#ifndef RAPP_RASTERIZE_H
#define RAPP_RASTERIZE_H

#include <stdint.h>
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
 *  Rasterize a 4-connectivity chain code.
 *
 *  @param[out] line  Chain code string buffer with at least
 *                    abs(xe - xs) + abs(ye - ys) + 1 bytes.
 *  @param      xs    Start position x coordinate.
 *  @param      ys    Start position y coordinate.
 *  @param      xe    End position x coordinate.
 *  @param      ye    End position y coordinate.
 *  @return           The number of codes written to the buffer,
 *                    not including the trailing '\\0' character,
 *                    or a negative error code on error.
 */
RAPP_EXPORT int
rapp_rasterize_4conn(char *line, int xs, int ys, int xe, int ye);


/**
 *  Rasterize an 8-connectivity chain code.
 *
 *  @param[out] line  Chain code string buffer with at least
 *                    max(abs(xe - xs), abs(ye - ys)) + 1 bytes.
 *  @param      xs    Start position x coordinate.
 *  @param      ys    Start position y coordinate.
 *  @param      xe    End position x coordinate.
 *  @param      ye    End position y coordinate.
 *  @return           The number of codes written to the buffer,
 *                    not including the trailing '\\0' character,
 *                    or a negative error code on error.
 */
RAPP_EXPORT int
rapp_rasterize_8conn(char *line, int xs, int ys, int xe, int ye);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_RASTERIZE_H */
/** @} */
