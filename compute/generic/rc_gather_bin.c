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
 *  @file   rc_gather_bin.c
 *  @brief  RAPP Compute layer binary gather.
 */

#include "rc_platform.h"   /* RC_UNLIKELY()   */
#include "rc_word.h"       /* Word operations */
#include "rc_gather_bin.h" /* Exported API    */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  A binary gather iteration.
 */
#define RC_GATHER_ITER(pack, src, mask, acc1, acc2, fill, pos) \
do {                                                           \
    if (RC_UNLIKELY(mask)) {                                   \
        rc_word_t sw_  = RC_WORD_LOAD(src);                    \
        int       cnt_ = 8*RC_WORD_SIZE;                       \
        if (RC_UNLIKELY((fill) >= 8*RC_WORD_SIZE)) {           \
            /* Accumulator full - write to the pack buffer */  \
            RC_WORD_STORE(pack, acc1);                         \
            (acc1)  = (acc2);                                  \
            (pack) +=   RC_WORD_SIZE;                          \
            (fill) -= 8*RC_WORD_SIZE;                          \
            (pos)  += 8*RC_WORD_SIZE;                          \
        }                                                      \
        if ((mask) != RC_WORD_ONE) {                           \
            /* Gather an inhomogenous mask */                  \
            RC_WORD_BITCOUNT(cnt_, mask);                      \
            sw_ = rc_gather_word(sw_, mask);                   \
        }                                                      \
        (acc1)  = ((acc1) & ~RC_WORD_SHR(RC_WORD_ONE, fill)) | \
                  RC_WORD_SHR(sw_, fill);                      \
        (acc2)  = RC_WORD_SHL(sw_, 8*RC_WORD_SIZE - (fill));   \
        (fill) += cnt_;                                        \
    }                                                          \
} while (0)


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static RC_NOINLINE rc_word_t
rc_gather_word(rc_word_t word, rc_word_t mask);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary pixel gather.
 */
int
rc_gather_bin(uint8_t *restrict pack,
              const uint8_t *restrict src, int src_dim,
              const uint8_t *restrict map, int map_dim,
              int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    rc_word_t acc1 = 0;
    rc_word_t acc2 = 0;
    int       fill = 0;
    int       pos  = 0;
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            RC_GATHER_ITER(pack, &src[j], word, acc1, acc2, fill, pos);
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            RC_GATHER_ITER(pack, &src[j], word, acc1, acc2, fill, pos);
        }
    }

    /* Store any remaining accumulator pixels */
    if (fill > 0) {
        RC_WORD_STORE(pack, acc1);
        if (fill > 8*RC_WORD_SIZE) {
            RC_WORD_STORE(&pack[RC_WORD_SIZE], acc2);
        }
        pos += fill;
    }

    return pos;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

/**
 *  Gather a word.
 */
static RC_NOINLINE rc_word_t
rc_gather_word(rc_word_t word, rc_word_t mask)
{
    rc_word_t dstw = 0;
    int       pos  = 0;
    int       k;

    /* Process all bytes */
    for (k = 0;
         k < 8*RC_WORD_SIZE && mask;
         k += 8, mask = RC_WORD_SHL(mask, 8))
    {
        if ((mask & RC_WORD_INSERT(0xff, 0, 8)) ==
            RC_WORD_INSERT(0xff, 0, 8))
        {
            /* Handle full bytes */
            unsigned byte = RC_WORD_EXTRACT(word, k, 8);
            dstw |= RC_WORD_INSERT(byte, pos, 8);
            pos  += 8;
        }
        else {
            /* Process all bits in a partial byte */
            int i;
            for (i = 0; i < 8; i++) {
                if (RC_WORD_EXTRACT(mask, i, 1)) {
                    /* Copy bit */
                    unsigned bit = RC_WORD_EXTRACT(word, k + i, 1);
                    dstw |= RC_WORD_INSERT(bit, pos, 1);
                    pos++;
                }
            }
        }
    }

    return dstw;
}
