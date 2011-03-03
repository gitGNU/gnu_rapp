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
 *  @file   rapp_check.c
 *  @brief  Setup and run RAPP unit tests within the Check framework.
 */

#include <stdlib.h>     /* Exit codes         */
#include <check.h>      /* Unit test fwk      */
#include "rc_stdbool.h" /* Portable stdbool.h */
#include "rapp.h"

/*
 * -------------------------------------------------------------
 *  Test case fixtures
 * -------------------------------------------------------------
 */

void setup(void)
{
    /* Initialize library */
    rapp_initialize();
}


void teardown(void)
{
    /* Terminate library */
    rapp_terminate();
}


/*
 * -------------------------------------------------------------
 *  Test case definition/creation macros
 * -------------------------------------------------------------
 */

#define RAPP_TEST(func)                    \
    extern bool rapp_test_ ## func(void);  \
    START_TEST(tcase_ ## func)             \
    {                                      \
        fail_unless(rapp_test_ ## func()); \
    }                                      \
    END_TEST

#define RAPP_TESTH(func, s) RAPP_TEST(func)

#include "rapp_tests.def"


/*
 * -------------------------------------------------------------
 *  Tester main function
 * -------------------------------------------------------------
 */

int main(void)
{
    SRunner *srunner;
    Suite   *suite   = NULL;
    TCase   *tcase;
    int      num;

    /* Create the Suite Runner object */
    srunner = srunner_create(NULL);

    /* Redefine the tests macros and generate the check tests */
#undef RAPP_TEST
#undef RAPP_TESTH

#define RAPP_TEST(func)                         \
    tcase = tcase_create("rapp_" #func);        \
    tcase_add_checked_fixture(tcase, setup, teardown); \
    tcase_add_test(tcase, tcase_ ## func);      \
    tcase_set_timeout(tcase, 30);               \
    suite_add_tcase(suite, tcase);

#define RAPP_TESTH(func, s)                     \
    suite = suite_create(s);                    \
    srunner_add_suite(srunner, suite);          \
    RAPP_TEST(func);

#include "rapp_tests.def"

    /* Run the tests */
    srunner_run_all(srunner, CK_ENV);
    num = srunner_ntests_failed(srunner);
    srunner_free(srunner);

    return num ? EXIT_FAILURE : EXIT_SUCCESS;
}
