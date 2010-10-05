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
 *  @file   rapp_test_reduce_bin.c
 *  @brief  Correctness tests for binary 2x reduction.
 */

#include <string.h>              /* memcpy()       */
#include "rapp.h"                /* RAPP API       */
#include "rapp_ref_reduce_bin.h" /* Reference API  */
#include "rapp_test_util.h"      /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER  1024

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
rapp_test_driver(int (*func)(), void (*ref)(), int xscale, int yscale);


/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_reduce_1x2_rk1_bin(void)
{
    return rapp_test_driver(&rapp_reduce_1x2_rk1_bin,
                            &rapp_ref_reduce_1x2_rk1_bin,
                            2, 1);
}

bool
rapp_test_reduce_1x2_rk2_bin(void)
{
    return rapp_test_driver(&rapp_reduce_1x2_rk2_bin,
                            &rapp_ref_reduce_1x2_rk2_bin,
                            2, 1);
}

bool
rapp_test_reduce_2x1_rk1_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x1_rk1_bin,
                            &rapp_ref_reduce_2x1_rk1_bin,
                            1, 2);
}

bool
rapp_test_reduce_2x1_rk2_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x1_rk2_bin,
                            &rapp_ref_reduce_2x1_rk2_bin,
                            1, 2);
}

bool
rapp_test_reduce_2x2_rk1_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x2_rk1_bin,
                            &rapp_ref_reduce_2x2_rk1_bin,
                            2, 2);
}

bool
rapp_test_reduce_2x2_rk2_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x2_rk2_bin,
                            &rapp_ref_reduce_2x2_rk2_bin,
                            2, 2);
}

bool
rapp_test_reduce_2x2_rk3_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x2_rk3_bin,
                            &rapp_ref_reduce_2x2_rk3_bin,
                            2, 2);
}

bool
rapp_test_reduce_2x2_rk4_bin(void)
{
    return rapp_test_driver(&rapp_reduce_2x2_rk4_bin,
                            &rapp_ref_reduce_2x2_rk4_bin,
                            2, 2);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(), int xscale, int yscale)
{
    int      src_dim = rapp_align((RAPP_TEST_WIDTH + 7)/8) + rapp_alignment;
    int      dst_dim = rapp_align((RAPP_TEST_WIDTH/xscale + 7)/8);

    uint8_t *src_buf = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *dst_buf = rapp_malloc(dst_dim*(RAPP_TEST_HEIGHT / yscale), 0);
    uint8_t *ref_buf = rapp_malloc(dst_dim*(RAPP_TEST_HEIGHT / yscale), 0);
    int      k;
    bool    ok = false;

    /* Initialize the source buffer */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, true);

    /* Run random tests */
    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(2, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(2, RAPP_TEST_HEIGHT);

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*func)(dst_buf, dst_dim, dst_buf, src_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*func)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height / yscale - 1) +
                       rapp_align((width / xscale + 7) / 8) - rapp_alignment,
                       src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*func)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                                  rapp_align((width + 7) / 8) -
                                  rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Call the reduction function */
        if ((*func)(dst_buf, dst_dim, src_buf, src_dim,
                    width, height) < 0)
        {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim, width, height);

        /* Compare the results */
        if (!rapp_test_compare_bin(ref_buf, dst_dim,
                                   dst_buf, dst_dim,
                                   0, width/xscale, height/yscale))
        {
            DBG("Failed\n");
            DBG("src=\n");
            rapp_test_dump_bin(src_buf, src_dim, 0, width, height);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, dst_dim, 0,
                               width/xscale, height/yscale);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, dst_dim, 0,
                               width/xscale, height/yscale);
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
