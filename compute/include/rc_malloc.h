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
 *  @file   rc_malloc.h
 *  @brief  Memory allocation.
 */

#ifndef RC_MALLOC_H
#define RC_MALLOC_H

#include <stdlib.h>    /* size_t    */
#include "rc_export.h" /* RC_EXPORT */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  The required pixel buffer and row length alignment in bytes.
 */
extern const unsigned int rc_alignment;


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Align (round up) to the next alignment boundary.
 */
RC_EXPORT size_t
rc_align(size_t size);

/**
 *  Allocate an aligned chunk of memory.
 */
RC_EXPORT void*
rc_malloc(size_t size);

/**
 *  Free an aligned chunk of memory.
 */
RC_EXPORT void
rc_free(void *ptr);

#ifdef __cplusplus
};
#endif

#endif /* RC_MALLOC_H */
