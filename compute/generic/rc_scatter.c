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

/*
 *  @file   rc_scatter.c
 *  @brief  RAPP Compute layer 8-bit scatter, generic implementation.
 */

#include <string.h>      /* memcpy()        */
#include "rc_platform.h" /* RC_UNLIKELY()   */
#include "rc_word.h"     /* Word operations */
#include "rc_scatter.h"  /* Exported API    */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rc_scatter_word(uint8_t *restrict dst,
                const uint8_t *restrict src,
                int pos, rc_word_t word);

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
rc_scatter_u8(uint8_t *restrict dst, int dst_dim,
              const uint8_t *restrict map, int map_dim,
              const uint8_t *restrict pack, int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    int       pos  = 0;                        /* Pack buffer position  */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                pos = rc_scatter_word(&dst[j], pack, pos, word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                pos = rc_scatter_word(&dst[j], pack, pos, word);
            }
        }
    }

    return pos;
}

/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rc_scatter_word(uint8_t *restrict dst,
                const uint8_t *restrict src,
                int pos, rc_word_t word)
{
    const uint8_t *sptr = &src[pos];

    if (word == RC_WORD_ONE) {
        /* Copy 8 words */
        memcpy(dst, sptr, 8*RC_WORD_SIZE);
        return pos + 8*RC_WORD_SIZE;
    }
    else {
        /* Handle individual bytes */
        int k;
        for (k = 0;
             k < RC_WORD_SIZE && word;
             k++, word = RC_WORD_SHL(word, 8))
        {
            rc_word_t byte = word & RC_WORD_INSERT(0xff, 0, 8);

            if (byte == 0) {
                /* Skip 8 pixels */
                dst += 8;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy 8 pixels */
                int i;
                for (i = 0; i < 8; i++) {
                    *dst++ = *sptr++;
                }
            }
            else {
                /* Handle individual pixels */
                int i;
                for (i = 0; i < 8; i++) {
                    if (RC_WORD_EXTRACT(byte, i, 1)) {
                        *dst = *sptr++;
                    }
                    dst++;
                }
            }
        }

        return sptr - src;
    }
}
