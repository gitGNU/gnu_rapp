/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_thresh.c
 *  @brief  Correctness tests thresholding operations.
 */

#include <string.h>          /* memcpy()      */
#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_thresh.h" /* Reference API */
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


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_thresh_driver(int (*test)(), void (*ref)());

static bool
rapp_test_thresh_pixel_driver(int (*test)(), void (*ref)());

static bool
rapp_test_thresh_pixel_driver2(int (*test)(), void (*ref)());

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_thresh_gt_u8(void)
{
    return rapp_test_thresh_driver(&rapp_thresh_gt_u8,
                                   &rapp_ref_thresh_gt_u8);
}

bool
rapp_test_thresh_lt_u8(void)
{
    return rapp_test_thresh_driver(&rapp_thresh_lt_u8,
                                   &rapp_ref_thresh_lt_u8);
}

bool
rapp_test_thresh_gtlt_u8(void)
{
    return rapp_test_thresh_driver(&rapp_thresh_gtlt_u8,
                                   &rapp_ref_thresh_gtlt_u8);
}

bool
rapp_test_thresh_ltgt_u8(void)
{
    return rapp_test_thresh_driver(&rapp_thresh_ltgt_u8,
                                   &rapp_ref_thresh_ltgt_u8);
}

bool
rapp_test_thresh_gt_pixel_u8(void)
{
    return rapp_test_thresh_pixel_driver(&rapp_thresh_gt_pixel_u8,
                                         &rapp_ref_thresh_gt_pixel_u8);
}

bool
rapp_test_thresh_lt_pixel_u8(void)
{
    return rapp_test_thresh_pixel_driver(&rapp_thresh_lt_pixel_u8,
                                         &rapp_ref_thresh_lt_pixel_u8);
}

bool
rapp_test_thresh_gtlt_pixel_u8(void)
{
    return rapp_test_thresh_pixel_driver2(&rapp_thresh_gtlt_pixel_u8,
                                          &rapp_ref_thresh_gtlt_pixel_u8);
}

bool
rapp_test_thresh_ltgt_pixel_u8(void)
{
    return rapp_test_thresh_pixel_driver2(&rapp_thresh_ltgt_pixel_u8,
                                          &rapp_ref_thresh_ltgt_pixel_u8);
}

/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_thresh_driver(int (*test)(), void (*ref)())
{
    /* Special boundary cases to test explicitly */
    static const int special[][2] = {{0,   0xfe }, {-1,  0xfe }, {1,   0xfe },
                                     {0,   0xff }, {-1,  0xff }, {1,   0xff },
                                     {0,   0x100}, {-1,  0x100}, {1,   0x100},
                                     {0xfe,    0}, {0xfe,   -1}, {0xfe,    1},
                                     {0xff,    0}, {0xff,   -1}, {0xff,    1},
                                     {0x100,   0}, {0x100,  -1}, {0x100,   1},
                                     {0x80, 0x80}, {0x81, 0x80}, {0x80, 0x81}};

    int      dst_dim = rapp_align(RAPP_TEST_WIDTH);
    int      src_dim = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    uint8_t *dst_buf = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *src_buf = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    int      k;
    bool     ok = false;
    int width, height;
    int low, high;

    /* Initialize the source buffer */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, false);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        height = rapp_test_rand(1, RAPP_TEST_HEIGHT);

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*test)(dst_buf, dst_dim, dst_buf, src_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment,
                       src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                                  rapp_align((width + 7)/8) - rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        if (k < (int)(sizeof special / sizeof special[0])) {
            /* Test special cases */
            low  = special[k][0];
            high = special[k][1];
        }
        else {
            /* Test random cases */
            low  = rapp_test_rand(-8, 0xff + 8);
            high = rapp_test_rand(-8, 0xff + 8);
        }

        /* Call RAPP function */
        if ((*test)(dst_buf, dst_dim, src_buf,
                    src_dim, width, height, low, high) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call reference function */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim, width, height, low, high);

        /* Check result */
        if (!rapp_test_compare_bin(dst_buf, dst_dim, ref_buf,
                                   dst_dim, 0, width, height))
        {
            DBG("Invalid result\n");
            DBG("src=\n");
            rapp_test_dump_u8(src_buf, src_dim, width, height);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);

            goto Done;
        }
    }

    /**
     *  Test a source just *before* the destination. We don't bother
     *  testing different width and heights.
     */
    width = 8*rapp_alignment;
    height = 1;
    memcpy(dst_buf, src_buf, width);

    /**
     *  We happen to know that the threshold tests are performed before
     *  the thresholds are inspected for special cases, so we just keep
     *  the last (random) values for low and high.
     */
    if ((*test)(dst_buf + width, dst_dim, dst_buf,
                src_dim, width, height, low, high) < 0) {
        DBG("Got FAIL return value for src-before-dest overlap test\n");
        goto Done;
    }

    (*ref)(ref_buf, dst_dim, src_buf, src_dim, width, height, low, high);
    if (!rapp_test_compare_bin(dst_buf + width, dst_dim, ref_buf,
                               dst_dim, 0, width, height)) {
        DBG("Invalid result, src-before-dst overrlap test\n");
        DBG("src=\n");
        rapp_test_dump_u8(src_buf, src_dim, width, height);
        DBG("dst=\n");
        rapp_test_dump_bin(dst_buf + width, dst_dim, 0, width, height);
        DBG("ref=\n");
        rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);

        goto Done;
      }

    /* Test a source just *after* the destination. */
    memcpy(dst_buf + rapp_alignment, src_buf, width);
    if ((*test)(dst_buf, dst_dim, dst_buf + rapp_alignment,
                src_dim, width, height, low, high) < 0) {
        DBG("Got FAIL return value for src-after-dest overlap test\n");
        goto Done;
    }

    /* We don't have to re-generate the reference result here. */

    if (!rapp_test_compare_bin(dst_buf, dst_dim, ref_buf,
                               dst_dim, 0, width, height)) {
        DBG("Invalid result, src-after-dest overlap test\n");
        DBG("src=\n");
        rapp_test_dump_u8(src_buf, src_dim, width, height);
        DBG("dst=\n");
        rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
        DBG("ref=\n");
        rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);

        goto Done;
    }

    ok = true;

Done:
    rapp_free(dst_buf);
    rapp_free(src_buf);
    rapp_free(ref_buf);

    return ok;
}

static bool
rapp_test_thresh_pixel_driver(int (*test)(), void (*ref)())
{
    /* Special boundary cases to test explicitly. */
    static const uint8_t special[] = {0, 1, 0x80, 0x81, 0xfe, 0xff};

    int      dst_dim     = rapp_align(RAPP_TEST_WIDTH);
    int      src_dim     = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    int      thresh_dim  = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    uint8_t *dst_buf     = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *src_buf     = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *thresh_buf  = rapp_malloc(thresh_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf     = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    int      k;
    bool     ok = false;

    /* Initialize the source buffer. */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, false);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        uint8_t thresh;

        /* Verify that we get an overlap error for overlapping buffers. */
        if (/* src == dst */
            (*test)(dst_buf, dst_dim, dst_buf, src_dim,
                    thresh_buf, thresh_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment, src_dim,
                       thresh_buf, thresh_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough. */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment), src_dim,
                       thresh_buf, thresh_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Src/dst overlap undetected\n");
            goto Done;
        }

        if (/* thresh_buf == dst */
            (*test)(dst_buf, dst_dim, src_buf, src_dim,
                    dst_buf, thresh_dim, width, height) != RAPP_ERR_OVERLAP
            /* thresh_buf = far end of dst_buf */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment, thresh_dim,
                       width, height) != RAPP_ERR_OVERLAP
            /* thresh_buf = before dst, but not long enough. */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf - (thresh_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment), thresh_dim,
                       width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("thresh_buf/dst overlap undetected\n");
            goto Done;
        }

        if (k < (int)(sizeof special / sizeof special[0])) {
            /* Test special cases. */
            thresh  = special[k];
        }
        else {
            /* Test random cases. */
            thresh  = rapp_test_rand(0, 0xff);
        }

        memset(thresh_buf, thresh, thresh_dim*RAPP_TEST_HEIGHT);

        /* Call RAPP function. */
        if ((*test)(dst_buf, dst_dim, src_buf, src_dim,
                    thresh_buf, thresh_dim, width, height) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call reference function. */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim,
               thresh_buf, thresh_dim, width, height);

        /* Check result. */
        if (!rapp_test_compare_bin(dst_buf, dst_dim, ref_buf,
                                   dst_dim, 0, width, height))
        {
            DBG("Invalid result\n");
            DBG("src=\n");
            rapp_test_dump_u8(src_buf, src_dim, width, height);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);

            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(dst_buf);
    rapp_free(src_buf);
    rapp_free(thresh_buf);
    rapp_free(ref_buf);

    return ok;
}

static bool
rapp_test_thresh_pixel_driver2(int (*test)(), void (*ref)())
{
    /* Special boundary cases to test explicitly. */
    static const uint8_t special[][2] = {{0,   0xfe }, {1,   0xfe },
                                         {0,   0xff }, {1,   0xff },
                                         {0xfe,    0}, {0xfe,    1},
                                         {0xff,    0}, {0xff,    1},
                                         {0x80, 0x80}, {0x81, 0x80}, {0x80, 0x81}};

    int      dst_dim  = rapp_align(RAPP_TEST_WIDTH);
    int      src_dim  = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    int      low_dim  = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    int      high_dim = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    uint8_t *dst_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *src_buf  = rapp_malloc(src_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *low_buf  = rapp_malloc(low_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *high_buf = rapp_malloc(high_dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf  = rapp_malloc(dst_dim*RAPP_TEST_HEIGHT, 0);
    int      k;
    bool     ok = false;

    /* Initialize the source buffer. */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, false);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width  = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        uint8_t low, high;

        /* Verify that we get an overlap error for overlapping buffers. */
        if (/* src == dst */
            (*test)(dst_buf, dst_dim, dst_buf, src_dim,
                    width, height, low_buf, low_dim,
                    high_buf, high_dim) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment, src_dim,
                       width, height, low_buf, low_dim,
                       high_buf, high_dim) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough. */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment), src_dim,
                       width, height, low_buf, low_dim,
                       high_buf, high_dim) != RAPP_ERR_OVERLAP)
        {
            DBG("Src/dst overlap undetected\n");
            goto Done;
        }

        if (/* low == dst */
            (*test)(dst_buf, dst_dim, src_buf, src_dim,
                    dst_buf, low_dim, high_buf, high_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* low = far end of dst_buf */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment, low_dim,
                       high_buf, high_dim, width, height) != RAPP_ERR_OVERLAP
            /* low = before dst, but not long enough. */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       dst_buf - (low_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment), low_dim,
                       high_buf, high_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Low/dst overlap undetected\n");
            goto Done;
        }

        if (/* high == dst */
            (*test)(dst_buf, dst_dim, src_buf, src_dim,
                    low_buf, low_dim, dst_buf, high_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* high = far end of dst_buf */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       low_buf, low_dim, dst_buf + dst_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment, high_dim,
                       width, height) != RAPP_ERR_OVERLAP
            /* high = before dst, but not long enough. */
            || (*test)(dst_buf, dst_dim, src_buf, src_dim,
                       low_buf, low_dim, dst_buf - (high_dim*(height - 1) +
                       rapp_align((width + 7)/8) - rapp_alignment), high_dim,
                       width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("High/dst overlap undetected\n");
            goto Done;
        }

        if (k < (int)(sizeof special / sizeof special[0])) {
            /* Test special cases. */
            low  = special[k][0];
            high = special[k][1];
        }
        else {
            /* Test random cases. */
            low  = rapp_test_rand(0, 0xff);
            high = rapp_test_rand(0, 0xff);
        }

        memset(low_buf, low, low_dim*RAPP_TEST_HEIGHT);
        memset(high_buf, high, high_dim*RAPP_TEST_HEIGHT);

        /* Call RAPP function. */
        if ((*test)(dst_buf, dst_dim, src_buf, src_dim,
                    low_buf, low_dim, high_buf, high_dim,
                    width, height) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call reference function. */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim,
               low_buf, low_dim, high_buf, high_dim,
               width, height);

        /* Check result. */
        if (!rapp_test_compare_bin(dst_buf, dst_dim, ref_buf,
                                   dst_dim, 0, width, height))
        {
            DBG("Invalid result\n");
            DBG("src=\n");
            rapp_test_dump_u8(src_buf, src_dim, width, height);
            DBG("dst=\n");
            rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);

            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(dst_buf);
    rapp_free(src_buf);
    rapp_free(low_buf);
    rapp_free(high_buf);
    rapp_free(ref_buf);

    return ok;
}
