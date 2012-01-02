/*  Copyright (C) 2005-2012, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test.c
 *  @brief  Run RAPP correctness tests.
 */

#include <stdlib.h>     /* Exit codes         */
#include <stdio.h>      /* printf()           */
#include "rc_stdbool.h" /* Portable stdbool.h */
#include "rapp.h"
#include "rapp_test_util.h"

/*
 * -------------------------------------------------------------
 *  Test case definition/creation macros
 * -------------------------------------------------------------
 */

#define RAPP_TEST(func) \
    extern bool rapp_test_ ## func(void);

#define RAPP_TESTH(func, s) \
    extern bool rapp_test_ ## func(void);

#include "rapp_tests.def"


/*
 * -------------------------------------------------------------
 *  Test case table
 * -------------------------------------------------------------
 */

static const struct {
    const char *title;
    const char *name;
    bool      (*test)(void);
} rapp_test_table[] = {

#undef RAPP_TEST
#undef RAPP_TESTH

#define RAPP_TEST(func) \
    {NULL, "rapp_" #func, &rapp_test_ ## func},

#define RAPP_TESTH(func, s) \
    {s, "rapp_" #func, &rapp_test_ ## func},

#include "rapp_tests.def"
    {NULL, NULL, NULL} /* Terminating entry */
};


/*
 * -------------------------------------------------------------
 *  Tester main function
 * -------------------------------------------------------------
 */

int main(void)
{
    int len;
    int k;
    int fails = 0;

#ifdef RAPP_LOGFILE
    if (setenv("RAPP_LOGFILE", RAPP_LOGFILE, 1) != 0) {
        perror("setenv");
        exit(1);
    }
#endif

    /* Initialize library */
    rapp_initialize();

    len = sizeof rapp_test_table / sizeof rapp_test_table[0] - 1;

    /* Run all tests */
    for (k = 0; k < len; k++)
    {
        const char *title = rapp_test_table[k].title;

        /* Print suite title */
        if (title) {
            printf("\n%s\n", title);
        }

        /* Make pseudo-random element of tests independent of earlier tests */
        rapp_test_srand(1);

        /* Print function name */
        printf("  %s\n", rapp_test_table[k].name);

        /* Run the test */
        if (!rapp_test_table[k].test()) {
            printf("FAILED: %s\n", rapp_test_table[k].name);
            fails++;
        }
    }

    if (fails != 0) {
        printf("%d out of %d tests failed.\n", fails, len);
        return EXIT_FAILURE;
    }

    printf("All %d tests passed.\n", len);

    /* Terminate library */
    rapp_terminate();

    return EXIT_SUCCESS;
}
