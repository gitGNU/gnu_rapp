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
 *  @file   rapp_test_fill.c
 *  @brief  Correctness tests for connected-components seed fill.
 */

#include <string.h>         /* memcpy()       */
#include "rapp.h"           /* RAPP API       */
#include "rapp_ref_fill.h"  /* Reference API  */
#include "rapp_test_util.h" /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#define RAPP_TEST_ENTRY(pattern)               \
    {pattern[0],                               \
     sizeof pattern[0] / sizeof pattern[0][0], \
     sizeof pattern    / sizeof pattern[0]}


/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

typedef struct rapp_test_data_st {
    const char *pattern;
    int         width;
    int         height;
} rapp_test_data_t;


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static const char rapp_test_pat_3x3[11][11] =
    {"111  1  1  ",
     "111 111 1  ",
     "111  1  111",
     "           ",
     "  1 111 111",
     "  1 1     1",
     "111 1     1",
     "           ",
     "1 1 1     1",
     " 1   1   1 ",
     "1 1   1 1  "};

static const char rapp_test_pat_maze1[32][32] =
    {"11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "1111111111111111 111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1",
     "11111111111111111111111111111111",
     "1                               ",
     "11111111111111111111111111111111",
     "                               1"};

static const char rapp_test_pat_maze2[32][32] =
    {"111 111 111 111 111 111 111 111 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1   1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 111 111 111 111 111 111 111 11"};

static const char rapp_test_pat_maze3[32][32] =
    {"11    11    11    11    11    11",
     "  1  1  1  1  1  1  1  1  1  1 1",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "   1  1  1  1  1  1  1  1  1  1 ",
     "  1  1  1  1  1  1  1  1  1  1 1",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "   1  1  1  1  1  1  1  1  1  1 ",
     "  1  1  1  1  1  1  1  1  1  1 1",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     "1 1  1  1  1  1     1  1  1  1  ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "   1  1  1  1  1  1  1  1  1  1 ",
     "  1  1  1  1  1  1  1  1  1  1 1",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "   1  1  1  1  1  1  1  1  1  1 ",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "1   1  1  1  1  1  1  1  1  1  1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1   1  1  1  1  1  1  1  1  1  ",
     "    1  1  1  1  1  1  1  1  1  1",
     "   1  1  1  1  1  1  1  1  1  1 ",
     "    11    11    11    11    11  "};

static const char rapp_test_pat_maze4[32][32] =
    {"     11    11    11    11    11 ",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1     1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1 1",
     "1  1  1  1  1  1  1  1  1  1  1 ",
     " 11    11    11    11    11     "};

static const char rapp_test_pat_maze5[16][64] =
    {"1111111111111111111111111111111111111111111111111111111111111111",
     "1                                                               ",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "                                                               1",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "1                                                               ",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "                                                               1",
     "11111111111111111111111111111111 1111111111111111111111111111111",
     "1                                                               ",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "                                                               1",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "1                                                               ",
     "1111111111111111111111111111111111111111111111111111111111111111",
     "                                                               1"};

static const char rapp_test_pat_maze6[16][64] =
    {"111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1   1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ",
     "1 111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 11",};

static const char rapp_test_pat_maze7[16][64] =
    {"11    11    11    11    11    11    11    11    11    11    11  ",
     "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
     "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
     "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
     "   1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
     "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
     "  11    11    11    11    11    11    11    11    11    11    11"};

static const char rapp_test_pat_maze8[16][64] =
    {"     11    11    11    11    11    11    11    11    11    11  1",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
     "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
     "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     "   1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
     "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
     "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     "   1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
     " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
     "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
     "   11    11    11    11    11    11    11    11    11    11     "};


static const rapp_test_data_t rapp_test_data[] =
    {RAPP_TEST_ENTRY(rapp_test_pat_3x3),
     RAPP_TEST_ENTRY(rapp_test_pat_maze1),
     RAPP_TEST_ENTRY(rapp_test_pat_maze2),
     RAPP_TEST_ENTRY(rapp_test_pat_maze3),
     RAPP_TEST_ENTRY(rapp_test_pat_maze4),
     RAPP_TEST_ENTRY(rapp_test_pat_maze5),
     RAPP_TEST_ENTRY(rapp_test_pat_maze6),
     RAPP_TEST_ENTRY(rapp_test_pat_maze7),
     RAPP_TEST_ENTRY(rapp_test_pat_maze8)};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*test)(), void (*ref)());

static void
rapp_test_decode(uint8_t *buf, int dim, const rapp_test_data_t *data);

static bool
rapp_test_pattern(const uint8_t *map, int dim,
                  int width, int height,
                  int (*test)(), void (*ref)());


/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_fill_4conn_bin(void)
{
    return rapp_test_driver(&rapp_fill_4conn_bin,
                            &rapp_ref_fill_4conn_bin);
}

bool
rapp_test_fill_8conn_bin(void)
{
    return rapp_test_driver(&rapp_fill_8conn_bin,
                            &rapp_ref_fill_8conn_bin);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_driver(int (*test)(), void (*ref)())
{
    int  k;

    /* Run all tests for all patterns */
    for (k = 0; k < (int)(sizeof rapp_test_data /
                          sizeof rapp_test_data[0]); k++)
    {
        const rapp_test_data_t *data = &rapp_test_data[k];
        int                     dim  = rapp_align((data->width + 7)/8);
        uint8_t                *map  = rapp_malloc(dim*data->height, 0);

        /* Decode pattern data to a binary pattern image */
        rapp_test_decode(map, dim, data);

        /* Test the pattern */
        if (!rapp_test_pattern(map, dim, data->width,
                               data->height, test, ref))
        {
            rapp_free(map);
            return false;
        }

        /* Free buffers */
        rapp_free(map);
    }

    return true;
}

static void
rapp_test_decode(uint8_t *buf, int dim, const rapp_test_data_t *data)
{
    int x, y;

    memset(buf, 0, dim*data->height);

    for (y = 0; y < data->height; y++) {
        for (x = 0; x < data->width; x++) {
            if (data->pattern[y*data->width + x] == '1') {
                rapp_pixel_set_bin(buf, dim, 0, x, y, 1);
            }
        }
    }
}

static bool
rapp_test_pattern(const uint8_t *map, int dim,
                  int width, int height,
                  int (*test)(), void (*ref)())
{
    int      dst_dim = dim + rapp_alignment;
    uint8_t *dst_buf = rapp_malloc(dst_dim*height, 0);
    uint8_t *ref_buf = rapp_malloc(dst_dim*height, 0);
    bool     ok      = false;
    int      x, y;

    /* Verify that we get an overlap error for overlapping buffers */
    if (/* map == dst */
        (*test)(dst_buf, dst_dim, dst_buf, dim,
                width, height) != RAPP_ERR_OVERLAP
        /* map = far end of dst_buf */
        || (*test)(dst_buf, dst_dim,
                   dst_buf + dst_dim*(height - 1) +
                   rapp_align((width + 7) / 8) - rapp_alignment,
                   dim, width, height) != RAPP_ERR_OVERLAP
        /* map = before dst, but not long enough */
        || (*test)(dst_buf, dst_dim,
                   dst_buf - (dim*(height - 1) +
                              rapp_align((width + 7) / 8) - rapp_alignment),
                   dim, width, height) != RAPP_ERR_OVERLAP)
      {
        DBG("Overlap undetected\n");
        goto Done;
      }

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (rapp_pixel_get_bin(map, dim, 0, x, y)) {

                /* Run the test function */
                if ((*test)(dst_buf, dst_dim, map, dim,
                            width, height, x, y) < 0)
                {
                    DBG("Got FAIL return value\n");
                    goto Done;
                }

                /* Run the reference function */
                (*ref)(ref_buf, dst_dim, map, dim, width, height, x, y);

                /* Compare the result */
                if (!rapp_test_compare_bin(dst_buf, dst_dim,
                                           ref_buf, dst_dim, 0,
                                           width, height))
                {
                    DBG("Invalid result\n");
                    DBG("map=\n");
                    rapp_test_dump_bin(map, dim, 0, width, height);
                    DBG("dst=\n");
                    rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
                    DBG("ref=\n");
                    rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);
                    DBG("x=%d y=%d\n", x, y);
                    goto Done;
                }
            }
        }
    }

    ok = true;

Done:
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    return ok;
}
