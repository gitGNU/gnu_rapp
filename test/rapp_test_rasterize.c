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
 *  @file   rapp_test_rasterize.c
 *  @brief  Correctness tests for chain code rasterization.
 */

#include <string.h>         /* strcmp()      */
#include <assert.h>         /* assert()      */
#include "rapp.h"           /* RAPP API      */
#include "rapp_test_util.h" /* Test utils    */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The position range.
 */
#define RAPP_TEST_RANGE 1024

/**
 *  Number of tests performed.
 */
#define RAPP_TEST_ITER 16


/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

/**
 *  Test data object.
 */
typedef struct rapp_test_data_st {
    int         xpos;
    int         ypos;
    const char *code;
} rapp_test_data_t;


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static const rapp_test_data_t rapp_test_4conn_star[] = {
    { 3,  0, "000"   },
    { 3,  1, "0030"  },
    { 3,  2, "03030" },
    { 3,  3, "303030"},
    { 2,  3, "30303" },
    { 1,  3, "3303"  },
    { 0,  3, "333"   },
    {-1,  3, "3323"  },
    {-2,  3, "32323" },
    {-3,  3, "232323"},
    {-3,  2, "23232" },
    {-3,  1, "2322"  },
    {-3,  0, "222"   },
    {-3, -1, "2122"  },
    {-3, -2, "21212" },
    {-3, -3, "212121"},
    {-2, -3, "12121" },
    {-1, -3, "1211"  },
    { 0, -3, "111"   },
    { 1, -3, "1011"  },
    { 2, -3, "10101" },
    { 3, -3, "101010"},
    { 3, -2, "01010" },
    { 3, -1, "0010"  }
};

static const rapp_test_data_t rapp_test_8conn_star[] = {
    { 3,  0, "000"},
    { 3,  1, "070"},
    { 3,  2, "707"},
    { 3,  3, "777"},
    { 2,  3, "767"},
    { 1,  3, "676"},
    { 0,  3, "666"},
    {-1,  3, "656"},
    {-2,  3, "565"},
    {-3,  3, "555"},
    {-3,  2, "545"},
    {-3,  1, "454"},
    {-3,  0, "444"},
    {-3, -1, "434"},
    {-3, -2, "343"},
    {-3, -3, "333"},
    {-2, -3, "323"},
    {-1, -3, "232"},
    { 0, -3, "222"},
    { 1, -3, "212"},
    { 2, -3, "121"},
    { 3, -3, "111"},
    { 3, -2, "101"},
    { 3, -1, "010"}
};

static const rapp_test_data_t rapp_test_4conn_sample[] = {
    { 20,   0, "00000000000000000000"},
    { 20, -10, "010010010010010010010010010010"},
    { 20, -20, "1010101010101010101010101010101010101010"},
    { 10, -20, "101101101101101101101101101101"},
    {  0, -20, "11111111111111111111"},
    {-10, -20, "121121121121121121121121121121"},
    {-20, -20, "2121212121212121212121212121212121212121"},
    {-20, -10, "212212212212212212212212212212"},
    {-20,   0, "22222222222222222222"},
    {-20,  10, "232232232232232232232232232232"},
    {-20,  20, "2323232323232323232323232323232323232323"},
    {-10,  20, "323323323323323323323323323323"},
    {  0,  20, "33333333333333333333"},
    { 10,  20, "303303303303303303303303303303"},
    { 20,  20, "3030303030303030303030303030303030303030"},
    { 20,  10, "030030030030030030030030030030"}
};

static const rapp_test_data_t rapp_test_8conn_sample[] = {
    { 20,   0, "00000000000000000000"},
    { 20, -10, "10101010101010101010"},
    { 20, -20, "11111111111111111111"},
    { 10, -20, "21212121212121212121"},
    {  0, -20, "22222222222222222222"},
    {-10, -20, "23232323232323232323"},
    {-20, -20, "33333333333333333333"},
    {-20, -10, "43434343434343434343"},
    {-20,   0, "44444444444444444444"},
    {-20,  10, "45454545454545454545"},
    {-20,  20, "55555555555555555555"},
    {-10,  20, "56565656565656565656"},
    {  0,  20, "66666666666666666666"},
    { 10,  20, "76767676767676767676"},
    { 20,  20, "77777777777777777777"},
    { 20,  10, "70707070707070707070"}
};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static bool
rapp_test_rasterize_driver(int (*func)(),
                           const rapp_test_data_t *data, int len);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

bool
rapp_test_rasterize_4conn(void)
{
    return rapp_test_rasterize_driver(&rapp_rasterize_4conn,
                                      rapp_test_4conn_star,
                                      sizeof rapp_test_4conn_star /
                                      sizeof rapp_test_4conn_star[0]) &&
           rapp_test_rasterize_driver(&rapp_rasterize_4conn,
                                      rapp_test_4conn_sample,
                                      sizeof rapp_test_4conn_sample /
                                      sizeof rapp_test_4conn_sample[0]);
}

bool
rapp_test_rasterize_8conn(void)
{
    return rapp_test_rasterize_driver(&rapp_rasterize_8conn,
                                      rapp_test_8conn_star,
                                      sizeof rapp_test_8conn_star /
                                      sizeof rapp_test_8conn_star[0]) &&
           rapp_test_rasterize_driver(&rapp_rasterize_8conn,
                                      rapp_test_8conn_sample,
                                      sizeof rapp_test_8conn_sample /
                                      sizeof rapp_test_8conn_sample[0]);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static bool
rapp_test_rasterize_driver(int (*func)(),
                           const rapp_test_data_t *data, int len)
{
    char code[42];
    int  k;

    for (k = 0; k < RAPP_TEST_ITER; k++) {
        int x0 = rapp_test_rand(-RAPP_TEST_RANGE, RAPP_TEST_RANGE);
        int y0 = rapp_test_rand(-RAPP_TEST_RANGE, RAPP_TEST_RANGE);
        int i;

        for (i = 0; i < len; i++) {
            int x1 = x0 + data[i].xpos;
            int y1 = y0 + data[i].ypos;
            int ret;

            assert(strlen(data[i].code) < sizeof code);

            /* Perform rasterization */
            ret = (*func)(code, x0, y0, x1, y1);

            /* Check the return value */
            if (ret < 0) {
                DBG("Got FAIL return value\n");
                return false;
            }

            /* Check the chain code length */
            if (ret != (int)strlen(code) ||
                ret != (int)strlen(data[i].code))
            {
                DBG("Invalid chain code length: %d (%d)\n",
                    ret, (int) strlen(data[i].code));
                return false;
            }

            /* Check the chain code data */
            if (strcmp(data[i].code, code) != 0) {
                DBG("Invalid chain code\n");
                DBG("ref=\n");
                DBG("%s\n", data[i].code);
                DBG("code=\n");
                DBG("%s\n", code);
                return false;
            }
        }
    }

    return true;
}
