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
 *  @file   rc_vector.h
 *  @brief  RAPP Compute layer vector operation backend.
 */

#ifndef RC_VECTOR_H
#define RC_VECTOR_H

#include "rc_platform.h" /* Platform-specific definitions */
#include "rc_impl.h"     /* Implementation names          */

#ifdef RAPP_USE_SIMD

#if RAPP_ENABLE_SIMD

/* Set the vector implementation to SIMD */
#define RC_VEC_IMPL RC_IMPL_SIMD
#include RAPP_VECTOR_BACKEND_HEADER

#else  /* ! RAPP_ENABLE_SIMD */

/* No available SIMD vector implementation or explicitly turned off */
#undef  RC_VEC_IMPL
#define RC_VEC_IMPL RC_IMPL_NONE

#endif

#else

/* Set the vector implementation to SWAR */
#define RC_VEC_IMPL RC_IMPL_SWAR

/* Use the SWAR (soft-SIMD) implementation */
#include "rc_vec_swar.h"

#endif /* !RAPP_USE_SIMD */

#endif /* RC_VECTOR_H */
