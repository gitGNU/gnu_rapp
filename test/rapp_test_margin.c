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
 *  @file   rapp_test_margin.c
 *  @brief  Correctness tests for binary image logical margins.
 */

#include <string.h>           /* memcpy()       */
#include "rapp.h"             /* RAPP API       */
#include "rapp_ref_margin.h"  /* Reference API  */
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
#define RAPP_TEST_WIDTH  256

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 256


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(), bool horz);


/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_margin_horz_bin(void)
{
    return rapp_test_driver(&rapp_margin_horz_bin,
                            &rapp_ref_margin_horz_bin,
                            true);
}

bool
rapp_test_margin_vert_bin(void)
{
    return rapp_test_driver(&rapp_margin_vert_bin,
                            &rapp_ref_margin_vert_bin,
                            false);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(), bool horz)
{
    int      src_dim = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    int      dst_dim = rapp_align((MAX(RAPP_TEST_WIDTH,
                                       RAPP_TEST_HEIGHT) + 7) / 8);
    uint8_t *src_buf = rapp_malloc(RAPP_TEST_HEIGHT*src_dim, 0);
    uint8_t *dst_buf = rapp_malloc(dst_dim, 0);
    uint8_t *ref_buf = rapp_malloc(dst_dim, 0);
    int      iter;
    bool     ok = false;

    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int fill   = rapp_test_rand(0, 100);
        int x, y;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*func)(dst_buf, dst_buf, src_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*func)(dst_buf,
                       dst_buf + rapp_align(((horz ? width : height) + 7) / 8) -
                       rapp_alignment,
                       src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*func)(dst_buf,
                       dst_buf - (rapp_align((width + 7) / 8) -
                                  rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Clear all buffers */
        memset(src_buf, 0, height*src_dim);
        memset(dst_buf, 0, dst_dim);
        memset(ref_buf, 0, dst_dim);

        /* Set pixel values according to the fill rate */
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                int pix = rapp_test_rand(1, 100) <= fill;
                rapp_pixel_set_bin(src_buf, src_dim, 0, x, y, pix);
            }
        }

        /* Call the margin function */
        if ((*func)(dst_buf, src_buf, src_dim, width, height) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, src_buf, src_dim, width, height);

        /* Check the result */
        if (!rapp_test_compare_bin(ref_buf, 0, dst_buf, 0, 0,
                                   horz ? width : height, 1))
        {
            int size = horz ? width : height;
            DBG("Invalid result\n");
            DBG("src=\n");
            rapp_test_dump_bin(src_buf, src_dim, 0, width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, 0, 0, size, 1);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, 0, 0, size, 1);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(src_buf);
    rapp_free(dst_buf);
    rapp_free(ref_buf);

    return ok;
}
