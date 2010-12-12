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
 *  @file   rapp_test_filter.c
 *  @brief  Correctness tests for fixed-filter convolutions.
 */

#include <string.h>          /* memcpy()      */
#include <math.h>            /* fabs()        */
#include <assert.h>          /* assert()      */
#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_filter.h" /* Reference API */
#include "rapp_test_util.h"  /* Test utils    */


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
#define RAPP_TEST_HEIGHT 32


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(),
                 int width, int height, int bits, float tol, float bias);

static bool
rapp_test_prec_driver(int (*func)(), void (*ref)(),
                      int width, int height, int bits, float tol, float bias);

static bool
rapp_test_rand_driver(int (*func)(), void (*ref)(), float tol, float bias);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_filter_diff_1x2_horz_u8(void)
{
    return rapp_test_driver(&rapp_filter_diff_1x2_horz_u8,
                            &rapp_ref_filter_diff_1x2_horz_u8,
                            2, 1, 2, 0.5f, 0.0f);
}

bool
rapp_test_filter_diff_1x2_horz_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_diff_1x2_horz_abs_u8,
                            &rapp_ref_filter_diff_1x2_horz_abs_u8,
                            2, 1, 2, 0.0f, 0.0f);
}

bool
rapp_test_filter_diff_2x1_vert_u8(void)
{
    return rapp_test_driver(&rapp_filter_diff_2x1_vert_u8,
                            &rapp_ref_filter_diff_2x1_vert_u8,
                            1, 2, 2, 0.5f, 0.0f);
}

bool
rapp_test_filter_diff_2x1_vert_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_diff_2x1_vert_abs_u8,
                            &rapp_ref_filter_diff_2x1_vert_abs_u8,
                            1, 2, 2, 0.0f, 0.0f);
}

bool
rapp_test_filter_diff_2x2_magn_u8(void)
{
    return rapp_test_driver(&rapp_filter_diff_2x2_magn_u8,
                            &rapp_ref_filter_diff_2x2_magn_u8,
                            2, 2, 2, 0.5f, 0.0f);
}

bool
rapp_test_filter_sobel_3x3_horz_u8(void)
{
    return rapp_test_driver(&rapp_filter_sobel_3x3_horz_u8,
                            &rapp_ref_filter_sobel_3x3_horz_u8,
                            3, 3, 2, 0.625f, 0.0f);
}

bool
rapp_test_filter_sobel_3x3_horz_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_sobel_3x3_horz_abs_u8,
                            &rapp_ref_filter_sobel_3x3_horz_abs_u8,
                            3, 3, 2, 0.75f, 0.0f);
}

bool
rapp_test_filter_sobel_3x3_vert_u8(void)
{
    return rapp_test_driver(&rapp_filter_sobel_3x3_vert_u8,
                            &rapp_ref_filter_sobel_3x3_vert_u8,
                            3, 3, 2, 0.625f, 0.0f);
}

bool
rapp_test_filter_sobel_3x3_vert_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_sobel_3x3_vert_abs_u8,
                            &rapp_ref_filter_sobel_3x3_vert_abs_u8,
                            3, 3, 2, 0.75f, 0.0f);
}

bool
rapp_test_filter_sobel_3x3_magn_u8(void)
{
    return rapp_test_driver(&rapp_filter_sobel_3x3_magn_u8,
                            &rapp_ref_filter_sobel_3x3_magn_u8,
                            3, 3, 2, 1.0f, -0.25f);
}

bool
rapp_test_filter_gauss_3x3_u8(void)
{
    return rapp_test_driver(&rapp_filter_gauss_3x3_u8,
                            &rapp_ref_filter_gauss_3x3_u8,
                            3, 3, 2, 0.75f, 0.0f);
}

bool
rapp_test_filter_laplace_3x3_u8(void)
{
    return rapp_test_driver(&rapp_filter_laplace_3x3_u8,
                            &rapp_ref_filter_laplace_3x3_u8,
                            3, 3, 2, 0.5f, 0.0f);
}

bool
rapp_test_filter_laplace_3x3_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_laplace_3x3_abs_u8,
                            &rapp_ref_filter_laplace_3x3_abs_u8,
                            3, 3, 2, 0.5f, 0.0f);
}

bool
rapp_test_filter_highpass_3x3_u8(void)
{
    return rapp_test_driver(&rapp_filter_highpass_3x3_u8,
                            &rapp_ref_filter_highpass_3x3_u8,
                            3, 3, 2, 0.625f, 0.0f);
}

bool
rapp_test_filter_highpass_3x3_abs_u8(void)
{
    return rapp_test_driver(&rapp_filter_highpass_3x3_abs_u8,
                            &rapp_ref_filter_highpass_3x3_abs_u8,
                            3, 3, 2, 0.75f, 0.0f);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*func)(), void (*ref)(),
                 int width, int height, int bits, float tol, float bias)
{
    return rapp_test_prec_driver(func, ref, width, height, bits, tol, bias) &&
        rapp_test_rand_driver(func, ref, tol, bias);
}

static bool
rapp_test_prec_driver(int (*func)(), void (*ref)(),
                      int width, int height, int bits, float tol, float bias)
{
    float    val = 0.0f;                           /* Reference value      */
    int      dim = 3*rapp_alignment;               /* Source buffer dim    */

    /**
     *  The tested filters (or to be precise, the reference functions) don't
     *  read more than half the MAX(width, height) above the image and no
     *  more than half the MAX(width, height) horizontally.
     */
    int   maxref = MAX(width, height) / 2;
    size_t alloc_size = (height + maxref) * dim + rapp_align(maxref);
    uint8_t *sbuf = rapp_malloc(alloc_size, 0);
    uint8_t *dst = rapp_malloc(alloc_size, 0);     /* Destination buffer   */
    uint8_t *src =                                 /* Source buffer, padded */
        &sbuf[dim * maxref + rapp_align(maxref)];
    uint8_t *pad = &src[-(maxref + dim * maxref)]; /* Source padding start */
    long     size = 1 << (bits*width*height);      /* Num of combinations  */
    long     code;
    int ret;

    /* Check that src is within the allocated sbuf size. */
    assert(src < sbuf + alloc_size);
    /* Check that the max written index of pad[] is within sbuf. */
    assert(sbuf <= pad &&
           pad + (height - 1) * dim + width - 1 < sbuf + alloc_size);
    assert((unsigned)bits*width*height < 8*sizeof size);
    assert((unsigned)width < rapp_alignment * 2);
    memset(sbuf, 0, alloc_size);

    /* Test all input combinations with different 'bits' lowest bits */
    for (code = 0; code < size; code++) {
        int   sh, x, y;
        float err;

        /* Initialize the source image, starting at the used padding */
        for (y = 0, sh = 0; y < height; y++) {
            for (x = 0; x < width; x++, sh += bits) {
                pad[y*dim + x] = (code >> sh) & ((1 << bits) - 1);
            }
        }

        /* Call the convolution function */
        ret = (*func)(dst, dim, src, dim, 1, 1);
        if (ret < 0) {
            DBG("Got FAIL return value %d\n", ret);
            return false;
        }

        /* Call the reference function */
        (*ref)(&val, sizeof val, src, dim, 1, 1);

        /* Compare the results */
        err = fabs((float)dst[0] - bias - val);
        if (err > tol) {
            DBG("Error = %.4f\n", err);
            DBG("dst=%d\n", dst[0]);
            DBG("val=%f\n", val);
            DBG("src=\n");
            rapp_test_dump_u8(pad, dim, 3, 3);
            return false;
        }
    }

    rapp_free(sbuf);
    rapp_free(dst);

    return true;
}

static bool
rapp_test_rand_driver(int (*func)(), void (*ref)(), float tol, float bias)
{
    int      src_dim = rapp_align(RAPP_TEST_WIDTH) + 2*rapp_alignment;
    int      dst_dim = rapp_align(RAPP_TEST_WIDTH);

    uint8_t *pad_buf = rapp_malloc(src_dim*(RAPP_TEST_HEIGHT + 2), 0);
    uint8_t *src_buf = &pad_buf[src_dim + rapp_alignment];
    uint8_t *dst_buf = rapp_malloc(dst_dim* RAPP_TEST_HEIGHT, 0);
    float   *ref_buf = malloc(RAPP_TEST_WIDTH*RAPP_TEST_HEIGHT*sizeof(float));
    int      k;
    bool     ok = false;

    /* Initialize the source buffer */
    rapp_test_init(pad_buf, 0, src_dim*(RAPP_TEST_HEIGHT + 2), 1, true);

    /* Run random tests */
    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int w = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int h = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int x, y;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*func)(dst_buf, dst_dim, dst_buf, src_dim, w, h) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*func)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(h - 1) +
                       rapp_align(w) - rapp_alignment,
                       src_dim, w, h) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*func)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(h - 1) +
                                  rapp_align(w) - rapp_alignment),
                       src_dim, w, h) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Call the convolution function */
        if ((*func)(dst_buf, dst_dim, src_buf, src_dim, w, h) < 0) {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, w*sizeof(float), src_buf, src_dim, w, h);

        /* Compare the results */
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                int   i = y*dst_dim + x;
                int   j = y*w + x;
                float e = fabs(dst_buf[i] - ref_buf[j] - bias);
                if (e > tol) {
                    DBG("Error=%.4f\n", e);
                    DBG("ref=%.4f\n", ref_buf[j]);
                    DBG("dst=%d\n", dst_buf[i]);
                    rapp_test_dump_u8(&src_buf[-src_dim - 1],
                                      src_dim, w + 2, h + 2);
                    goto Done;
                }
            }
        }
    }

    ok = true;

Done:
    rapp_free(pad_buf);
    rapp_free(dst_buf);
    free(ref_buf);

    return ok;
}
