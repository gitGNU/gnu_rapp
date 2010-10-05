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
 *  @file   rc_impl.h
 *  @brief  RAPP Compute layer implementation-specific symbolic names.
 */

#ifndef RC_IMPL_H
#define RC_IMPL_H

/**
 *  No implementation.
 */
#define RC_IMPL_NONE 0

/**
 *  A generic scalar implementation.
 *  This is a fallback available on all platforms.
 */
#define RC_IMPL_GEN 1

/**
 *  A SWAR (SIMD-Within-A-Register) vector implementation.
 */
#define RC_IMPL_SWAR 2

/**
 *  A vector implementation using SIMD instructions.
 */
#define RC_IMPL_SIMD 3

#endif /* RC_IMPL_H */
