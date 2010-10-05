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
 *  @file   rapp_test_stat.c
 *  @brief  Correctness tests for statistics operations.
 */

#include <string.h>          /* memcpy()      */
#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_stat.h"   /* Reference API */
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
#define RAPP_TEST_WIDTH  256

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 128

/**
 *  The result buffer magic number.
 */
#define RAPP_TEST_MAGIC ((uintmax_t)0xcafebabeUL << \
                         (8*sizeof(uintmax_t) - 32))


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_stat_driver(int32_t (*test)(), int32_t (*ref)(),
                      int border, bool binary);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_stat_sum_bin(void)
{
    return rapp_test_stat_driver(&rapp_stat_sum_bin,
                                 &rapp_ref_stat_sum_bin, 0, true);
}

bool
rapp_test_stat_sum_u8(void)
{
    return rapp_test_stat_driver(&rapp_stat_sum_u8,
                                 &rapp_ref_stat_sum_u8, 0, false);
}

bool
rapp_test_stat_sum2_u8(void)
{
    return rapp_test_stat_driver(&rapp_stat_sum2_u8,
                                 (int32_t (*)())&rapp_ref_stat_sum2_u8,
                                 0, false);
}

bool
rapp_test_stat_xsum_u8(void)
{
    int      dim1 = rapp_align(RAPP_TEST_WIDTH);
    int      dim2 = dim1 + rapp_alignment;
    uint8_t *buf1 = rapp_malloc(dim1*RAPP_TEST_HEIGHT, 0);
    uint8_t *buf2 = rapp_malloc(dim2*RAPP_TEST_HEIGHT, 0);
    int      iter;

    /* Perform tests */
    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int       width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int       height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        uintmax_t sum[5];
        uintmax_t ref[5];

        /* Initialize the buffers */
        memset(buf1, 0, dim1*height);
        memset(buf2, 0, dim2*height);
        rapp_test_init(buf1, dim1, width, height, true);
        rapp_test_init(buf2, dim2, width, height, true);

        /* Call the statistical function */
        if (rapp_stat_xsum_u8(buf1, dim1, buf2, dim2,
                              width, height, sum) < 0)
        {
            DBG("Got FAIL return value\n");
            return false;
        }

        /* Call the reference function */
        rapp_ref_stat_xsum_u8(buf1, dim1, buf2, dim2, width, height, ref);

        /* Compare the results */
        if (memcmp(ref, sum, sizeof sum) != 0) {
            DBG("Failed\n");
            return false;
        }
    }

    /* Release buffers */
    rapp_free(buf1);
    rapp_free(buf2);

    return true;
}

bool
rapp_test_stat_min_bin(void)
{
    return rapp_test_stat_driver(&rapp_stat_min_bin,
                                 (int32_t (*)())&rapp_ref_stat_min_bin,
                                 1, true);
}

bool
rapp_test_stat_max_bin(void)
{
    return rapp_test_stat_driver(&rapp_stat_max_bin,
                                 (int32_t (*)())&rapp_ref_stat_max_bin,
                                 0, true);
}

bool
rapp_test_stat_min_u8(void)
{
    return rapp_test_stat_driver(&rapp_stat_min_u8,
                                 (int32_t (*)())&rapp_ref_stat_min_u8,
                                 1, false);
}

bool
rapp_test_stat_max_u8(void)
{
    return rapp_test_stat_driver(&rapp_stat_max_u8,
                                 (int32_t (*)())&rapp_ref_stat_max_u8,
                                 0, false);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_stat_driver(int32_t (*test)(), int32_t (*ref)(),
                      int border, bool binary)
{
    int      dim = rapp_align(RAPP_TEST_WIDTH);
    uint8_t *buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    int      iter;

    /* Perform tests */
    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int       width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int       height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int       len    = binary ? (width + 7)/8 : width;
        union { uintmax_t uim[5]; uint8_t u8[5 * sizeof (uintmax_t)]; } tsum;
        union { uintmax_t uim[5]; uint8_t u8[5 * sizeof (uintmax_t)]; } rsum;
        int32_t   t, r;
        unsigned int i;

        /* Initialize the buffer */
        rapp_test_init(buf, dim, len, height, true);
        if (binary) {
            rapp_pad_align_bin(buf, dim, 0, width, height, border != 0);
        }
        else {
            rapp_pad_align_u8(buf, dim, width, height, border ? 0xff : 0);
        }

        /* Initialize the result buffers */
        for (i = 0; i < sizeof tsum.uim / sizeof tsum.uim[0]; i++) {
            tsum.uim[i] = RAPP_TEST_MAGIC;
            rsum.uim[i] = RAPP_TEST_MAGIC;
        }

        /* Call the statistical function */
        t = (*test)(buf, dim, width, height, tsum.uim);
        if (t < 0) {
            DBG("Got FAIL return value %d\n", t);
            return false;
        }

        /* Call the reference function */
        r = (*ref)(buf, dim, width, height, rsum.uim);

        /* Compare the return values if sum[0] is unused */
        if (tsum.uim[0] == RAPP_TEST_MAGIC && t != r) {
            DBG("Failed; return value is %d (expected %d)\nsrc=\n", t, r);
            goto dump_failed_result;
        }

        /* Compare the results */
        if (memcmp(tsum.uim, rsum.uim, sizeof tsum) != 0) {
            DBG("Failed; result differs from the expected\nsrc=\n");

        dump_failed_result:
            rapp_test_dump_u8(buf, dim, width, height);
            DBG("dst=\n");
            rapp_test_dump_u8(tsum.u8, sizeof tsum, sizeof tsum, 1);
            DBG("ref=\n");
            rapp_test_dump_u8(rsum.u8, sizeof rsum, sizeof rsum, 1);
            return false;
        }
    }

    /* Release buffer */
    rapp_free(buf);

    return true;
}
