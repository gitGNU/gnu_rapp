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
 *  @file   rapp_test_type.c
 *  @brief  Correctness tests for type conversions.
 */

#include <string.h>         /* memcpy()      */
#include "rapp.h"           /* RAPP API      */
#include "rapp_ref_type.h"  /* Reference API */
#include "rapp_test_util.h" /* Test utils    */


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
#define RAPP_TEST_WIDTH  256

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
rapp_test_type_driver(int (*test)(), void (*ref)(), bool binary);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_type_u8_to_bin(void)
{
    return rapp_test_type_driver(&rapp_type_u8_to_bin,
                                 &rapp_ref_type_u8_to_bin, true);
}

bool
rapp_test_type_bin_to_u8(void)
{
    return rapp_test_type_driver(&rapp_type_bin_to_u8,
                                 &rapp_ref_type_bin_to_u8, false);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_type_driver(int (*test)(), void (*ref)(), bool binary)
{
    int      dst_dim = rapp_align(RAPP_TEST_WIDTH);
    int      src_dim = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    uint8_t *dst_buf = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *src_buf = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    int      k;
    bool     ok = false;

    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, false);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*test)(dst_buf, dst_dim, dst_buf, src_dim, width,
                    height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align(binary ? (width + 7) / 8 : width) -
                       rapp_alignment,
                       src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                                  rapp_align(binary ? width : (width + 7) / 8) -
                                  rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Call RAPP function */
        if ((*test)(dst_buf, dst_dim, src_buf, src_dim, width, height) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call reference function */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim, width, height);

        /* Check result */
        if (( binary && !rapp_test_compare_bin(dst_buf, dst_dim, ref_buf,
                                              dst_dim, 0, width, height)) ||
            (!binary && !rapp_test_compare_u8(dst_buf, dst_dim, ref_buf,
                                              dst_dim, width, height)))
        {
            DBG("Invalid result\n");
            DBG("width=%d height=%d\n", width, height);
            DBG("src=\n");
            rapp_test_dump_bin(src_buf, src_dim, 0, width, height);
            DBG("dst=\n");
            rapp_test_dump_u8(dst_buf, dst_dim, width, height);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf, dst_dim, width, height);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(dst_buf);
    rapp_free(src_buf);
    rapp_free(ref_buf);

    return ok;
}
