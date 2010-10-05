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
 *  @file   rc_cond.c
 *  @brief  RAPP Compute layer conditional operations.
 */

#include <string.h>      /* memset()        */
#include "rc_platform.h" /* RC_UNLIKELY()   */
#include "rc_word.h"     /* Word operations */
#include "rc_table.h"    /* Lookup tables   */
#include "rc_cond.h"     /* Exported API    */

/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rc_cond_set_word(uint8_t *buf, rc_word_t word, uint32_t v32);

static void
rc_cond_copy_word(uint8_t *restrict dst,
                  const uint8_t *restrict src,
                  rc_word_t word);

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rc_cond_set_u8(uint8_t *restrict dst, int dst_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height, unsigned value)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    uint32_t  v32;
    int       y;

    /* Set the value to all fields */
    v32  = value & 0xff;
    v32 |= (v32 << 8) | (v32 << 16) | (v32 << 24);

    /* Process all rows */
    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                rc_cond_set_word(&dst[j], word, v32);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                rc_cond_set_word(&dst[j], word, v32);
            }
        }
    }
}

void
rc_cond_copy_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    /* Process all rows */
    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*dst_dim;
        int k = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0;
             x < len;
             x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE, k += 8*RC_WORD_SIZE)
        {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                rc_cond_copy_word(&dst[j], &src[k], word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                rc_cond_copy_word(&dst[j], &src[k], word);
            }
        }
    }
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rc_cond_set_word(uint8_t *buf, rc_word_t word, uint32_t v32)
{
    if (word == RC_WORD_ONE) {
        /* Set 8 full words */
        memset(buf, v32 & 0xff, 8*RC_WORD_SIZE);
    }
    else {
        /* Handle individual bytes */
        uint32_t *p32 = (uint32_t*)buf;
        int       k;
        for (k = 0;
             k < 8*RC_WORD_SIZE && word;
             k += 8, word = RC_WORD_SHL(word, 8))
        {
            rc_word_t byte = word & RC_WORD_INSERT(0xff, 0, 8);
            if (!byte) {
                /* Skip two 32-bit words */
                p32 += 2;
            }
            else if (byte == 0xff) {
                /* Set two 32-bit words */
                *p32++ = v32;
                *p32++ = v32;
            }
            else {
                /* Handle nibbles */
                unsigned  nibble;

                nibble = RC_WORD_EXTRACT(byte, 0, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *p32 = (*p32 & ~m32) | (v32 & m32);
                }
                p32++;

                nibble = RC_WORD_EXTRACT(byte, 4, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *p32 = (*p32 & ~m32) | (v32 & m32);
                }
                p32++;
            }
        }
    }
}

static void
rc_cond_copy_word(uint8_t *restrict dst,
                  const uint8_t *restrict src,
                  rc_word_t word)
{
    if (word == RC_WORD_ONE) {
        /* Copy 8 full words */
        memcpy(dst, src, 8*RC_WORD_SIZE);
    }
    else {
        /* Handle individual bytes */
        uint32_t       *d32 = (uint32_t*)dst;
        const uint32_t *s32 = (const uint32_t*)src;
        int             k;
        for (k = 0;
             k < 8*RC_WORD_SIZE && word;
             k += 8, word = RC_WORD_SHL(word, 8))
        {
            rc_word_t byte = word & RC_WORD_INSERT(0xff, 0, 8);
            if (!byte) {
                /* Skip two 32-bit words */
                d32 += 2;
                s32 += 2;
            }
            else if (byte == 0xff) {
                /* Copy two 32-bit words */
                *d32++ = *s32++;
                *d32++ = *s32++;
            }
            else {
                /* Handle nibbles */
                unsigned  nibble;

                nibble = RC_WORD_EXTRACT(byte, 0, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *d32 = (*d32 & ~m32) | (*s32 & m32);
                }
                d32++, s32++;

                nibble = RC_WORD_EXTRACT(byte, 4, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *d32 = (*d32 & ~m32) | (*s32 & m32);
                }
                d32++, s32++;
            }
        }
    }
}
