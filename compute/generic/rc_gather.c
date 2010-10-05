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
 *  @file   rc_gather.c
 *  @brief  RAPP Compute layer 8-bit gather, generic implementation.
 */

#include <string.h>      /* memcpy()        */
#include "rc_platform.h" /* RC_UNLIKELY()   */
#include "rc_word.h"     /* Word operations */
#include "rc_gather.h"   /* Exported API    */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rc_gather_word_row1(uint8_t *restrict dst,
                    const uint8_t *restrict src,
                    int pos, rc_word_t word);

static int
rc_gather_word_row2(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int pos, rc_word_t word);

static int
rc_gather_word_row3(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int pos, rc_word_t word);

static int
rc_gather_word_gen(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int pos, rc_word_t word, int rows);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  8-bit gather, one row.
 */
int
rc_gather_row1_u8(uint8_t *restrict pack,
                  const uint8_t *restrict src, int src_dim,
                  const uint8_t *restrict map, int map_dim,
                  int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    int       pos  = 0;                        /* Pack buffer position  */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row1(pack, &src[j], pos, word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row1(pack, &src[j], pos, word);
            }
        }
    }

    return pos;
}


/**
 *  8-bit gather, two rows.
 */
int
rc_gather_row2_u8(uint8_t *restrict pack, int pack_dim,
                  const uint8_t *restrict src, int src_dim,
                  const uint8_t *restrict map, int map_dim,
                  int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    int       pos  = 0;                        /* Pack buffer position  */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row2(pack, pack_dim,
                                          &src[j], src_dim, pos, word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row2(pack, pack_dim,
                                          &src[j], src_dim, pos, word);
            }
        }
    }

    return pos;
}


/**
 *  8-bit gather, three rows.
 */
int
rc_gather_row3_u8(uint8_t *restrict pack, int pack_dim,
                  const uint8_t *restrict src, int src_dim,
                  const uint8_t *restrict map, int map_dim,
                  int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    int       pos  = 0;                        /* Pack buffer position  */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row3(pack, pack_dim,
                                          &src[j], src_dim, pos, word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_row3(pack, pack_dim,
                                          &src[j], src_dim, pos, word);
            }
        }
    }

    return pos;
}


/**
 *  8-bit gather, any number of rows.
 */
int
rc_gather_gen_u8(uint8_t *restrict pack, int pack_dim,
                 const uint8_t *restrict src, int src_dim,
                 const uint8_t *restrict map, int map_dim,
                 int width, int height, int rows)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    int       pos  = 0;                        /* Pack buffer position  */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_gen(pack, pack_dim,
                                         &src[j], src_dim, pos, word, rows);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                pos = rc_gather_word_gen(pack, pack_dim,
                                         &src[j], src_dim, pos, word, rows);
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
rc_gather_word_row1(uint8_t *restrict dst,
                    const uint8_t *restrict src,
                    int pos, rc_word_t word)
{
    uint8_t *dptr = &dst[pos];

    if (word == RC_WORD_ONE) {
        /* Copy 8 words */
        memcpy(dptr, src, 8*RC_WORD_SIZE);
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
                src += 8;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy 8 pixels */
                int i;
                for (i = 0; i < 8; i++) {
                    *dptr++ = *src++;
                }
            }
            else {
                /* Handle individual pixels */
                int i;
                for (i = 0; i < 8; i++) {
                    if (RC_WORD_EXTRACT(byte, i, 1)) {
                        *dptr++ = *src;
                    }
                    src++;
                }
            }
        }

        return dptr - dst;
    }
}

static int
rc_gather_word_row2(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int pos, rc_word_t word)
{
    uint8_t       *dp1 = &dst[pos];
    uint8_t       *dp2 = &dp1[dst_dim];
    const uint8_t *sp1 =  src;
    const uint8_t *sp2 = &src[src_dim];

    if (word == RC_WORD_ONE) {
        /* Copy 8 words */
        memcpy(dp1, sp1, 8*RC_WORD_SIZE);
        memcpy(dp2, sp2, 8*RC_WORD_SIZE);
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
                sp1 += 8;
                sp2 += 8;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy 8 pixels */
                int b;
                for (b = 0; b < 8; b++) {
                    *dp1++ = *sp1++;
                    *dp2++ = *sp2++;
                }
            }
            else {
                /* Handle individual pixels */
                int b;
                for (b = 0; b < 8; b++) {
                    if (RC_WORD_EXTRACT(byte, b, 1)) {
                        *dp1++ = *sp1;
                        *dp2++ = *sp2;
                    }
                    sp1++, sp2++;
                }
            }
        }

        return dp1 - dst;
    }
}

static int
rc_gather_word_row3(uint8_t *restrict dst, int dst_dim,
                    const uint8_t *restrict src, int src_dim,
                    int pos, rc_word_t word)
{
    uint8_t       *dp1 = &dst[pos];
    uint8_t       *dp2 = &dp1[dst_dim];
    uint8_t       *dp3 = &dp2[dst_dim];
    const uint8_t *sp1 =  src;
    const uint8_t *sp2 = &sp1[src_dim];
    const uint8_t *sp3 = &sp2[src_dim];

    if (word == RC_WORD_ONE) {
        /* Copy 8 words */
        memcpy(dp1, sp1, 8*RC_WORD_SIZE);
        memcpy(dp2, sp2, 8*RC_WORD_SIZE);
        memcpy(dp3, sp3, 8*RC_WORD_SIZE);
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
                sp1 += 8;
                sp2 += 8;
                sp3 += 8;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy 8 pixels */
                int b;
                for (b = 0; b < 8; b++) {
                    *dp1++ = *sp1++;
                    *dp2++ = *sp2++;
                    *dp3++ = *sp3++;
                }
            }
            else {
                /* Handle individual pixels */
                int b;
                for (b = 0; b < 8; b++) {
                    if (RC_WORD_EXTRACT(byte, b, 1)) {
                        *dp1++ = *sp1;
                        *dp2++ = *sp2;
                        *dp3++ = *sp3;
                    }
                    sp1++, sp2++, sp3++;
                }
            }
        }

        return dp1 - dst;
    }
}

static int
rc_gather_word_gen(uint8_t *restrict dst, int dst_dim,
                   const uint8_t *restrict src, int src_dim,
                   int pos, rc_word_t word, int rows)
{
    uint8_t *dptr = &dst[pos];

    if (word == RC_WORD_ONE) {
        /* Copy 8 words */
        int r, i, j;
        for (r = 0, i = 0, j = 0;
             r < rows;
             r++, i += src_dim, j += dst_dim)
        {
            memcpy(&dptr[j], &src[i], 8*RC_WORD_SIZE);
        }
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
                src += 8;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy 8 pixels */
                int i, j, r;
                for (r = 0, i = 0, j = 0;
                     r < rows;
                     r++, i += src_dim, j += dst_dim)
                {
                    uint8_t       *dp = &dptr[j];
                    const uint8_t *sp = &src[i];
                    int            b;
                    for (b = 0; b < 8; b++) {
                        *dp++ = *sp++;
                    }
                }
                dptr += 8;
                src  += 8;
            }
            else {
                /* Handle individual pixels */
                int b;
                for (b = 0; b < 8; b++) {
                    if (RC_WORD_EXTRACT(byte, b, 1)) {
                        int i, j, r;
                        for (r = 0, i = 0, j = 0;
                             r < rows;
                             r++, i += src_dim, j += dst_dim)
                        {
                            dptr[j] = src[i];
                        }
                        dptr++;
                    }
                    src++;
                }
            }
        }

        return dptr - dst;
    }
}
