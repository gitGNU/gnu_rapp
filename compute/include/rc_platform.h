/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_platform.h
 *  @brief  RAPP Compute platform-specific definitions.
 */

#ifndef RC_PLATFORM_H
#define RC_PLATFORM_H

/* Include config.h generated by configure */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>  /* Fixed-size integers     */
#include <limits.h>  /* UINTPTR_MAX, USHORT_MAX */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The GCC version number.
 */
#define RC_GCC_VERSION (1000*__GNUC__ + __GNUC_MINOR__)

/**
 *  The CPU endianness.
 */
#ifdef WORDS_BIGENDIAN
#define RC_BIG_ENDIAN 1
#else
#define RC_LITTLE_ENDIAN 1
#endif

/**
 *  The native machine word size in bytes.
 *  Deduce from maximum values unless explicitly specified.
 */
#ifdef  RAPP_FORCE_SIZE
#define RC_NATIVE_SIZE RAPP_FORCE_SIZE

#else

/* See configure.ac; it's trivial but better have it in one place only. */
#define RC_NATIVE_SIZE RC_COMPUTED_NATIVE_SIZE

#endif

/**
 *  The maximum of the required buffer alignment value in bytes for
 *  vector memory access and the vector size: the latter can be bigger
 *  than the former.
 */
#if defined __SSE2__ || defined __VEC__ || defined __ALTIVEC__ || \
    defined __SSSE3__ || defined __VADMX2__
#define RC_ALIGNMENT 16

#elif defined __MMX__ || defined __SSE__ || defined __VADMX__ || \
      defined __mips_loongson_vector_rev || defined __ARM_NEON__ || \
      defined RC_VEC_BACKEND_VIS || \
      RC_NATIVE_SIZE == 8
#define RC_ALIGNMENT 8

#else
#define RC_ALIGNMENT 4
#endif

/**
 *  No-inline hint.
 */
#if defined __GNUC__ && RC_GCC_VERSION > 3001
#define RC_NOINLINE __attribute__((noinline))
#else
#define RC_NOINLINE
#endif


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Branch prediction hints.
 *  Use RC_LIKELY()/RC_UNLIKELY to indicate the preferred branch.
 */
#if defined __GNUC__ && RC_GCC_VERSION > 2095
#define RC_LIKELY(expr)   __builtin_expect((expr) != 0, 1)
#define RC_UNLIKELY(expr) __builtin_expect((expr), 0)
#else
#define RC_LIKELY(expr)   (expr)
#define RC_UNLIKELY(expr) (expr)
#endif

#endif /* RC_PLATFORM_H */
