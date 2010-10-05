/*  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_pixel.c
 *  @brief  Correctness tests for pixel access functions.
 */

#include "rapp.h"           /* RAPP API      */
#include "rapp_test_util.h" /* Test utils    */

/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH  256


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_pixel_set_u8(void)
{
    uint8_t *buf;
    uint8_t val;
    int dim;
    int x;
    bool ok = true;

    /* Allocate image */
    dim = rapp_align(RAPP_TEST_WIDTH);
    buf = rapp_malloc(dim, 0);

    /* Set pixel in image */
    for (x = 0, val = 0; x < RAPP_TEST_WIDTH; x++, val += 1) {
        if (rapp_pixel_set_u8(buf, dim, x, 0, val) != 0) {
            DBG("Function exited with an error\n");
            ok = false;
            goto Done;
        }
    }

    /* Verify */
    for (x = 0, val = 0; x < RAPP_TEST_WIDTH; x++, val += 1) {
        int res;

        res = rapp_pixel_get_u8(buf, dim, x, 0);
        if (res < 0) {
            DBG("Function exited with an error\n");
            ok = false;
            goto Done;
        }

        if ((uint8_t)res != val) {
            DBG("Got %d, expected %d\n", res, val);
            ok = false;
            goto Done;
        }
    }

Done:
    rapp_free(buf);

    return ok;
}


bool
rapp_test_pixel_set_bin(void)
{
    uint8_t *buf;
    bool val;
    int dim;
    int x;
    bool ok = true;

    /* Allocate image */
    dim = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    buf = rapp_malloc(dim, 0);

    /* Set pixel in image */
    for (x = 0, val = false; x < RAPP_TEST_WIDTH; x++, val = !val) {
        int res;

        res = rapp_pixel_set_bin(buf, dim, 0, x, 0, (int)val);
        if (res != 0) {
            DBG("Function exited with error: %d\n", res);
            ok = false;
            goto Done;
        }
    }

    /* Verify */
    for (x = 0, val = false; x < RAPP_TEST_WIDTH; x++, val = !val) {
        int res;

        res = rapp_pixel_get_bin(buf, dim, 0, x, 0);
        if (res < 0) {
            DBG("Function exited with error: %d\n", res);
            ok = false;
            goto Done;
        }

        if (res != (int)val) {
            DBG("Got %d, expected %d\n", res, val);
            ok = false;
            goto Done;
        }
    }

Done:
    rapp_free(buf);

    return ok;
}
