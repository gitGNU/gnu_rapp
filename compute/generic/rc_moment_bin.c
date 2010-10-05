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
 *  @file   rc_moment_bin.c
 *  @brief  RAPP Compute layer binary image moments.
 *
 *  ALGORITHM
 *  ---------
 *  The binary image is handled in as large chunks as possible.
 *  If a word is all-cleared, we skip it. If a word is all-set,
 *  we update the moment sums by a fixed amount. Otherwise, each
 *  individual byte in the word is processed using lookup tables.
 */

#include "rc_impl_cfg.h"   /* Implementation config */
#include "rc_platform.h"   /* Endianness            */
#include "rc_stdbool.h"    /* C99 boolean           */
#include "rc_util.h"       /* RC_DIV_CEIL()         */
#include "rc_word.h"       /* Word operations       */
#include "rc_table.h"      /* Bitcount table        */
#include "rc_moment_bin.h" /* Binary moment API     */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Compute the sum of all values up to, but not including, 'n'.
 */
#define RC_MOMENT_SUM(n) \
    ((n)*((n) - 1) / 2)

/**
 *  Compute the sum of all squared values up to, but not including, 'n'.
 */
#define RC_MOMENT_SUM2(n) \
    ((n)*((n) - 1)*(2*(n) - 1) / 6)


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  8-bit position sum lookup table.
 *  table[i] = 0*BIT(i, 0) + 1*BIT(i, 1) + ... + 7*BIT(i, 7),
 *  where BIT(x, k) is the endian-dependent k:th bit of x.
 */
#ifdef RC_BIG_ENDIAN
static const uint8_t rc_moment_xsum_lut[256] = {
     0,  7,  6, 13,  5, 12, 11, 18,  4, 11, 10, 17,  9, 16, 15, 22,
     3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
     2,  9,  8, 15,  7, 14, 13, 20,  6, 13, 12, 19, 11, 18, 17, 24,
     5, 12, 11, 18, 10, 17, 16, 23,  9, 16, 15, 22, 14, 21, 20, 27,
     1,  8,  7, 14,  6, 13, 12, 19,  5, 12, 11, 18, 10, 17, 16, 23,
     4, 11, 10, 17,  9, 16, 15, 22,  8, 15, 14, 21, 13, 20, 19, 26,
     3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
     6, 13, 12, 19, 11, 18, 17, 24, 10, 17, 16, 23, 15, 22, 21, 28,
     0,  7,  6, 13,  5, 12, 11, 18,  4, 11, 10, 17,  9, 16, 15, 22,
     3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
     2,  9,  8, 15,  7, 14, 13, 20,  6, 13, 12, 19, 11, 18, 17, 24,
     5, 12, 11, 18, 10, 17, 16, 23,  9, 16, 15, 22, 14, 21, 20, 27,
     1,  8,  7, 14,  6, 13, 12, 19,  5, 12, 11, 18, 10, 17, 16, 23,
     4, 11, 10, 17,  9, 16, 15, 22,  8, 15, 14, 21, 13, 20, 19, 26,
     3, 10,  9, 16,  8, 15, 14, 21,  7, 14, 13, 20, 12, 19, 18, 25,
     6, 13, 12, 19, 11, 18, 17, 24, 10, 17, 16, 23, 15, 22, 21, 28
};
#else
static const uint8_t rc_moment_xsum_lut[256] = {
     0,  0,  1,  1,  2,  2,  3,  3,  3,  3,  4,  4,  5,  5,  6,  6,
     4,  4,  5,  5,  6,  6,  7,  7,  7,  7,  8,  8,  9,  9, 10, 10,
     5,  5,  6,  6,  7,  7,  8,  8,  8,  8,  9,  9, 10, 10, 11, 11,
     9,  9, 10, 10, 11, 11, 12, 12, 12, 12, 13, 13, 14, 14, 15, 15,
     6,  6,  7,  7,  8,  8,  9,  9,  9,  9, 10, 10, 11, 11, 12, 12,
    10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 17, 17,
    15, 15, 16, 16, 17, 17, 18, 18, 18, 18, 19, 19, 20, 20, 21, 21,
     7,  7,  8,  8,  9,  9, 10, 10, 10, 10, 11, 11, 12, 12, 13, 13,
    11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 15, 15, 16, 16, 17, 17,
    12, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 17, 17, 18, 18,
    16, 16, 17, 17, 18, 18, 19, 19, 19, 19, 20, 20, 21, 21, 22, 22,
    13, 13, 14, 14, 15, 15, 16, 16, 16, 16, 17, 17, 18, 18, 19, 19,
    17, 17, 18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 22, 22, 23, 23,
    18, 18, 19, 19, 20, 20, 21, 21, 21, 21, 22, 22, 23, 23, 24, 24,
    22, 22, 23, 23, 24, 24, 25, 25, 25, 25, 26, 26, 27, 27, 28, 28
};
#endif


/**
 *  8-bit squared position sum lookup table.
 *  table[i] = 0*0*BIT(i, 0) + 1*1*BIT(i, 1) + ... + 7*7*BIT(i, 7),
 *  where BIT(x, k) is the endian-dependent k:th bit of x.
 */
#ifdef RC_BIG_ENDIAN
static const uint8_t rc_moment_x2sum_lut[256] = {
      0, 49, 36, 85, 25, 74, 61,110, 16, 65, 52,101, 41, 90, 77,126,
      9, 58, 45, 94, 34, 83, 70,119, 25, 74, 61,110, 50, 99, 86,135,
      4, 53, 40, 89, 29, 78, 65,114, 20, 69, 56,105, 45, 94, 81,130,
     13, 62, 49, 98, 38, 87, 74,123, 29, 78, 65,114, 54,103, 90,139,
      1, 50, 37, 86, 26, 75, 62,111, 17, 66, 53,102, 42, 91, 78,127,
     10, 59, 46, 95, 35, 84, 71,120, 26, 75, 62,111, 51,100, 87,136,
      5, 54, 41, 90, 30, 79, 66,115, 21, 70, 57,106, 46, 95, 82,131,
     14, 63, 50, 99, 39, 88, 75,124, 30, 79, 66,115, 55,104, 91,140,
      0, 49, 36, 85, 25, 74, 61,110, 16, 65, 52,101, 41, 90, 77,126,
      9, 58, 45, 94, 34, 83, 70,119, 25, 74, 61,110, 50, 99, 86,135,
      4, 53, 40, 89, 29, 78, 65,114, 20, 69, 56,105, 45, 94, 81,130,
     13, 62, 49, 98, 38, 87, 74,123, 29, 78, 65,114, 54,103, 90,139,
      1, 50, 37, 86, 26, 75, 62,111, 17, 66, 53,102, 42, 91, 78,127,
     10, 59, 46, 95, 35, 84, 71,120, 26, 75, 62,111, 51,100, 87,136,
      5, 54, 41, 90, 30, 79, 66,115, 21, 70, 57,106, 46, 95, 82,131,
     14, 63, 50, 99, 39, 88, 75,124, 30, 79, 66,115, 55,104, 91,140
};
#else
static const uint8_t rc_moment_x2sum_lut[256] = {
      0,  0,  1,  1,  4,  4,  5,  5,  9,  9, 10, 10, 13, 13, 14, 14,
     16, 16, 17, 17, 20, 20, 21, 21, 25, 25, 26, 26, 29, 29, 30, 30,
     25, 25, 26, 26, 29, 29, 30, 30, 34, 34, 35, 35, 38, 38, 39, 39,
     41, 41, 42, 42, 45, 45, 46, 46, 50, 50, 51, 51, 54, 54, 55, 55,
     36, 36, 37, 37, 40, 40, 41, 41, 45, 45, 46, 46, 49, 49, 50, 50,
     52, 52, 53, 53, 56, 56, 57, 57, 61, 61, 62, 62, 65, 65, 66, 66,
     61, 61, 62, 62, 65, 65, 66, 66, 70, 70, 71, 71, 74, 74, 75, 75,
     77, 77, 78, 78, 81, 81, 82, 82, 86, 86, 87, 87, 90, 90, 91, 91,
     49, 49, 50, 50, 53, 53, 54, 54, 58, 58, 59, 59, 62, 62, 63, 63,
     65, 65, 66, 66, 69, 69, 70, 70, 74, 74, 75, 75, 78, 78, 79, 79,
     74, 74, 75, 75, 78, 78, 79, 79, 83, 83, 84, 84, 87, 87, 88, 88,
     90, 90, 91, 91, 94, 94, 95, 95, 99, 99,100,100,103,103,104,104,
     85, 85, 86, 86, 89, 89, 90, 90, 94, 94, 95, 95, 98, 98, 99, 99,
    101,101,102,102,105,105,106,106,110,110,111,111,114,114,115,115,
    110,110,111,111,114,114,115,115,119,119,120,120,123,123,124,124,
    126,126,127,127,130,130,131,131,135,135,136,136,139,139,140,140
};
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  First order binary image moment.
 */
void
rc_moment_order1_bin(const uint8_t *buf, int dim,
                     int width, int height, uint32_t mom[3])
{
    uint32_t nsum = 0;
    uint32_t xsum = 0;
    uint32_t ysum = 0;
    int      len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int      y;

    for (y = 0; y < height; y++) {
        uint32_t sn = 0;
        int      i  = y*dim;
        int      xpos, k;

        /* Process the row */
        for (k = 0, xpos = 0;
             k < len;
             k++, xpos += 8*RC_WORD_SIZE, i += RC_WORD_SIZE)
        {
            rc_word_t word = RC_WORD_LOAD(&buf[i]);

            if (RC_UNLIKELY(word)) {
                if (word == RC_WORD_ONE) {
                    /* Full - update */
                    sn   += 8*RC_WORD_SIZE;
                    xsum += 8*RC_WORD_SIZE*xpos +
                            RC_MOMENT_SUM(8*RC_WORD_SIZE);
                }
                else {
                    /* Process all bytes in the word */
                    int cnt = 0;
                    int xb;

                    for (xb = 0; xb < 8*RC_WORD_SIZE; xb += 8) {
                        unsigned byte = RC_WORD_EXTRACT(word, xb, 8);

                        if (byte) {
                            unsigned num = rc_table_bitcount[byte];
                            cnt  += num;
                            xsum += xb*num + rc_moment_xsum_lut[byte];
                        }
                    }
                    sn   += cnt;
                    xsum += cnt*xpos;
                }
            }
        }

        /* Update sums for this row */
        nsum += sn;
        ysum += sn*y;
    }

    mom[0] = nsum;
    mom[1] = xsum;
    mom[2] = ysum;
}

/**
 *  Second order binary image moment.
 */
void
rc_moment_order2_bin(const uint8_t *buf, int dim,
                     int width, int height, uintmax_t mom[6])
{
    uint32_t  nsum  = 0;
    uint32_t  xsum  = 0;
    uint32_t  ysum  = 0;
    uintmax_t x2sum = 0;
    uintmax_t y2sum = 0;
    uintmax_t xysum = 0;
    int       len   = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int       y, y2;

    for (y = 0, y2 = 0; y < height; y2 += 1 + 2*y, y++) {
        bool     upd = false;
        uint32_t sn  = 0;
        uint32_t sx  = 0;
        uint32_t sx2 = 0;
        int      i   = y*dim;
        int      x   = 0;
        int      x2  = 0;
        int      k;

        /* Handle all words */
        for (k = 0; k < len; k++, i += RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&buf[i]);

            if (RC_UNLIKELY(word)) {
                /* Mark for row-level updating */
                upd = true;

                if (word == RC_WORD_ONE) {
                    /* Handle the full word */
                    sn  += 8*RC_WORD_SIZE;
                    sx  += 8*RC_WORD_SIZE*x +
                           RC_MOMENT_SUM(8*RC_WORD_SIZE);
                    sx2 += 8*RC_WORD_SIZE*x2 +
                           2*RC_MOMENT_SUM (8*RC_WORD_SIZE)*x +
                             RC_MOMENT_SUM2(8*RC_WORD_SIZE);
                }
                else {
                    /* Process all bytes in the word */
                    int cnt = 0;
                    int dx  = 0;
                    int j;

                    for (j = 0; j < 8*RC_WORD_SIZE; j += 8) {
                        int byte = RC_WORD_EXTRACT(word, j, 8);

                        if (byte) {
                            int bn  = rc_table_bitcount[byte];
                            int bx  = rc_moment_xsum_lut[byte];
                            int bx2 = rc_moment_x2sum_lut[byte];
                            cnt += bn;
                            dx  += j*bn + bx;
                            sx2 += j*j*bn + 2*j*bx + bx2;
                        }
                    }
                    sn  += cnt;
                    sx  += cnt*x + dx;
                    sx2 += cnt*x2 + 2*dx*x;
                }
            }

            /* Update position variables */
            x2 += 8*RC_WORD_SIZE*(8*RC_WORD_SIZE + 2*x);
            x  += 8*RC_WORD_SIZE;
        }

        /* Update sums for this row */
        if (upd) {
            nsum  += sn;
            xsum  += sx;
            ysum  += sn*y;
            x2sum += sx2;
            y2sum += (uintmax_t)sn*(uintmax_t)y2;
            xysum += (uintmax_t)sx*(uintmax_t)y;
        }
    }

    mom[0] = nsum;
    mom[1] = xsum;
    mom[2] = ysum;
    mom[3] = x2sum;
    mom[4] = y2sum;
    mom[5] = xysum;
}
