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
 *  @file   rapp_test_scatter_bin.c
 *  @brief  Correctness tests for binary scatter.
 */

#include <string.h>               /* memcpy()      */
#include "rapp.h"                 /* RAPP API      */
#include "rapp_ref_scatter_bin.h" /* Reference API */
#include "rapp_test_util.h"       /* Test utils    */


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
rapp_test_scatter_driver(int (*test)(), int (*ref)());

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_scatter_bin(void)
{
    return rapp_test_scatter_driver(&rapp_scatter_bin,
                                    &rapp_ref_scatter_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_scatter_driver(int (*test)(), int (*ref)())
{
    int      dst_dim  = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    int      map_dim  = dst_dim + rapp_alignment;
    int      pack_len = (RAPP_TEST_WIDTH*RAPP_TEST_HEIGHT + 7) / 8;
    uint8_t *dst_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *map_buf  = rapp_malloc(map_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *pack_buf = rapp_malloc(pack_len, 0);
    bool     pass     = false;
    int      k;

    /* Initialize the packed pixel buffer */
    rapp_test_init(pack_buf, 0, pack_len, 1, true);

    /* Initialize the map image */
    rapp_test_init_mask(map_buf, map_dim*RAPP_TEST_HEIGHT, 64);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int rlen;
        int tlen;

        /* Test the full image at least once */
        if (k == 0) {
            width  = RAPP_TEST_WIDTH;
            height = RAPP_TEST_HEIGHT;
        }

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* pack == dst */
            (*test)(dst_buf, dst_dim, map_buf, map_dim,
                    dst_buf, width, height) != RAPP_ERR_OVERLAP
            /* pack = far end of dst_buf */
            || (*test)(dst_buf, dst_dim, map_buf, map_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7) / 8) - rapp_alignment,
                       width, height) != RAPP_ERR_OVERLAP
            /* map == dst */
            || (*test)(dst_buf, dst_dim, dst_buf, map_dim,
                       pack_buf, width, height) != RAPP_ERR_OVERLAP
            /* map = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7) / 8) - rapp_alignment,
                       map_dim, pack_buf, width, height) != RAPP_ERR_OVERLAP
            /* map = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (map_dim*(height - 1) +
                                  rapp_align((width + 7) / 8) -
                                  rapp_alignment),
                       map_dim, pack_buf, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the destination pixel buffers with 0x55 markers */
        memset(dst_buf, 0x55, RAPP_TEST_HEIGHT*dst_dim);
        memcpy(ref_buf, dst_buf, RAPP_TEST_HEIGHT*dst_dim);

        /* Call the RAPP function */
        tlen = (*test)(dst_buf, dst_dim, map_buf, map_dim,
                       pack_buf, width, height);
        if (tlen < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        rlen = (*ref)(ref_buf, dst_dim, map_buf, map_dim,
                      pack_buf, width, height);

        /* Check the number of scattered pixels */
        if (tlen != rlen) {
            DBG("Invalid number of pixels %d, should be %d\n", tlen, rlen);
            goto Done;
        }

        /* Check the result */
        if (memcmp(ref_buf, dst_buf, dst_dim*RAPP_TEST_HEIGHT) != 0) {
            DBG("Invalid result\n");
            DBG("map=\n");
            rapp_test_dump_bin(map_buf, map_dim, 0, width, height);
            DBG("pack=\n");
            rapp_test_dump_bin(pack_buf, 0, 0, rlen, 1);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);
            goto Done;
        }
    }

    pass = true;

Done:
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    rapp_free(map_buf);
    rapp_free(pack_buf);

    return pass;
}
