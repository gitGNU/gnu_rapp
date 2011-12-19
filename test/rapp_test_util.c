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
 *  @brief  RAPP test utilities.
 */

#include <stdlib.h>         /* rand()           */
#include <string.h>         /* memcmp()         */
#include "rapp.h"           /* RAPP API         */
#include "rapp_test_util.h" /* Test utility API */

/**
 *  These two functions, rapp_test_srand and rc_test_rand, are the only
 *  ones that need to be replaced when recreating random sequences or
 *  making repeatable across rand / srand implementations.
 */
int
rc_test_rand(void)
{
  return rand();
}

void
rapp_test_srand(unsigned int seed)
{
  srand(seed);
}

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
rapp_test_rand(int min, int max)
{
    return min + rc_test_rand() % (max - min + 1);
}

void
rapp_test_init(uint8_t *buf, int dim, int width, int height, bool rnd)
{
    int x, y, k;

    for (y = 0, k = 0; y < height; y++) {
        int i = y*dim;

        for (x = 0; x < width; x++, i++, k++) {
            buf[i] = rnd ? rc_test_rand() : k;
        }
    }
}

void
rapp_test_init_mask(uint8_t *buf, int bytes, int run)
{
    int len = 8*bytes;
    int pos = 0;

    while (pos < len) {
        int set = rapp_test_rand(1, run);
        int clr = rapp_test_rand(1, run);
        int k;

        /* Set the SET pixel run */
        set = MIN(set, len - pos);
        for (k = 0; k < set; k++) {
            rapp_pixel_set_bin(buf, 0, 0, pos + k, 0, 1);
        }
        pos += set;

        /* Set the CLEAR pixel run */
        clr = MIN(clr, len - pos);
        for (k = 0; k < clr; k++) {
            rapp_pixel_set_bin(buf, 0, 0, pos + k, 0, 0);
        }
        pos += clr;
    }
}

bool
rapp_test_compare_u8(const uint8_t *src1, int dim1,
                     const uint8_t *src2, int dim2,
                     int width, int height)
{
    int y;

    for (y = 0; y < height; y++) {
        if (memcmp(&src1[y*dim1], &src2[y*dim2], width) != 0) {
            return false;
        }
    }

    return true;
}

bool
rapp_test_compare_bin(const uint8_t *src1, int dim1,
                      const uint8_t *src2, int dim2,
                      int offset, int width, int height)
{
    uint8_t mhead = 0;
    uint8_t mtail = 0;
    int end   = (width + offset - 1) % 8 + 1;
    int len   = (width + offset + 7) / 8;
    int x, y;

    for (x = offset; x < 8; x++) {
        /**
         *  We somewhat abuse the return value of this function to also
         *  include testing the integrity of rapp_pixel_set_bin; that it
         *  always returns 0.
         */
        if (rapp_pixel_set_bin(&mhead, 0, 0, x, 0, 1))
            return false;
    }
    for (x = 0; x < end; x++) {
        if (rapp_pixel_set_bin(&mtail, 0, 0, x, 0, 1))
            return false;
    }

    if (len == 1) {
        mhead &= mtail;
    }

    for (y = 0; y < height; y++) {
        int i1 = y*dim1;
        int i2 = y*dim2;

        if ((src1[i1] ^ src2[i2]) & mhead) {
            return false;
        }

        for (x = 1, i1++, i2++;
             x < len - 1;
             x++, i1++, i2++)
        {
            if (src1[i1] ^ src2[i2]) {
                return false;
            }
        }

        if (x < len && ((src1[i1] ^ src2[i2]) & mtail)) {
            return false;
        }
    }

    return true;
}

void
rapp_test_dump_u8(const uint8_t *buf, int dim, int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        int i = y*dim;

        for (x = 0; x < width; x++, i++) {
            DBG("%02x ", rapp_pixel_get_u8(buf, dim, x, y));
        }

        DBG("\n");
    }
}

void
rapp_test_dump_bin(const uint8_t *buf, int dim, int off,
                   int width, int height)
{
    int x, y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            DBG("%c", rapp_pixel_get_bin(buf, dim, off, x, y) ? '1' : '.');
        }

        DBG("\n");
    }
}
