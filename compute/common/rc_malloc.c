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
 *  @file   rc_malloc.c
 *  @brief  Memory allocation.
 */

#include <stdlib.h>    /* malloc(), _aligned_malloc(), free() */
#if defined __linux__ || defined __CYGWIN__
#include <malloc.h>    /* memalign()                          */
#endif

#include "rc_util.h"   /* RC_ALIGNMENT, RC_ALIGN()            */
#include "rc_malloc.h" /* Exported API                        */


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

const unsigned int rc_alignment = RC_ALIGNMENT;


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

size_t
rc_align(size_t size)
{
    return RC_ALIGN(size);
}

void*
rc_malloc(size_t size)
{
    size = RC_ALIGN(size);

#if RC_ALIGNMENT <= RC_NATIVE_SIZE
    /* Use standard malloc() for normal alignment restrictions */
    return malloc(size);

#elif defined __APPLE__
    /* On OSX malloc() uses 128-bit alignment */
    return malloc(size);

#elif defined __WIN32__
    /* Use special aligned malloc */
    return _aligned_malloc(size, RC_ALIGNMENT);

#elif defined __linux__ || defined __CYGWIN__
    /* Use the memalign() function */
    return memalign(RC_ALIGNMENT, size);

#else
#error "Unknown architecture"
#endif
}

void
rc_free(void *ptr)
{
#if RC_ALIGNMENT <= RC_NATIVE_SIZE
    /* Use standard free() for normal alignment restrictions */
    free(ptr);

#elif defined __APPLE__ || defined __linux__ || defined __CYGWIN__
    /* Memory allocated by malloc() or memalign() - use free() */
    free(ptr);

#elif defined __WIN32__
    /* Use special aligned free */
    _aligned_free(ptr);

#else
#error "Unknown architecture"
#endif
}
