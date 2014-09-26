/*  Copyright (C) 2014, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_framework.c
 *  @brief  Correctness tests for RAPP framework functions.
 */

#include <string.h>
#include "rapp.h"
#include "rapp_test_util.h"

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_framework(void)
{
    size_t ra = rapp_alignment;
    size_t raf = rapp_align(1);
    uint8_t *src_alloc = NULL;
    uint8_t *dst_alloc = NULL;
    uint8_t *ref_alloc = NULL;
    uint8_t *src;
    uint8_t *dst;

    int i, j;
    /**
     *  The 256 is somewhat arbitrary. It's intended to reflect the remaining
     *  *actual* alignment multiplier requirement if we got it wrong.
     */
    size_t opsize = 256 * rapp_alignment;
    size_t src_alloc_size = opsize + 3 * rapp_alignment;
    size_t dst_alloc_size = opsize + 3 * rapp_alignment;
    bool ok = true;

    /* First a trivial sanity-check that rapp_align matches rapp_alignment. */
    if (raf != ra) {
        DBG("rapp_align(1) = %d, rapp_alignment = %d\n", (int) ra, (int) raf);
        ok = false;
        goto Done;
    }

    /**
     *  Second, check that vector granularity matches rapp_alignment.
     *  We assume rapp_pixop_add_u8 uses whatever vector implementation
     *  is available, and then we check that it doesn't overwrite
     *  anything when shifting by rapp_alignment. Implicitly, access
     *  alignment is checked by the test completing without a fatal
     *  signal. While an alignment check could be folded into each
     *  function, making it a separate test helps keeping the test
     *  focused.
     *
     *  If alignment is wrong, you'll likely see a SIGSEGV or SIGBUS,
     *  not a plain wrong result. While we could use a signal handler to
     *  then improve the error report, it doesn't seem worthwhile, in
     *  presence of this test and comment; it's sufficiently better than
     *  the pixelwise or bitblt tests failing with a bad failure mode
     *  that indicates an operation being implemented wrongly, before an
     *  eventual fatal signal.
     */
    src_alloc = rapp_malloc(src_alloc_size, 0);
    dst_alloc = rapp_malloc(dst_alloc_size, 0);
    ref_alloc = rapp_malloc(opsize + 2*rapp_alignment, 0);
    memset(ref_alloc, 0xae, rapp_alignment);
    memset(ref_alloc + rapp_alignment, 3, opsize);
    memset(ref_alloc + rapp_alignment + opsize, 0xae, rapp_alignment);

    for (i = 0; i <= 1; i++) {
        for (j = 0; j <= 1; j++) {
            int ret;

            src = src_alloc + (i + 1) * rapp_alignment;
            dst = dst_alloc + (j + 1) * rapp_alignment;

            /**
             *  dst = src2 = { ... 0xae, 1, ..., 1, 0xae, ... }
             *  src1 = src = { ... 0xff, 2, ..., 2, 0xff, ... }
             *  op()
             *  dst = { 0xae, 3, ..., 3, 0xae, ... }
             */
            memset(dst - rapp_alignment, 0xae, rapp_alignment);
            memset(dst, 1, opsize);
            memset(dst + opsize, 0xae, rapp_alignment);

            memset(src - rapp_alignment, 0xff, rapp_alignment);
            memset(src, 2, opsize);
            memset(src + opsize, 0xff, rapp_alignment);

            ret = rapp_pixop_add_u8(dst, opsize, src, opsize, opsize, 1);
            if (ret != 0) {
                DBG("alignment test: got %d expecting 0 for offset (%d,%d)*%d\n",
                    ret, i, j, rapp_alignment);
                ok = false;
                goto Done;
            }

            if (!rapp_test_compare_u8(dst - rapp_alignment,
                                      opsize + 2*rapp_alignment,
                                      ref_alloc, opsize + 2*rapp_alignment,
                                      opsize + 2*rapp_alignment, 1)) {
                DBG("alignment test fails compare.  Ref:\n");
                rapp_test_dump_u8(ref_alloc,
                                  opsize + 2*rapp_alignment,
                                  opsize + 2*rapp_alignment, 1);
                DBG("Dst:\n");
                rapp_test_dump_u8(dst - rapp_alignment,
                                  opsize + 2*rapp_alignment,
                                  opsize + 2*rapp_alignment, 1);
                ok = false;
                goto Done;
            }
        }
    }

Done:
    rapp_free(ref_alloc);
    rapp_free(dst_alloc);
    rapp_free(src_alloc);
    return ok;
}
