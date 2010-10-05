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
 *  @file   rc_rasterize.h
 *  @brief  RAPP Compute chain code line rasterization.
 */

#ifndef RC_RASTERIZE_H
#define RC_RASTERIZE_H

#include <stdint.h>
#include "rc_export.h"

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
 *  @param[out]  line  Line chain code string with at least
 *                     abs(x1 - x0) + abs(y1 - y0) + 1 bytes.
 *  @param       x0    X coordinate of the start position.
 *  @param       y0    Y coordinate of the start position.
 *  @param       x1    X coordinate of the end position.
 *  @param       y1    Y coordinate of the end position.
 *  @return            The length of the chain code, excluding the
 *                     NUL-termination.
 */
RC_EXPORT int
rc_rasterize_4conn(char *line, int x0, int y0, int x1, int y1);


/**
 *  Rasterize an 8-connectivity chain code.
 *
 *  @param[out]  line  Line chain code string with at least
 *                     max(abs(x1 - x0), abs(y1 - y0)) + 1 bytes.
 *  @param       x0    X coordinate of the start position.
 *  @param       y0    Y coordinate of the start position.
 *  @param       x1    X coordinate of the end position.
 *  @param       y1    Y coordinate of the end position.
 *  @return            The length of the chain code, excluding the
 *                     NUL-termination.
 */
RC_EXPORT int
rc_rasterize_8conn(char *line, int x0, int y0, int x1, int y1);


#ifdef __cplusplus
};
#endif

#endif /* RC_RASTERIZE_H */
