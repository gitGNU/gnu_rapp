/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_reduce.c
 *  @brief  Correctness tests for 8-bit 2x reduction.
 */

#include <string.h>          /* memcpy()       */
#include <math.h>            /* fabs()         */
#include "rapp.h"            /* RAPP API       */
#include "rapp_ref_reduce.h" /* Reference API  */
#include "rapp_test_util.h"  /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER   512

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH  256

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 256

/**
 *  The number of LSBs in exhaustive test.
 */
#define RAPP_TEST_BITS 3


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(),
                 int xscale, int yscale, float tol);
static bool
rapp_test_prec_driver(int (*func)(), void (*ref)(), float tol);

static bool
rapp_test_rand_driver(int (*func)(), void (*ref)(),
                      int xscale, int yscale, float tol);

/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_reduce_1x2_u8(void)
{
    return rapp_test_driver(&rapp_reduce_1x2_u8,
                            &rapp_ref_reduce_1x2_u8,
                            2, 1, 0.5f);
}

bool
rapp_test_reduce_2x1_u8(void)
{
    return rapp_test_driver(&rapp_reduce_2x1_u8,
                            &rapp_ref_reduce_2x1_u8,
                            1, 2, 0.5f);
}

bool
rapp_test_reduce_2x2_u8(void)
{
    return rapp_test_driver(&rapp_reduce_2x2_u8,
                            &rapp_ref_reduce_2x2_u8,
                            2, 2, 0.5f);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(),
                 int xscale, int yscale, float tol)
{
    return rapp_test_prec_driver(func, ref, tol) &&
           rapp_test_rand_driver(func, ref, xscale, yscale, tol);
}

static bool
rapp_test_prec_driver(int (*func)(), void (*ref)(), float tol)
{
    float    val[2] = {0.0f, 0.0f};             /* Reference value        */
    int      dim    = rapp_alignment;           /* Source buffer dim      */
    uint8_t *src    = rapp_malloc(2*dim, 0);    /* Source buffer          */
    uint8_t *dst    = rapp_malloc(2*dim, 0);    /* Destination buffer     */
    long     size   = 1 << (RAPP_TEST_BITS*4); /* Number of combinations */
    long     code;

    memset(src, 0, 2*dim);

    /* Test all combinations with different RAPP_TEST_BITS lowest bits */
    for (code = 0; code < size; code++) {
        int   sh, x, y;
        float err;

        /* Initialize the source image */
        for (y = 0, sh = 0; y < 2; y++) {
            for (x = 0; x < 2; x++, sh += RAPP_TEST_BITS) {
                src[y*dim + x] = (code >> sh) & ((1 << RAPP_TEST_BITS) - 1);
            }
        }

        /* Call the scaling function */
        if ((*func)(dst, dim, src, dim, 2, 2) < 0) {
            DBG("Got FAIL return value, %p dim %d -> %p\n", src, dim, dst);
            return false;
        }

        /* Call the reference function */
        (*ref)(val, sizeof val[0], src, dim, 2, 2);

        /* Compare the results */
        err = fabs((float)dst[0] - val[0]);
        if (err > tol) {
            DBG("Error = %.4f\n", err);
            DBG("dst=%d\n", dst[0]);
            DBG("val=%f\n", val[0]);
            DBG("src=\n");
            rapp_test_dump_u8(src, dim, 2, 2);
            return false;
        }
    }

    rapp_free(src);
    rapp_free(dst);

    return true;
}

static bool
rapp_test_rand_driver(int (*func)(), void (*ref)(),
                      int xscale, int yscale, float tol)
{
    int      src_dim = rapp_align(RAPP_TEST_WIDTH) + rapp_alignment;
    int      dst_dim = rapp_align(RAPP_TEST_WIDTH/xscale);

    /* The dst buffer can be on either side of the src buffer */
    uint8_t *srcdst_buf = rapp_malloc((src_dim*RAPP_TEST_HEIGHT +
                                       2*dst_dim*(RAPP_TEST_HEIGHT / yscale)),
                                      0);
    uint8_t *src_buf = srcdst_buf + dst_dim*(RAPP_TEST_HEIGHT / yscale);
    float   *ref_buf = malloc(RAPP_TEST_WIDTH*RAPP_TEST_HEIGHT*sizeof(float));
    int      k;
    bool     ok = false;

    /* Initialize the source buffer */
    rapp_test_init(src_buf, 0, src_dim*RAPP_TEST_HEIGHT, 1, true);

    /* Run random tests */
    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int w = rapp_test_rand(2, RAPP_TEST_WIDTH);
        int h = rapp_test_rand(2, RAPP_TEST_HEIGHT);
        int src_len = (h - 1)*src_dim + rapp_align(w);
        int dst_len = (h / yscale - 1)*dst_dim + rapp_align(w / xscale);
        uint8_t *dst_buf = rapp_test_rand(0, 1) ?
                           src_buf + src_len : src_buf - dst_len;
        int x, y;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*func)(dst_buf, dst_dim, dst_buf, src_dim,
                    w, h) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*func)(dst_buf, dst_dim,
                       dst_buf + dst_len - rapp_alignment,
                       src_dim, w, h) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*func)(dst_buf, dst_dim,
                       dst_buf - (src_len - rapp_alignment),
                       src_dim, w, h) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected, (%d/%d, %d/%d) %p dim %d -> dim %d (%d)\n",
                w, xscale, h, yscale, src_buf, src_dim, dst_dim, rapp_alignment);
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, w*sizeof(float), src_buf, src_dim, w, h);

        /* Call the reduction function */
        if ((*func)(dst_buf, dst_dim, src_buf, src_dim, w, h) < 0) {
            DBG("Got FAIL return value, (%d,%d) %p dim %d -> %p dim %d (%d)\n",
                w, h, src_buf, src_dim, dst_buf, dst_dim, rapp_alignment);
            goto Done;
        }

        /* Compare the results */
        for (y = 0; y < h/yscale; y++) {
            for (x = 0; x < w/xscale; x++) {
                int   i = y*dst_dim + x;
                int   j = y*w + x;
                float e = fabs(ref_buf[j] - dst_buf[i]);
                if (e > tol) {
                    DBG("Error=%.4f (tol=%.4f) at x=%d, y=%d, test #%d\n", e, tol, x, y, k);
                    DBG("ref=%.4f\n", ref_buf[j]);
                    DBG("dst=%d\n", dst_buf[i]);
                    DBG("width=%d, height=%d, src_dim=%d, dst_dim=%d\n",
                        w, h, src_dim, dst_dim);
                    DBG("src=\n");
                    rapp_test_dump_u8(src_buf, src_dim, w, h);
                    DBG("dst=\n");
                    rapp_test_dump_u8(dst_buf, dst_dim, w/xscale, h/yscale);
                    goto Done;
                }
            }
        }
    }

    ok = true;

Done:
    rapp_free(srcdst_buf);
    free(ref_buf);

    return ok;
}
