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
 *  @file   rapp_test_crop.c
 *  @brief  Correctness tests for binary image cropping.
 */

#include <string.h>           /* memcpy()       */
#include "rapp.h"             /* RAPP API       */
#include "rapp_ref_crop.h"    /* Reference API  */
#include "rapp_test_util.h"   /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER  128

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH  1024

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 8


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_point_driver(int (*func)());

static bool
rapp_test_rand_driver(int (*func)(), int (*ref)());


/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_crop_seek_bin(void)
{
    return rapp_test_point_driver(&rapp_crop_seek_bin) &&
           rapp_test_rand_driver(&rapp_crop_seek_bin,
                                 &rapp_ref_crop_seek_bin);
}

bool
rapp_test_crop_box_bin(void)
{
     return rapp_test_point_driver(&rapp_crop_box_bin) &&
            rapp_test_rand_driver(&rapp_crop_box_bin,
                                  &rapp_ref_crop_box_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_point_driver(int (*func)())
{
    int      dim = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    uint8_t *src = rapp_malloc(RAPP_TEST_HEIGHT*dim, 0);
    int      iter;
    bool     ok = false;

    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        unsigned box[4]  = {0, 0, 1, 1};
        int      width   = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int      height  = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int      xpos    = rapp_test_rand(0, width - 1);
        int      ypos    = rapp_test_rand(0, height - 1);
        int      val     = rapp_test_rand(0, 1);
        int      ret;

        /* Initialize the buffer */
        memset(src, 0, height*dim);
        rapp_pixel_set_bin(src, dim, 0, xpos, ypos, val);

        /* Call the test function */
        ret = (*func)(src, dim, width, height, box);

        /* Check the return value */
        if (ret < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }
        else if ((val == 1) != (ret > 0)) {
            DBG("Invalid return value %d\n", ret);
            goto Done;
        }

        /* Check the result */
        if (val == 1 &&
            (box[0] != (unsigned)xpos || box[1] != (unsigned)ypos ||
             box[2] != 1              || box[3] != 1))
        {
            DBG("Invalid result: (%d, %d, %d, %d)\n",
                box[0], box[1], box[2], box[3]);
            DBG("xpos=%d ypos=%d\n", xpos, ypos);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(src);
    return ok;
}

static bool
rapp_test_rand_driver(int (*func)(), int (*ref)())
{
    int      dim = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    uint8_t *src = rapp_malloc(RAPP_TEST_HEIGHT*dim, 0);
    int      iter;
    bool     ok = false;

    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        unsigned rbox[4] = {0};
        unsigned tbox[4] = {0};
        int      width   = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int      height  = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int      fill    = rapp_test_rand(0, 100);
        int      x, y;

        /* Clear buffer */
        memset(src, 0, height*dim);

        /* Set pixel values according to the fill rate */
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                int pix = rapp_test_rand(1, 100) <= fill;
                rapp_pixel_set_bin(src, dim, 0, x, y, pix);
            }
        }

        /* Call the test function */
        if ((*func)(src, dim, width, height, tbox) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(src, dim, width, height, rbox);

        /* Check the result */
        if (memcmp(tbox, rbox, sizeof tbox) != 0) {
            DBG("Invalid result\n");
            DBG("Got (%d, %d, %d, %d), should be (%d, %d, %d, %d)\n",
                tbox[0], tbox[1], tbox[2], tbox[3],
                rbox[0], rbox[1], rbox[2], rbox[3]);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(src);
    return ok;
}
