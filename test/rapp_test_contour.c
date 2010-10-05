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
 *  @file   rapp_test_contour.c
 *  @brief  Correctness tests for contour chain code generation.
 */

#include <string.h>           /* memcpy()       */
#include <assert.h>           /* assert()       */
#include "rapp.h"             /* RAPP API       */
#include "rapp_ref_contour.h" /* Reference API  */
#include "rapp_test_util.h"   /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  An entry in the pattern table.
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

/**
 *  The pattern data type.
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

static const char rapp_test_full[4][64] = {
    "1111111111111111111111111111111111111111111111111111111111111111",
    "1111111111111111111111111111111111111111111111111111111111111111",
    "1111111111111111111111111111111111111111111111111111111111111111",
    "1111111111111111111111111111111111111111111111111111111111111111"
};

static const char rapp_test_maze1[32][32] = {
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
    "                               1"
};

static const char rapp_test_maze2[32][32] = {
    "111 111 111 111 111 111 111 111 ",
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
    "1 111 111 111 111 111 111 111 11"
};

static const char rapp_test_maze3[32][32] =
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

static const char rapp_test_maze4[32][32] = {
    "     11    11    11    11    11 ",
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
    " 11    11    11    11    11     "
};

static const char rapp_test_maze5[16][64] = {
    "1111111111111111111111111111111111111111111111111111111111111111",
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
    "                                                               1"
};

static const char rapp_test_maze6[16][64] = {
    "111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 ",
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
    "1 111 111 111 111 111 111 111 111 111 111 111 111 111 111 111 11"
};

static const char rapp_test_maze7[16][64] = {
    "11    11    11    11    11    11    11    11    11    11    11  ",
    "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
    " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
    "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
    "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
    " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
    "   1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
    "  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
    " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
    "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1",
    "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
    " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  ",
    "1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   ",
    "1 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 ",
    " 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1 1",
    "     11    11    11    11    11    11    11    11    11    11  1"
};

static const char rapp_test_maze8[16][64] = {
    "     11    11    11    11    11    11    11    11    11    11  1",
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
    "   11    11    11    11    11    11    11    11    11    11     "
};

/**
 *  The test pattern data table.
 */
static const rapp_test_data_t rapp_test_data[] = {
    RAPP_TEST_ENTRY(rapp_test_full),
    RAPP_TEST_ENTRY(rapp_test_maze1),
    RAPP_TEST_ENTRY(rapp_test_maze2),
    RAPP_TEST_ENTRY(rapp_test_maze3),
    RAPP_TEST_ENTRY(rapp_test_maze4),
    RAPP_TEST_ENTRY(rapp_test_maze5),
    RAPP_TEST_ENTRY(rapp_test_maze6),
    RAPP_TEST_ENTRY(rapp_test_maze7),
    RAPP_TEST_ENTRY(rapp_test_maze8)
};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_full_driver(int (*test)(), int (*ref)());

static bool
rapp_test_pattern_driver(int (*test)(), int (*ref)(),
                         const char *pat, int width, int height);

static void
rapp_test_get_pattern(uint8_t **buf, int *dim,
                      const char *pat, int width, int height);

/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_contour_4conn_bin(void)
{
    bool ok;
    int  k;

    ok = rapp_test_full_driver(&rapp_contour_4conn_bin,
                               &rapp_ref_contour_4conn_bin);

    for (k = 0; k < (int)(sizeof rapp_test_data /
                          sizeof rapp_test_data[0]) && ok; k++)
    {
        const rapp_test_data_t *data = &rapp_test_data[k];
        ok = rapp_test_pattern_driver(&rapp_contour_4conn_bin,
                                      &rapp_ref_contour_4conn_bin,
                                      data->pattern,
                                      data->width, data->height);
    }

    return ok;
}

bool
rapp_test_contour_8conn_bin(void)
{
    bool ok;
    int  k;

    ok = rapp_test_full_driver(&rapp_contour_8conn_bin,
                               &rapp_ref_contour_8conn_bin);

    for (k = 0; k < (int)(sizeof rapp_test_data /
                          sizeof rapp_test_data[0]) && ok; k++)
    {
        const rapp_test_data_t *data = &rapp_test_data[k];
        ok = rapp_test_pattern_driver(&rapp_contour_8conn_bin,
                                      &rapp_ref_contour_8conn_bin,
                                      data->pattern,
                                      data->width, data->height);
    }

    return ok;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_full_driver(int (*test)(), int (*ref)())
{
    const int first[4][2]   = {{1, 0}, { 1, 1}, { 0, 1}, {-1, 1}};
    const int second[8][2]  = {{2, 0}, { 2, 1}, { 2, 2}, { 1, 2},
                               {0, 2}, {-1, 2}, {-2, 2}, {-2, 1}};
    uint8_t  *pattern;
    int       dim;
    int       m1, m2;
    bool      ok = false;

    /* Set up the pattern buffer */
    dim     = rapp_alignment;
    pattern = rapp_malloc(5*dim, 0);
    memset(pattern, 0, 5*dim);
    rapp_pixel_set_bin(pattern, dim, 0, 3, 1, 1);

    /* Test all combinations of nearest neighbor pixels */
    for (m1 = 0; m1 < 16; m1++) {
        int i;
        for (i = 0; i < 4; i++) {
            int x = 3 + first[i][0];
            int y = 1 + first[i][1];
            int v = (m1 & (1 << i)) != 0;
            rapp_pixel_set_bin(pattern, dim, 0, x, y, v);
        }

        /* Test all combinations of next nearest neighbor pixels */
        for (m2 = 0; m2 < 256; m2++) {
            unsigned  rpos[2];
            unsigned  tpos[2];
            char      rbuf[18 + 1];
            char      tbuf[18 + 1];
            int       rlen;
            int       tlen;
            int       len;
            int       j;

            for (j = 0; j < 8; j++) {
                int x = 3 + second[j][0];
                int y = 1 + second[j][1];
                int v = (m2 & (1 << j)) != 0;
                rapp_pixel_set_bin(pattern, dim, 0, x, y, v);
            }

            /* Call the contour function with NULL contour */
            tlen = (*test)(tpos, NULL, 0, pattern, dim, 7, 5);

            /* Call reference function with NULL contour */
            rlen = (*ref)(rpos, NULL, 0, pattern, dim, 7, 5);

            /* Check error condition */
            if (tlen < 0) {
                DBG("Got FAIL return value\n");
                goto Done;
            }

            /* Check contour length */
            if (tlen != rlen) {
                DBG("Invalid contour length %d, should be %d\n", tlen, rlen);
                goto Done;
            }

            /* Save the computed contour length */
            len = rlen;
            assert(len < (int)(sizeof rbuf));
            assert(len < (int)(sizeof tbuf));

            /* Call the contour tracing function to test */
            tlen = (*test)(tpos, tbuf, len + 1, pattern, dim, 7, 5);

            /* Call reference function */
            rlen = (*ref)(rpos, rbuf, len + 1, pattern, dim, 7, 5);

            /* Check error condition */
            if (tlen < 0) {
                DBG("Got FAIL return value\n");
                goto Done;
            }

            /* Check contour length */
            if (tlen != len || tlen != rlen) {
                DBG("Invalid contour length %d, should be %d\n", tlen, rlen);
                goto Done;
            }

            /* Check the origin position */
            if (memcmp(rpos, tpos, sizeof rpos) != 0) {
                DBG("Invalid origin position\n");
                DBG("tpos=(%d,%d), rpos=(%d,%d)\n",
                    tpos[0], tpos[0], rpos[0], rpos[0]);
                goto Done;
            }

            /* Check NUL termination */
            if (tbuf[len] != '\0') {
                DBG("Contour not NUL terminated\n");
                goto Done;
            }

            /* Check contour buffer */
            if (memcmp(tbuf, rbuf, len + 1) != 0) {
                DBG("Invalid contour data\n");
                goto Done;
            }
        }
    }

    ok = true;

 Done:
    rapp_free(pattern);
    return ok;
}

static bool
rapp_test_pattern_driver(int (*test)(), int (*ref)(),
                         const char *pat, int width, int height)
{
    uint8_t  *src;
    char      tbuf[680 + 1];
    char      rbuf[680 + 1];
    unsigned  tpos[2];
    unsigned  rpos[2];
    int       tlen;
    int       rlen;
    int       len;
    int       dim;
    bool      ok = false;

    /* Create the test pattern buffer with one pixel padding */
    rapp_test_get_pattern(&src, &dim, pat, width, height);

    /* Run the contour function with NULL buffer */
    tlen = (*test)(tpos, NULL, 0, src, dim, width + 2, height + 2);

    /* Run the reference function with NULL buffer */
    rlen = (*ref)(rpos, NULL, 0, src, dim, width + 2, height + 2);

    /* Check error condition */
    if (tlen < 0) {
        DBG("Got FAIL return value\n");
        goto Done;
    }

    /* Check contour length */
    if (tlen != rlen) {
        DBG("Invalid contour length %d, should be %d)\n", tlen, rlen);
        goto Done;
    }

    /* Save the computed contour length */
    len = rlen;
    assert(len < (int)(sizeof rbuf));
    assert(len < (int)(sizeof tbuf));

    /* Verify that we get an overlap error for overlapping buffers */
    if (/* contour == src */
        (*test)(tpos, src, len + 1, src, dim,
                width + 2, height + 2) != RAPP_ERR_OVERLAP
        /* contour = far end of src */
        || (*test)(tpos, src + dim*(height + 2 - 1) +
                   rapp_align((width + 2 + 7) / 8) - rapp_alignment,
                   len + 1, src, dim,
                   width + 2, height + 2) != RAPP_ERR_OVERLAP
        /* contour = before src, but not long enough */
        || (*test)(tpos, src - (rapp_align(len + 1) -
                                rapp_alignment),
                   len + 1, src, dim,
                   width + 2, height + 2) != RAPP_ERR_OVERLAP)
      {
        DBG("Overlap undetected\n");
        goto Done;
      }

    /* Run the contour function */
    tlen = (*test)(tpos, tbuf, len + 1, src, dim, width + 2, height + 2);

    /* Run the reference function */
    rlen = (*ref)(rpos, rbuf, len + 1, src, dim, width + 2, height + 2);

    /* Check error condition */
    if (tlen < 0) {
        DBG("Got FAIL return value\n");
        goto Done;
    }

    /* Check contour length */
    if (tlen != len || tlen != rlen) {
        DBG("Invalid contour length %d, should be %d)\n", tlen, rlen);
        goto Done;
    }

    /* Check the origin position */
    if (memcmp(rpos, tpos, sizeof rpos) != 0) {
        DBG("Invalid origin position\n");
        goto Done;
    }

    /* Check NUL termination */
    if (tbuf[len] != '\0') {
        DBG("Contour not NUL terminated\n");
        goto Done;
    }

    /* Check contour buffer */
    if (memcmp(tbuf, rbuf, len + 1) != 0) {
        DBG("Invalid contour data\n");
        goto Done;
    }

    ok = true;

 Done:
    rapp_free(src);
    return ok;
}

static void
rapp_test_get_pattern(uint8_t **buf, int *dim,
                      const char *pat, int width, int height)
{
    int x, y, i;

    *dim = rapp_align((width + 2 + 7) / 8);
    *buf = rapp_malloc((height + 2)*(*dim), 0);
    memset(*buf, 0, (height + 2)*(*dim));

    for (y = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            rapp_pixel_set_bin(*buf, *dim, 0, x + 1, y + 1, pat[i] == '1');
        }
    }
}
