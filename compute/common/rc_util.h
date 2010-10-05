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
 *  @file   rc_util.h
 *  @brief  Utility constants and macros.
 */

#ifndef RC_UTIL_H
#define RC_UTIL_H

#include "rc_platform.h" /* RC_ALIGNMENT */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Standard MIN().
 */
#undef  MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 *  Standard MAX().
 */
#undef  MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 *  Standard CLAMP().
 */
#undef  CLAMP
#define CLAMP(val, lo, hi) \
    ((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))

/**
 *  Integer division, rounded up.
 */
#define RC_DIV_CEIL(num, den) \
     (((num) + (den) - 1) / (den))

/**
 *  Round up 'value' to a multiple of RC_ALIGNMENT.
 */
#define RC_ALIGN(value) \
    (RC_ALIGNMENT*RC_DIV_CEIL(value, RC_ALIGNMENT))

#endif /* RC_UTIL_H */
