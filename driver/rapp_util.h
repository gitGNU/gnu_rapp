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
 *  @file   rapp_util.h
 *  @brief  Utility constants and macros.
 */

#ifndef RAPP_UTIL_H
#define RAPP_UTIL_H

#include <stdint.h>      /* uintptr_t                     */
#include "rappcompute.h" /* Platform-specific definitions */

/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  OK return value.
 */
#define RAPP_OK 0


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
#define MAX(a, b) ((a) < (b) ? (a) : (b))

/**
 *  Standard CLAMP().
 */
#undef  CLAMP
#define CLAMP(val, lo, hi) \
    ((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))

/**
 *  Validate parameters for an aligned binary image.
 */
#define RAPP_VALIDATE_BIN(ptr, dim, width, height) \
    ((ptr) != NULL                         &&      \
     (uintptr_t)(ptr) % RC_ALIGNMENT == 0  &&      \
                (dim) % RC_ALIGNMENT == 0  &&      \
     (int)(width) > 0 && (int)(height) > 0 &&      \
     (int)(width) <= 8*(int)(dim))

/**
 *  Validate parameters for an aligned u8 image.
 */
#define RAPP_VALIDATE_U8(ptr, dim, width, height) \
    RAPP_VALIDATE_U(ptr, dim, width, height)

/**
 *  Validate parameters for an aligned u16 image.
 */
#define RAPP_VALIDATE_U16(ptr, dim, width, height) \
    RAPP_VALIDATE_U(ptr, dim, width, height)

/**
 *  Validate parameters for an aligned u32 image.
 */
#define RAPP_VALIDATE_U32(ptr, dim, width, height) \
    RAPP_VALIDATE_U(ptr, dim, width, height)

/**
 *  Validate parameters for an aligned unsigned images.
 */
#define RAPP_VALIDATE_U(ptr, dim, width, height)       \
    ((ptr) != NULL                         &&          \
     (uintptr_t)(ptr) % RC_ALIGNMENT == 0  &&          \
                (dim) % RC_ALIGNMENT == 0  &&          \
     (int)(width) > 0 && (int)(height) > 0 &&          \
     (int)(width) <= (int)(dim) / (int) sizeof *(ptr))

/**
 *  Validate parameters for a binary image without alignment restrictions.
 */
#define RAPP_VALIDATE_NOALIGN_BIN(ptr, dim, off, width, height) \
    ((ptr) != NULL                             &&               \
     (dim) % RC_ALIGNMENT == 0                 &&               \
     (int)(off) >= 0 && (int)(off) < 8         &&               \
     (int)(width) + (int)(off) <= 8*(int)(dim) &&               \
     (int)(width) > 0 && (int)(height) > 0)

/**
 *  Validate parameters for a u8 image without alignment restrictions.
 */
#define RAPP_VALIDATE_NOALIGN_U8(ptr, dim, width, height) \
    ((ptr) != NULL              &&                        \
     (dim) % RC_ALIGNMENT == 0  &&                        \
     (int)(width) <= (int)(dim) &&                        \
     (int)(width) > 0 && (int)(height) > 0)

/**
 *  Verify that rapp_initialize() has been called.
 */
#define RAPP_INITIALIZED() (rc_initialized != 0)

/**
 *  Validate that two buffers do not overlap.
 */
#define RAPP_VALIDATE_RESTRICT(p1, d1, p2, d2, h, wb) \
    RAPP_VALIDATE_RESTRICT_PLUS(p1, d1, p2, d2, h,    \
                                rc_align(wb), rc_align(wb))

#define RAPP_VALIDATE_RESTRICT_PLUS(p1, d1, p2, d2, h, of1, of2) \
    RAPP_VALIDATE_RESTRICT_OFFSET(p1, d1, p2, d2, h, 0, of1, 0, of2)

#define RAPP_VALIDATE_RESTRICT_OFFSET(p1, d1, p2, d2, h,    \
                                      nof1, of1, nof2, of2) \
    (((uint8_t *) (p1) + (d1) * ((h) - 1) + (of1) <=        \
      (uint8_t *) (p2) + (nof2)) ||                         \
     ((uint8_t *) (p2) + (d2) * ((h) - 1) + (of2) <=        \
      (uint8_t *) (p1) + (nof1)))

#endif /* RAPP_UTIL_H */
