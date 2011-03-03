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
 *  @file   rc_test.c
 *  @brief  Correctness tests for the vector backend implementations.
 */

#include <stdio.h>      /* fprintf()          */
#include <stdlib.h>     /* abs()              */
#include <stdint.h>     /* uint8_t            */
#include <string.h>     /* memset(), memcmp() */
#include "rc_vector.h"  /* Vector operations  */
#include "rc_stdbool.h" /* Portable stdbool.h */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

#define NTESTS (1 << 14)

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#define DBG printf

#define RC_TEST_ENTRY(operation, full, op2, min, max) \
    {#operation,                                      \
     rc_test_vec_ ## operation,                       \
     rc_test_vec_ ## operation ## _ref,               \
     full, op2, {min, max}}


/*
 * -------------------------------------------------------------
 *  Test functions
 * -------------------------------------------------------------
 */

#include "rc_test_vec_def.h"

/*
 * -------------------------------------------------------------
 *  Reference functions
 * -------------------------------------------------------------
 */

#define RC_TEST_VEC_REF
#include "rc_vec_ref.h"
#include "rc_test_vec_def.h"


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rc_test_vector(void);

static bool
rc_test_exec(int (*test)(), int (*ref)(),
             uint8_t *dst_buf, uint8_t *ref_buf,
             const uint8_t *src1_buf, const uint8_t *src2_buf, int arg);
static bool
rc_test_vec_full(int (*test)(), int (*ref)(),
                 uint8_t *dst_buf, uint8_t *ref_buf,
                 uint8_t *src1_buf, uint8_t *src2_buf,
                 const int *args);
static bool
rc_test_vec_rand(int (*test)(), int (*ref)(),
                 uint8_t *dst_buf, uint8_t *ref_buf,
                 uint8_t *src1_buf, uint8_t *src2_buf,
                 const int *args);
static void
rc_test_dump(const char *text, const uint8_t *vec);

static int
rc_test_rand(int min, int max);

static void
rc_test_init(uint8_t *buf, int len);


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static const char *rc_test_backend =
#ifdef RAPP_USE_SIMD
#if RAPP_ENABLE_SIMD
    RAPP_INFO_SIMD;
#else
    NULL;
#endif
#else
    "SWAR";
#endif

static const struct rc_test_entry_st {
    const char  *name;    /**< Name of operation         */
    int        (*test)(); /**< Vector test function      */
    int        (*ref)();  /**< Scalar reference function */
    bool         full;    /**< Full test or not          */
    bool         op2;     /**< Two or one operands       */
    int          arg[2];  /**< Argument range            */
} rc_test_table[] = {
    RC_TEST_ENTRY(zero,     true,  false, 0, 0),
    RC_TEST_ENTRY(not,      true,  false, 0, 0),
    RC_TEST_ENTRY(and,      true,  true,  0, 0),
    RC_TEST_ENTRY(or,       true,  true,  0, 0),
    RC_TEST_ENTRY(xor,      true,  true,  0, 0),
    RC_TEST_ENTRY(andnot,   true,  true,  0, 0),
    RC_TEST_ENTRY(ornot,    true,  true,  0, 0),
    RC_TEST_ENTRY(xornot,   true,  true,  0, 0),
    RC_TEST_ENTRY(loadu,    false, true,  1, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(shl,      false, false, 0, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(shr,      false, false, 0, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(shlc,     false, false, 0, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(shrc,     false, false, 0, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(alignc,   false, true,  1, RC_VEC_SIZE - 1),
    RC_TEST_ENTRY(pack,     false, true,  0, 0),
    RC_TEST_ENTRY(splat,    true,  false, 0, 255),
    RC_TEST_ENTRY(abs,      true,  false, 0, 0),
    RC_TEST_ENTRY(adds,     true,  true,  0, 0),
    RC_TEST_ENTRY(avgt,     true,  true,  0, 0),
    RC_TEST_ENTRY(avgr,     true,  true,  0, 0),
    RC_TEST_ENTRY(avgz,     true,  true,  0, 0),
    RC_TEST_ENTRY(subs,     true,  true,  0, 0),
    RC_TEST_ENTRY(suba,     true,  true,  0, 0),
    RC_TEST_ENTRY(subht,    true,  true,  0, 0),
    RC_TEST_ENTRY(subhr,    true,  true,  0, 0),
    RC_TEST_ENTRY(cmpgt,    true,  true,  0, 0),
    RC_TEST_ENTRY(cmpge,    true,  true,  0, 0),
    RC_TEST_ENTRY(min,      true,  true,  0, 0),
    RC_TEST_ENTRY(max,      true,  true,  0, 0),
    RC_TEST_ENTRY(lerp,     true,  true,  1, 127),
    RC_TEST_ENTRY(lerpz,    true,  true,  1, 127),
    RC_TEST_ENTRY(lerpn,    true,  true,  1, 127),
    RC_TEST_ENTRY(cnt,      true,  false, 0, 0),
    RC_TEST_ENTRY(sum,      true,  false, 0, 0),
    RC_TEST_ENTRY(mac,      true,  true,  0, 0),
    RC_TEST_ENTRY(getmaskw, true,  false, 0, 0),
    RC_TEST_ENTRY(getmaskv, true,  false, 0, 0)
};


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
main(void)
{
    if (rc_test_backend) {
        char text[128];
        char line[128];

        sprintf(text, "%d-bit %s vector backend",
                8*RC_VEC_SIZE, rc_test_backend);
        memset(line, '-', strlen(text));
        line[strlen(text)] = '\0';

        printf("\n%s\n%s\n%s\n", line, text, line);

        return rc_test_vector() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rc_test_vector(void)
{
    int      num  = sizeof rc_test_table / sizeof *rc_test_table;
    uint8_t *buf  = malloc(4*RC_VEC_SIZE + 16);
    uint8_t *dst  = (uint8_t*)(((intptr_t)buf + 15) & ~15);
    uint8_t *src1 = &dst [RC_VEC_SIZE];
    uint8_t *src2 = &src1[RC_VEC_SIZE];
    uint8_t *ref  = &src2[RC_VEC_SIZE];
    int      k;

    for (k = 0; k < num; k++) {
        const struct rc_test_entry_st *entry = &rc_test_table[k];

        if (entry->test) {
            printf("%s\n", entry->name);

            if (entry->full) {
                if (!rc_test_vec_full(entry->test, entry->ref,
                                      dst, ref, src1,
                                      entry->op2 ? src2 : NULL,
                                      entry->arg))
                {
                    DBG("Failed\n");
                    return false;
                }
            }

            if (!rc_test_vec_rand(entry->test, entry->ref, dst, ref, src1,
                                  entry->op2 ? src2 : NULL, entry->arg))
            {
                DBG("Failed\n");
                rc_test_dump("src1 = ", src1);
                rc_test_dump("src2 = ", src2);
                rc_test_dump("dst  = ", dst);
                rc_test_dump("ref  = ", ref);
                return false;
            }
        }
    }

    free(buf);

    return true;
}

static bool
rc_test_exec(int (*test)(), int (*ref)(),
             uint8_t *dst_buf, uint8_t *ref_buf,
             const uint8_t *src1_buf, const uint8_t *src2_buf, int arg)
{
    int t, r, ret;

    memset(ref_buf, 0, RC_VEC_SIZE);
    memset(dst_buf, 0, RC_VEC_SIZE);

    r = (*ref)(dst_buf, src1_buf, src2_buf, arg);
    memcpy(ref_buf, dst_buf, RC_VEC_SIZE);

    t = (*test)(dst_buf, src1_buf, src2_buf, arg);

    if (t != r) {
        DBG("Got %d, should be %d\n", t, r);
        return false;
    }

    ret = memcmp(dst_buf, ref_buf, RC_VEC_SIZE) == 0;
    if (!ret) {
        rc_test_dump("src1 = ", src1_buf);
        if (src2_buf != NULL) {
            rc_test_dump("src2 = ", src2_buf);
        }
        rc_test_dump("dst  = ", dst_buf);
        rc_test_dump("ref  = ", ref_buf);
    }
    return ret;
}

static bool
rc_test_vec_full(int (*test)(), int (*ref)(),
                 uint8_t *dst_buf, uint8_t *ref_buf,
                 uint8_t *src1_buf, uint8_t *src2_buf,
                 const int *args)
{
    int op1;

    for (op1 = 0; op1 < 256; op1++) {
        memset(src1_buf, op1, RC_VEC_SIZE);

        if (src2_buf) {
            int op2;

            for (op2 = 0; op2 < 256; op2++) {
                int arg;

                memset(src2_buf, op2, RC_VEC_SIZE);

                for (arg = args[0]; arg <= args[1]; arg++) {
                    /* Double-operand */
                    if (!rc_test_exec(test, ref, dst_buf, ref_buf,
                                      src1_buf, src2_buf, arg))
                    {
                        DBG("op1=%d op2=%d arg=%d\n", op1, op2, arg);
                        return false;
                    }
                }
            }
        }
        else {
            int arg;
            for (arg = args[0]; arg <= args[1]; arg++) {
                /* Single-operand */
                if (!rc_test_exec(test, ref, dst_buf, ref_buf,
                                  src1_buf, NULL, arg))
                {
                    DBG("op=%d arg=%d\n", op1, arg);
                    return false;
                }
            }
        }
    }

    return true;
}

static bool
rc_test_vec_rand(int (*test)(), int (*ref)(),
                 uint8_t *dst_buf, uint8_t *ref_buf,
                 uint8_t *src1_buf, uint8_t *src2_buf,
                 const int *args)
{
    int k;

    for (k = 0; k < NTESTS; k++) {
        int arg;

        rc_test_init(src1_buf, RC_VEC_SIZE);
        if (src2_buf) {
            rc_test_init(src2_buf, RC_VEC_SIZE);
        }

        arg = rc_test_rand(args[0], args[1]);

        if (!rc_test_exec(test, ref, dst_buf, ref_buf,
                          src1_buf, src2_buf, arg))
        {
            DBG("arg=%d\n", arg);
            return false;
        }
    }

    return true;
}

static void
rc_test_dump(const char *text, const uint8_t *vec)
{
    int k;
    DBG("%s", text);
    for (k = 0; k < RC_VEC_SIZE; k++) {
        DBG(" %02x", vec[k]);
    }
    DBG("\n");
}

static int
rc_test_rand(int min, int max)
{
    return min + rand() % (max - min + 1);
}

static void
rc_test_init(uint8_t *buf, int len)
{
    int k;
    for (k = 0; k < len; k++) {
        buf[k] = rand();
    }
}
