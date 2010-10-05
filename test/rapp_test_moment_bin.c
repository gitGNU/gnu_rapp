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
 *  @file   rapp_test_moment_bin.c
 *  @brief  Correctness tests for binary image moments.
 */

#include <string.h>              /* memcpy()      */
#include <assert.h>              /* assert()      */
#include "rapp.h"                /* RAPP API      */
#include "rapp_ref_moment_bin.h" /* Reference API */
#include "rapp_test_util.h"      /* Test utils    */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER   1024

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH  512

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 128


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_moment_driver(int (*test)(), void (*ref)());


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_moment_order1_bin(void)
{
    return rapp_test_moment_driver(&rapp_moment_order1_bin,
                                   &rapp_ref_moment_order1_bin);
}

bool
rapp_test_moment_order2_bin(void)
{
    return rapp_test_moment_driver(&rapp_moment_order2_bin,
                                   &rapp_ref_moment_order2_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_moment_driver(int (*test)(), void (*ref)())
{
    int      dim = rapp_align(RAPP_TEST_WIDTH);
    uint8_t *buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    int      iter;

    /* Perform tests */
    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int       width   = rapp_test_rand(8, RAPP_TEST_WIDTH) & ~7;
        int       height  = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int       len     = width / 8;
        uintmax_t tmom[6] = {0};
        uintmax_t rmom[6] = {0};

        assert(width % 8 == 0);

        /* Initialize the buffer */
        memset(buf, 0, dim*height);
        if (height > 1) {
            memset(&buf[dim], 0xff, len);
        }
        if (height > 2) {
            rapp_test_init(&buf[2*dim], dim, len, height - 2, true);
        }

        /* Call the statistical function */
        if ((*test)(buf, dim, width, height, tmom) < 0) {
            DBG("Got FAIL return value\n");
            return false;
        }

        /* Call the reference function */
        (*ref)(buf, dim, width, height, rmom);

        /* Compare the results */
        if (memcmp(tmom, rmom, sizeof tmom) != 0) {
            DBG("Failed\n");
            return false;
        }
    }

    /* Release buffer */
    rapp_free(buf);

    return true;
}
