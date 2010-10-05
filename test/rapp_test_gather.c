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
 *  @file   rapp_test_gather.c
 *  @brief  Correctness tests for 8-bit gather.
 */

#include <string.h>          /* memcpy()      */
#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_gather.h" /* Reference API */
#include "rapp_test_util.h"  /* Test utils    */


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

/**
 *  Maximum number of rows to gather.
 */
#define RAPP_TEST_ROWS 7


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_gather_driver(int (*test)(), int (*ref)());

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_gather_u8(void)
{
    return rapp_test_gather_driver(&rapp_gather_u8,
                                   &rapp_ref_gather_u8);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_gather_driver(int (*test)(), int (*ref)())
{
    int      src_dim  = rapp_align(RAPP_TEST_WIDTH);
    int      map_dim  = rapp_align((RAPP_TEST_WIDTH + 7) / 8);
    int      pack_dim = rapp_align(RAPP_TEST_WIDTH*RAPP_TEST_HEIGHT);
    uint8_t *src_buf  = rapp_malloc(src_dim*(RAPP_TEST_HEIGHT +
                                             RAPP_TEST_ROWS), 0);
    uint8_t *map_buf  = rapp_malloc(map_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf  = rapp_malloc(pack_dim*RAPP_TEST_ROWS, 0);
    uint8_t *pack_buf = rapp_malloc(pack_dim*RAPP_TEST_ROWS, 0);
    bool     pass     = false;
    int      k;

    /* Initialize the source pixel buffer */
    rapp_test_init(src_buf, 0, src_dim*(RAPP_TEST_HEIGHT +
                                        RAPP_TEST_ROWS), 1, false);
    /* Initialize the map image */
    rapp_test_init_mask(map_buf, map_dim*RAPP_TEST_HEIGHT, 64);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int rows   = rapp_test_rand(1, RAPP_TEST_ROWS);
        int rlen;
        int tlen;

        /* Test the full image at least once */
        if (k == 0) {
            width  = RAPP_TEST_WIDTH;
            height = RAPP_TEST_HEIGHT;
            rows   = RAPP_TEST_ROWS;
        }

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* pack == src */
            (*test)(src_buf, pack_dim, src_buf, src_dim,
                    map_buf, map_dim, width, height, rows) != RAPP_ERR_OVERLAP
            /* pack = far end of src_buf */
            || (*test)(src_buf + src_dim*(height - 1) +
                       rapp_align(width) - rapp_alignment,
                       pack_dim, src_buf, src_dim, map_buf, map_dim,
                       width, height, rows) != RAPP_ERR_OVERLAP
            /* pack == map */
            || (*test)(map_buf, pack_dim, src_buf, src_dim, map_buf, map_dim,
                       width, height, rows) != RAPP_ERR_OVERLAP
            /* pack = far end of map_buf */
            || (*test)(map_buf + map_dim*(height - 1) +
                       rapp_align((width + 7) / 8) - rapp_alignment,
                       pack_dim, src_buf, src_dim, map_buf, map_dim,
                       width, height, rows) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Clear the pack buffers */
        memset(pack_buf, 0, rows*pack_dim);
        memset(ref_buf,  0, rows*pack_dim);

        /* Call the RAPP function */
        tlen = (*test)(pack_buf, pack_dim, src_buf, src_dim,
                       map_buf, map_dim, width, height, rows);
        if (tlen < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        rlen = (*ref)(ref_buf, pack_dim, src_buf, src_dim,
                      map_buf, map_dim, width, height, rows);

        /* Check the number of gathered pixels */
        if (tlen != rlen) {
            DBG("Invalid number of pixels %d, should be %d\n", tlen, rlen);
            goto Done;
        }

        /* Check the result */
        if (memcmp(ref_buf, pack_buf, rows*pack_dim) != 0) {
            DBG("Invalid result\n");
            DBG("width=%d height=%d\n", width, height);
            DBG("map=\n");
            rapp_test_dump_bin(map_buf, map_dim, 0, width, height);
            DBG("src=\n");
            rapp_test_dump_u8(src_buf, src_dim, width, height);
            DBG("pack=\n");
            rapp_test_dump_u8(pack_buf, pack_dim, rlen, rows);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf,  pack_dim, rlen, rows);
            goto Done;
        }
    }

    pass = true;

Done:
    rapp_free(src_buf);
    rapp_free(map_buf);
    rapp_free(ref_buf);
    rapp_free(pack_buf);

    return pass;
}
