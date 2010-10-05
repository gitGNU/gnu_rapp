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
 *  @file   rapp_malloc.h
 *  @brief  RAPP aligned memory allocation.
 */

/**
 *  @defgroup grp_memory Memory Allocation
 *  @brief Aligned memory allocator.
 *
 *  This is an allocator for allocating aligned pixel buffers
 *  suitable for use with the RAPP library. The size of the allocated
 *  memory is rounded up to the nearest multiple of #rapp_alignment.
 *  The value of rapp_alignment is undefined before rapp_initialize()
 *  has been called.
 *  Memory allocated by this allocator must never be released by another
 *  allocator like malloc()/free() or vice-versa.
 *
 *  There is a mechanism for passing allocation strategy hints.
 *  Currently no hints are available, so the value 0 must always
 *  be used.
 *
 *  <p>@ref grp_error "Next section: Error Handling"</p>
 *
 *  @{
 */

#ifndef RAPP_MALLOC_H
#define RAPP_MALLOC_H

#include <stdlib.h> /* size_t */
#include "rapp_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The required pixel buffer and row length alignment in bytes.
 *  The value is at least four bytes.
 */
extern const RAPP_EXPORT unsigned int rapp_alignment;


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Align (round up) to the next alignment boundary.
 *
 *  @param size  The required size in bytes.
 *  @return      The nearest number greater than or equal to size
 *               that is a multiple of #rapp_alignment.
 */
RAPP_EXPORT size_t
rapp_align(size_t size);

/**
 *  Allocate an aligned chunk of memory.
 *
 *  @param size  The number of bytes to allocate.
 *  @param hint  Allocation hint flags. Pass 0 for no hint.
 *  @return      An aligned pointer to allocated memory of size
 *               rapp_align(size), or NULL if memory allocation failed.
 */
RAPP_EXPORT void*
rapp_malloc(size_t size, unsigned hint);

/**
 *  Free an aligned chunk of memory.
 *
 *  @param  ptr  An aligned pointer obtained from rapp_malloc().
 *               If the value is NULL no action is performed.
 */
RAPP_EXPORT void
rapp_free(void *ptr);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_MALLOC_H */
/** @} */
