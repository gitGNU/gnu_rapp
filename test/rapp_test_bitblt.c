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
 *  @file   rapp_test_bitblt.c
 *  @brief  Correctness tests for bitblt conversions.
 */

#include <string.h>          /* memcpy()      */
#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_bitblt.h" /* Reference API */
#include "rapp_test_util.h"  /* Test utils    */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The number of test iterations.
 */
#define RAPP_TEST_ITER  1024

/**
 *  Test image maximum width.
 */
#define RAPP_TEST_WIDTH  (8*8*rapp_alignment)

/**
 *  Test image maximum height.
 */
#define RAPP_TEST_HEIGHT 4


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_bitblt_driver(int (*test)(), void (*ref)());


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_bitblt_copy_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_copy_bin,
                                   &rapp_ref_bitblt_copy_bin);
}

bool
rapp_test_bitblt_not_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_not_bin,
                                   &rapp_ref_bitblt_not_bin);
}

bool
rapp_test_bitblt_and_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_and_bin,
                                   &rapp_ref_bitblt_and_bin);
}

bool
rapp_test_bitblt_or_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_or_bin,
                                   &rapp_ref_bitblt_or_bin);
}

bool
rapp_test_bitblt_xor_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_xor_bin,
                                   &rapp_ref_bitblt_xor_bin);
}

bool
rapp_test_bitblt_nand_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_nand_bin,
                                   &rapp_ref_bitblt_nand_bin);
}

bool
rapp_test_bitblt_nor_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_nor_bin,
                                   &rapp_ref_bitblt_nor_bin);
}

bool
rapp_test_bitblt_xnor_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_xnor_bin,
                                   &rapp_ref_bitblt_xnor_bin);
}

bool
rapp_test_bitblt_andn_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_andn_bin,
                                   &rapp_ref_bitblt_andn_bin);
}

bool
rapp_test_bitblt_orn_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_orn_bin,
                                   &rapp_ref_bitblt_orn_bin);
}

bool
rapp_test_bitblt_nandn_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_nandn_bin,
                                   &rapp_ref_bitblt_nandn_bin);
}

bool
rapp_test_bitblt_norn_bin(void)
{
    return rapp_test_bitblt_driver(&rapp_bitblt_norn_bin,
                                   &rapp_ref_bitblt_norn_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_bitblt_driver(int (*test)(), void (*ref)())
{
    int      dim     = rapp_align((RAPP_TEST_WIDTH + 7) / 8) +
                       rapp_alignment;
    uint8_t *srcdst_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT*3 +
                                      2*rapp_alignment, 0);
    uint8_t *src_buf = srcdst_buf + dim*RAPP_TEST_HEIGHT + rapp_alignment;
    uint8_t *ref_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    uint8_t *srcref_buf = rapp_malloc(dim*RAPP_TEST_HEIGHT, 0);
    int      k;
    bool     ok = false;

    /* Initialize the source buffer */
    rapp_test_init(srcref_buf, 0, dim*RAPP_TEST_HEIGHT, 1, true);

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int width   = k > 0 ? rapp_test_rand(1, RAPP_TEST_WIDTH) :
                              (int)RAPP_TEST_WIDTH;
        int height  = k > 0 ? rapp_test_rand(1, RAPP_TEST_HEIGHT) :
                              RAPP_TEST_HEIGHT;
        int dst_idx = rapp_test_rand(0, rapp_alignment - 1);
        int src_idx = rapp_test_rand(0, rapp_alignment - 1);
        int dst_off = rapp_test_rand(0, 7);
        int src_off = rapp_test_rand(0, 1) ?
                      rapp_test_rand(0, 7) : dst_off;
        int dst_dim = rapp_align(dst_idx + (width + dst_off + 7) / 8);
        int src_dim = rapp_align(src_idx + (width + src_off + 7) / 8);
        int src_len = src_dim * height;
        int dst_len = dst_dim * height;
        /**
         *  Place the destination buffer on either side of the source
         *  buffer, as close as possible.
         */
        uint8_t *dst_buf = rapp_test_rand(0, 1) ?
                           src_buf + src_len : src_buf - dst_len;

        /* Verify that we get an overlap error for overlapping buffers */
        if (/* src == dst */
            (*test)(&dst_buf[dst_idx], dst_dim, dst_off, &dst_buf[dst_idx],
                    src_dim, src_off, width, height) != RAPP_ERR_OVERLAP
            /* src = far end of dst_buf */
            || (*test)(&dst_buf[dst_idx], dst_dim, dst_off,
                       &dst_buf[dst_idx] + dst_len - 1 - dst_idx,
                       src_dim, src_off, width, height) != RAPP_ERR_OVERLAP
            /* src = before dst, but not long enough */
            || (*test)(&src_buf[src_idx] + src_len - 1 - src_idx,
                       dst_dim, dst_off,
                       &src_buf[src_idx],
                       src_dim, src_off, width, height) != RAPP_ERR_OVERLAP)
        {
            DBG("Overlap undetected\n");
            goto Done;
        }

        /* Initialize the source and destination buffers */
        rapp_test_init(dst_buf, 0, dst_len, 1, true);
        memcpy(ref_buf, dst_buf, dst_len);
        memcpy(src_buf, srcref_buf, src_len);

        /**
         *  Call the reference function first. We trust it not to overwrite
         *  src_buf, so we're guaranteed to see the same src_buf for both.
         */
        (*ref)(&ref_buf[dst_idx], dst_dim, dst_off,
               &src_buf[src_idx], src_dim, src_off,
               width, height);

        /* Call RAPP function */
        if ((*test)(&dst_buf[dst_idx], dst_dim, dst_off,
                    &src_buf[src_idx], src_dim, src_off,
                    width, height) < 0)
        {
            DBG("Got FAIL return value\n");
            goto Done;
        }

        /* Check result */
        if (!rapp_test_compare_bin(&dst_buf[dst_idx], dst_dim,
                                   &ref_buf[dst_idx], dst_dim,
                                   dst_off, width, height) ||
            memcmp (src_buf, srcref_buf, src_len) != 0)
        {
            DBG("Invalid result (%d,%d) dim %d -> %d, off %d -> %d;"
                " %p+%d to %p+%d (%d)\n",
                width, height, src_dim, dst_dim, src_off, dst_off,
                src_buf, src_idx, dst_buf, dst_idx, rapp_alignment);
            DBG("src_ref=\n");
            rapp_test_dump_bin(&srcref_buf[src_idx],
                               src_dim, src_off,
                               width, height);
            DBG("src=\n");
            rapp_test_dump_bin(&src_buf[src_idx],
                               src_dim, src_off,
                               width, height);
            DBG("dst=\n");
            rapp_test_dump_bin(&dst_buf[dst_idx],
                               dst_dim, dst_off,
                               width, height);
            DBG("ref=\n");
            rapp_test_dump_bin(&ref_buf[dst_idx],
                               dst_dim, dst_off,
                               width, height);
            goto Done;
        }
    }

    ok = true;

Done:
    rapp_free(srcref_buf);
    rapp_free(ref_buf);
    rapp_free(srcdst_buf);

    return ok;
}
