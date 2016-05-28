/*  Copyright (C) 2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_test_malloc.c
 *  @brief  Test allocation and alignment.
 */

#include "rapp.h"            /* RAPP API      */
#include "rapp_test_util.h"  /* Test utils    */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

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
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_validate_alignment(void)
{
    bool result = true;

    if (rapp_align(1) != rapp_alignment) {
        result = false;
        printf("Alignment error %d %d\n",
               (int) rapp_align(1), (int) rapp_alignment);
        goto Fail;
    }

Fail:
    return result;
}

bool
rapp_test_validate_buffer(void)
{
    bool result = true;
    uint8_t *aligned_buf = NULL;
    uint8_t *misaligned_buf = NULL;

    int aligned_dim = rapp_align(RAPP_TEST_WIDTH);
    int misaligned_dim = aligned_dim - 1;

    aligned_buf = rapp_malloc(aligned_dim * RAPP_TEST_HEIGHT, 0);
    misaligned_buf = malloc(misaligned_dim * RAPP_TEST_HEIGHT);

    if (rapp_validate_buffer(aligned_buf,
                             aligned_dim,
                             RAPP_TEST_WIDTH,
                             RAPP_TEST_HEIGHT) != 1) {
        result = false;
        printf("rapp_validate_buffer (aligned) error\n");
        goto Fail;
    }

    /**
     *  We can't know that misaligned_buf is actually misaligned, but we
     *  can force it.
     */
    if (rapp_validate_buffer(misaligned_buf + 1,
                             misaligned_dim,
                             RAPP_TEST_WIDTH,
                             RAPP_TEST_HEIGHT - 1) != 0) {
        result = false;
        printf("rapp_validate_buffer (misaligned) error\n");
        goto Fail;
    }

    /**
     *  Check that the aligned buffer is flagged as misaliged if we add
     *  a slightly misaligned offset.
     */
    if (rapp_validate_buffer(aligned_buf + rapp_alignment / 2,
                             misaligned_dim,
                             RAPP_TEST_WIDTH,
                             RAPP_TEST_HEIGHT - 1) != 0) {
        result = false;
        printf("rapp_validate_buffer (misaligned) error\n");
        goto Fail;
    }

    /* Temporary terminate library. */
    rapp_terminate();
    if (rapp_validate_buffer(aligned_buf,
                             aligned_dim,
                             RAPP_TEST_WIDTH,
                             RAPP_TEST_HEIGHT) != -1) {
        result = false;
        printf("rapp_validate_buffer (initialization) error\n");
    }

    /* Re-initialize library. */
    rapp_initialize();

    if (result == false) {
        goto Fail;
    }

    /* Run again to verify initialization works. */
    if (rapp_align(1) != rapp_alignment) {
        result = false;
        printf("Alignment error %d %d\n",
               (int) rapp_align(1), (int) rapp_alignment);
        goto Fail;
    }

Fail:
    rapp_free(aligned_buf);
    free(misaligned_buf);
    return result;
}
