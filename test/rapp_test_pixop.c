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
 *  @file   rapp_test_pixop.c
 *  @brief  Correctness tests for pixelwise operations.
 */

#include <string.h>         /* memcpy()      */
#include "rapp.h"           /* RAPP API      */
#include "rapp_ref_pixop.h" /* Reference API */
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
rapp_test_pixop_driver(int (*test)(), void (*ref)(),
                       bool arg1_lut, int arg1_min, int arg1_max,
                       int arg2_min, int arg2_max);
static bool
rapp_test_pixop_driver2(int (*test)(), void (*ref)(),
                        int arg_min, int arg_max);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_pixop_set_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_set_u8,
                                  &rapp_ref_pixop_set_u8,
                                  false, 0, 0xff, 0, 0);
}

bool
rapp_test_pixop_not_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_not_u8,
                                  &rapp_ref_pixop_not_u8,
                                  false, 0, 0, 0, 0);
}

bool
rapp_test_pixop_flip_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_flip_u8,
                                  &rapp_ref_pixop_flip_u8,
                                  false, 0, 0, 0, 0);
}

bool
rapp_test_pixop_lut_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_lut_u8,
                                  &rapp_ref_pixop_lut_u8,
                                  true, 0, 0, 0, 0);
}

bool
rapp_test_pixop_abs_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_abs_u8,
                                  &rapp_ref_pixop_abs_u8,
                                  false, 0, 0, 0, 0);
}

bool
rapp_test_pixop_addc_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_addc_u8,
                                  &rapp_ref_pixop_addc_u8,
                                  false, -0xff, 0xff, 0, 0);
}

bool
rapp_test_pixop_lerpc_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_lerpc_u8,
                                  &rapp_ref_pixop_lerpc_u8,
                                  false, 0, 0xff, 0, 0x100);
}

bool
rapp_test_pixop_lerpnc_u8(void)
{
    return rapp_test_pixop_driver(&rapp_pixop_lerpnc_u8,
                                  &rapp_ref_pixop_lerpnc_u8,
                                  false, 0, 0xff, 0, 0x100);
}

bool
rapp_test_pixop_copy_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_copy_u8,
                                   &rapp_ref_pixop_copy_u8,
                                   0, 0);
}

bool
rapp_test_pixop_add_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_add_u8,
                                   &rapp_ref_pixop_add_u8,
                                   0, 0);
}

bool
rapp_test_pixop_avg_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_avg_u8,
                                   &rapp_ref_pixop_avg_u8,
                                   0, 0);
}

bool
rapp_test_pixop_sub_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_sub_u8,
                                   &rapp_ref_pixop_sub_u8,
                                   0, 0);
}

bool
rapp_test_pixop_subh_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_subh_u8,
                                   &rapp_ref_pixop_subh_u8,
                                   0, 0);
}

bool
rapp_test_pixop_suba_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_suba_u8,
                                   &rapp_ref_pixop_suba_u8,
                                   0, 0);
}

bool
rapp_test_pixop_lerp_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_lerp_u8,
                                   &rapp_ref_pixop_lerp_u8,
                                   0, 0x100);
}

bool
rapp_test_pixop_lerpn_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_lerpn_u8,
                                   &rapp_ref_pixop_lerpn_u8,
                                   0, 0x100);
}

bool
rapp_test_pixop_lerpi_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_lerpi_u8,
                                   &rapp_ref_pixop_lerpi_u8,
                                   0, 0x100);
}

bool
rapp_test_pixop_norm_u8(void)
{
    return rapp_test_pixop_driver2(&rapp_pixop_norm_u8,
                                   &rapp_ref_pixop_norm_u8,
                                   0, 0);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_pixop_driver(int (*test)(), void (*ref)(),
                       bool arg1_lut, int arg1_min, int arg1_max,
                       int arg2_min, int arg2_max)
{
    int      dim     = rapp_align(RAPP_TEST_WIDTH);
    uint8_t *dst_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t  lut[256];
    int      iter;

    /* Initialize the LUT */
    rapp_test_init(lut, 0, sizeof lut, 1, false);

    /* Perform tests */
    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int      width   = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int      height  = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        intptr_t arg1    = arg1_lut ? (intptr_t)lut :
                           rapp_test_rand(arg1_min, arg1_max);
        int      arg2    = rapp_test_rand(arg2_min, arg2_max);
        int      dst_dim = rapp_test_rand(0, 1) ?
                           (int)rapp_align(width) : dim;

        /* Initialize the buf and ref buffers */
        rapp_test_init(dst_buf, 0, dst_dim*height, 1, true);
        memcpy(ref_buf, dst_buf, dst_dim*height);

        /* Call the pixel operation function */
        if ((*test)(dst_buf, dst_dim, width, height, arg1, arg2) < 0) {
            DBG("Got FAIL return value\n");
            return false;
        }

        /* Call the reference function */
        (*ref)(ref_buf, dst_dim, width, height, arg1, arg2);

        /* Compare the result */
        if (!rapp_test_compare_u8(ref_buf, dst_dim,
                                  dst_buf, dst_dim, width, height))
        {
            DBG("Invalid result\n");
            DBG("dst_dim=%d, width=%d, height=%d, arg1=%ld, arg2=%d\ndst=\n",
                    dst_dim, width, height, (long) arg1, arg2);
            rapp_test_dump_u8(dst_buf, dst_dim, width, height);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf, dst_dim, width, height);
            return false;
        }
    }

    rapp_free(dst_buf);
    rapp_free(ref_buf);

    return true;
}

static bool
rapp_test_pixop_driver2(int (*test)(), void (*ref)(),
                        int arg_min, int arg_max)
{
    int      dim     = rapp_align(RAPP_TEST_WIDTH);
    uint8_t *src_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *dst_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *ref_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *odst_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    int      iter;
    bool     ok = false;

    /* Initialize the src buffer */
    rapp_test_init(src_buf, 0, dim*RAPP_TEST_HEIGHT, 1, false);

    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int width   = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height  = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int arg     = rapp_test_rand(arg_min, arg_max);
        int src_dim = rapp_test_rand(0, 1) ?
                      (int)rapp_align(width) : dim;
        int dst_dim = rapp_test_rand(0, 1) ?
                      (int)rapp_align(width) : dim;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*test)(dst_buf, dst_dim, dst_buf, src_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst_buf, dst_dim,
                       dst_buf + dst_dim*(height - 1) +
                       rapp_align(width) - rapp_alignment,
                       src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(dst_buf, dst_dim,
                       dst_buf - (src_dim*(height - 1) +
                                  rapp_align(width) - rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the dst and ref buffers */
        rapp_test_init(dst_buf, 0, dst_dim*height, 1, false);
        memcpy(ref_buf, dst_buf, dst_dim*height);
        memcpy(odst_buf, dst_buf, dst_dim*height);

        /* Call the pixel operation function */
        if ((*test)(dst_buf, dst_dim, src_buf, src_dim,
                    width, height, arg) < 0)
        {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Call the reference function */
        (*ref)(ref_buf, dst_dim, src_buf, src_dim, width, height, arg);

        /* Compare the result */
        if (!rapp_test_compare_u8(ref_buf, dst_dim, dst_buf,
                                  dst_dim, width, height))
        {
            DBG("Invalid result at iteration %d\n", iter);
            DBG("src_dim=%d, dst_dim=%d, width=%d, height=%d, arg=%#x\nsrc=\n",
                    src_dim, dst_dim, width, height, arg);
            rapp_test_dump_u8(src_buf, src_dim, width, height);
            DBG("original dst=\n");
            rapp_test_dump_u8(odst_buf, dst_dim, width, height);
            DBG("dst=\n");
            rapp_test_dump_u8(dst_buf, dst_dim, width, height);
            DBG("ref=\n");
            rapp_test_dump_u8(ref_buf, dst_dim, width, height);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(src_buf);
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    rapp_free(odst_buf);

    return ok;
}
