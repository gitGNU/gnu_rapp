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
 *  @file   rapp_test_util.c
 *  @brief  RAPP correctness test utilities.
 */

#ifndef RAPP_TEST_UTIL_H
#define RAPP_TEST_UTIL_H

#include <stdio.h>      /* printf()           */
#include <stdint.h>     /* uint8_t            */
#include "rc_stdbool.h" /* Portable stdbool.h */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Debug print macro.
 */
#define DBG printf

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

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Integral random number in the range [min,max].
 */
int
rapp_test_rand(int min, int max);

/**
 *  Initialize a 2-dimensional byte array.
 */
void
rapp_test_init(uint8_t *buf, int dim, int width, int height, bool rnd);

/**
 *  Initialize a binary mask buffer with random runs of zeros and ones.
 */
void
rapp_test_init_mask(uint8_t *buf, int bytes, int run);

/**
 *  Binary image comparison.
 */
bool
rapp_test_compare_bin(const uint8_t *src1, int dim1,
                      const uint8_t *src2, int dim2,
                      int offset, int width, int height);
/**
 *  8-bit image comparison.
 */
bool
rapp_test_compare_u8(const uint8_t *src1, int dim1,
                     const uint8_t *src2, int dim2,
                     int width, int height);
/**
 *  Binary image printout.
 */
void
rapp_test_dump_bin(const uint8_t *buf, int dim, int off,
                   int width, int height);
/**
 *  8-bit image printout.
 */
void
rapp_test_dump_u8(const uint8_t *buf, int dim, int width, int height);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_TEST_UTIL_H */
