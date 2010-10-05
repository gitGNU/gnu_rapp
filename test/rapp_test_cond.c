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
 *  @file   rapp_test_cond.c
 *  @brief  Correctness tests for conditional operations.
 */

#include <string.h>         /* memcpy()      */
#include "rapp.h"           /* RAPP API      */
#include "rapp_ref_cond.h"  /* Reference API */
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
#define RAPP_TEST_WIDTH  1024

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 16


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_cond_driver(int (*test)(), void (*ref)());

static bool
rapp_test_cond_driver2(int (*test)(), void (*ref)());


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_cond_set_u8(void)
{
    return rapp_test_cond_driver(&rapp_cond_set_u8,
                                 &rapp_ref_cond_set_u8);
}

bool
rapp_test_cond_copy_u8(void)
{
    return rapp_test_cond_driver2(&rapp_cond_copy_u8,
                                  &rapp_ref_cond_copy_u8);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_cond_driver(int (*test)(), void (*ref)())
{
    int      dst_dim  = rapp_align(RAPP_TEST_WIDTH);
    int      map_dim  = rapp_align((RAPP_TEST_WIDTH + 7)/8);
    uint8_t *dst_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *map_buf  = rapp_malloc(map_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    bool     pass     = false;
    int      k;

    /* Initialize the map image */
    rapp_test_init_mask(map_buf, map_dim*RAPP_TEST_HEIGHT, 64);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int value  = rapp_test_rand(0, 0xff);

        /* Test the full image at least once */
        if (k == 0) {
            width  = RAPP_TEST_WIDTH;
            height = RAPP_TEST_HEIGHT;
        }

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* map == dst */
            (*test)(dst_buf, dst_dim, dst_buf, map_dim,
                    width, height, value) != RAPP_ERR_OVERLAP
            /* map = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align(width) - rapp_alignment,
                       map_dim, width, height, value) != RAPP_ERR_OVERLAP
            /* map = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (map_dim*(height - 1) +
                                  rapp_align((width + 7) / 8) -
                                  rapp_alignment),
                       map_dim, width, height, value) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the destination pixel buffers with 0x55 markers */
        memset(dst_buf, 0x55, RAPP_TEST_HEIGHT*dst_dim);
        memcpy(ref_buf, dst_buf, RAPP_TEST_HEIGHT*dst_dim);

        /* Call the RAPP function */
        if ((*test)(dst_buf, dst_dim, map_buf, map_dim,
                          width, height, value) < 0)
        {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, dst_dim, map_buf, map_dim, width, height, value);

        /* Check the result */
        if (!rapp_test_compare_u8(dst_buf, dst_dim,
                                  ref_buf, dst_dim,
                                  width, height))
        {
            DBG("Invalid result\n");
            DBG("width=%d height=%d\n", width, height);
            DBG("map=\n");
            rapp_test_dump_bin(map_buf, map_dim, 0, width, height);
            DBG("dst=\n");
            rapp_test_dump_u8(dst_buf, dst_dim, width, height);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf, dst_dim, width, height);
            goto Done;
        }
    }

    pass = true;

Done:
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    rapp_free(map_buf);

    return pass;
}

static bool
rapp_test_cond_driver2(int (*test)(), void (*ref)())
{
    int      src_dim  = rapp_align(RAPP_TEST_WIDTH);
    int      dst_dim  = src_dim + rapp_alignment;
    int      map_dim  = rapp_align((RAPP_TEST_WIDTH + 7)/8);
    uint8_t *dst_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *src_buf  = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *map_buf  = rapp_malloc(map_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    bool     pass     = false;
    int      k;

    /* Initialize the map image */
    rapp_test_init_mask(map_buf, map_dim*RAPP_TEST_HEIGHT, 16);

    /* Initialize the src image */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, false);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);

        /* Test the full image at least once */
        if (k == 0) {
            width  = RAPP_TEST_WIDTH;
            height = RAPP_TEST_HEIGHT;
        }

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* map == dst */
            (*test)(dst_buf, dst_dim, src_buf, src_dim, dst_buf, map_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* map = far end of dst_buf */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align(width) - rapp_alignment,
                       map_dim, width, height) != RAPP_ERR_OVERLAP
            /* map = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf - (map_dim*(height - 1) +
                                  rapp_align((width + 7) / 8) -
                                  rapp_alignment),
                       map_dim, width, height) != RAPP_ERR_OVERLAP
            /* src == dst */
            || (*test)(dst_buf, dst_dim, dst_buf, src_dim,
                       map_buf, map_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align(width) - rapp_alignment, src_dim, map_buf,
                       map_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                                  rapp_align(width) - rapp_alignment),
                       src_dim, map_buf, map_dim,
                       width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the destination pixel buffers with 0x55 markers */
        memset(dst_buf, 0x55, RAPP_TEST_HEIGHT*dst_dim);
        memcpy(ref_buf, dst_buf, RAPP_TEST_HEIGHT*dst_dim);

        /* Call the RAPP function */
        if ((*test)(dst_buf, dst_dim, src_buf, src_dim,
                    map_buf, map_dim, width, height) < 0)
        {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim,
               map_buf, map_dim, width, height);

        /* Check the result */
        if (!rapp_test_compare_u8(dst_buf, dst_dim,
                                  ref_buf, dst_dim,
                                  width, height))
        {
            DBG("Invalid result\n");
            DBG("width=%d height=%d\n", width, height);
            DBG("map=\n");
            rapp_test_dump_bin(map_buf, map_dim, 0, width, height);
            DBG("dst=\n");
            rapp_test_dump_u8(dst_buf, dst_dim, width, height);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf, dst_dim, width, height);
            goto Done;
        }
    }

    pass = true;

Done:
    rapp_free(dst_buf);
    rapp_free(src_buf);
    rapp_free(ref_buf);
    rapp_free(map_buf);

    return pass;
}
