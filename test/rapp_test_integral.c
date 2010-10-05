/*  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_integral.c
 *  @brief  Correctness tests for integral sums.
 */

#include <string.h> /* memset() */
#include "rapp.h" /* RAPP API */
#include "rapp_ref_integral.h" /* Reference API */
#include "rapp_test_util.h" /* Test utils */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER 128

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH 1024

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 768


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_integral_driver(int (*test)(), void (*ref)(),
        bool binary, size_t dst_size);

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_integral_bin_u8(void)
{
    return rapp_test_integral_driver(&rapp_integral_sum_bin_u8,
            &rapp_ref_integral_sum_bin_u8, true, 1);
}

bool
rapp_test_integral_bin_u16(void)
{
    return rapp_test_integral_driver(&rapp_integral_sum_bin_u16,
            &rapp_ref_integral_sum_bin_u16, true, 2);
}

bool
rapp_test_integral_bin_u32(void)
{
    return rapp_test_integral_driver(&rapp_integral_sum_bin_u32,
            &rapp_ref_integral_sum_bin_u32, true, 4);
}

bool
rapp_test_integral_u8_u16(void)
{
    return rapp_test_integral_driver(&rapp_integral_sum_u8_u16,
            &rapp_ref_integral_sum_u8_u16, false, 2);
}

bool
rapp_test_integral_u8_u32(void)
{
    return rapp_test_integral_driver(&rapp_integral_sum_u8_u32,
            &rapp_ref_integral_sum_u8_u32, false, 4);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_integral_driver(int (*test)(), void (*ref)(),
        bool binary, size_t bytes_per_pixel)
{
    uint8_t *src;
    uint8_t *dst1;
    uint8_t *dst2;
    int src_dim;
    int src_width;
    int dst_dim;
    int offset;
    int pad;
    bool pass = false;
    int iter;

    /* Calculate buffer sizes */
    src_width = binary ? (RAPP_TEST_WIDTH + 7) / 8 : RAPP_TEST_WIDTH;
    src_dim = rapp_align(src_width);

    /* Pad destination buffer to the left */
    pad = rapp_alignment;
    dst_dim = rapp_align(RAPP_TEST_WIDTH) * bytes_per_pixel + pad;
    offset = dst_dim + pad;

    /* Alocate buffers */
    src = rapp_malloc(src_dim * RAPP_TEST_HEIGHT, 0);
    dst1 = rapp_malloc(dst_dim * (RAPP_TEST_HEIGHT + 2), 0);
    dst2 = rapp_malloc(dst_dim * (RAPP_TEST_HEIGHT + 2), 0);

    /* Clear buffer */
    memset(src, 0, src_dim * RAPP_TEST_HEIGHT);

    /* Set the pointer the processing part, inside the border */
    dst1 += offset;
    dst2 += offset;

    /* Perform tests */
    for (iter = 0; iter < RAPP_TEST_ITER; iter++) {
        int width = rapp_test_rand(1, RAPP_TEST_WIDTH);
        int height = rapp_test_rand(1, RAPP_TEST_HEIGHT);
        int len = binary ? (width + 7) / 8 : width;
        int n_rows_cleared = rapp_test_rand(0, height / 4);
        int c;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*test)(dst1, dst_dim, dst1, src_dim,
                    width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(dst1, dst_dim,
                   dst1 + dst_dim*(height - 1) +
                   rapp_align(width*bytes_per_pixel) - rapp_alignment,
                   src_dim, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(dst1, dst_dim,
                       dst1 - (src_dim*(height - 1) +
                               rapp_align(binary ? (width + 7) / 8 :
                                          width) - rapp_alignment),
                       src_dim, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the source buffer */
        rapp_test_init(src, src_dim, len, height, true);

        /* Clear some rows to test optimization */
        for (c = 0; c < n_rows_cleared; c++) {
            int row = rapp_test_rand(0, height - 1);

            memset(&src[src_dim * row], 0, len);
        }

        /* Clear the destination buffers */
        memset(dst1 - offset, 0, dst_dim * (RAPP_TEST_HEIGHT + 2));
        memset(dst2 - offset, 0, dst_dim * (RAPP_TEST_HEIGHT + 2));

        /* Call the integral function */
        if ((*test)(dst1, dst_dim, src, src_dim, width, height) != 0) {
            DBG("Integral function failed\n");
            goto Done;
        }

        /* Call the reference implementation */
        (*ref)(dst2, dst_dim, src, src_dim, width, height);

        /* Compare the result */
        if (rapp_test_compare_u8(dst1, dst_dim, dst2, dst_dim,
                    width * bytes_per_pixel, height) == false) {
            DBG("Compare failed\n");
            goto Done;
        }
    }

    pass = true;

Done:
    /* Free the source buffer */
    rapp_free(src);
    rapp_free(dst1 - offset);
    rapp_free(dst2 - offset);

    return pass;
}
