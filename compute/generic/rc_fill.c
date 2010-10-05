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
 *  @file   rc_fill.c
 *  @brief  RAPP Compute layer connected-components seed fill.
 *
 *  ALGORITHM
 *  ---------
 *  The data is processed row-by-row, one word at a time. First, we
 *  let the neighborhood pixels bleed into the current word. This is done
 *  by bit shifts and bitwise OR operations on the neighboring words.
 *  The word is then masked by the map word using bitwise AND. Then we
 *  widen the word by shifting it one position to the left and to the right,
 *  and ORing the results with the unshifted word. As with the initial
 *  step, we then AND the word with the map word. This is iterated until
 *  convergence.
 */

#include "rc_util.h" /* MIN(), MAX()          */
#include "rc_word.h" /* Word operations       */
#include "rc_fill.h" /* Seed fill API         */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  4-connectivity word fill.
 *  @param word    The word to process.
 *  @param left    Left word.
 *  @param right   Right word.
 *  @param prev    Previous row word (up or down).
 *  @param map     Map mask word.
 *  @param change  Change indicator to increment.
 */
#define RC_FILL_4CONN(word, left, right, prev, map, change)                  \
do {                                                                         \
    rc_word_t map_  = (map);                       /* Mask map word       */ \
    rc_word_t word_ = (word) & map_;               /* Fill word           */ \
    if (word_ != map_) {                                                     \
        rc_word_t w1_;                             /* Fill word temp      */ \
        rc_word_t w2_ = (RC_WORD_SHL(left,  8*RC_WORD_SIZE - 1) |            \
                         RC_WORD_SHR(right, 8*RC_WORD_SIZE - 1) |            \
                         (prev) | word_) & map_;   /* New fill word       */ \
        int       ch_ = w2_ != word_;              /* Change counter      */ \
        do {                                                                 \
            ch_++;                                 /* One more change     */ \
            w1_ = w2_;                             /* Save temp word      */ \
            w2_ = ((w1_ << 1) | w1_ | (w1_ >> 1)) & map_; /* Widen word   */ \
        } while (w1_ != w2_);                      /* Until not changing  */ \
        (word)   |= w2_;                           /* Save fill word      */ \
        (change) += ch_ - 1;                       /* Update change count */ \
    }                                                                        \
} while (0)

/**
 *  8-connectivity word fill.
 *  @param word    The word to process.
 *  @param wl      Left word.
 *  @param wr      Right word.
 *  @param wp      Previous row word (up or down).
 *  @param wpl     Previous row left word.
 *  @param wpr     Previous row right word.
 *  @param map     Map mask word.
 *  @param change  Change indicator to update.
 */
#define RC_FILL_8CONN(word, wl, wr, wp, wpl, wpr, map, change)              \
do {                                                                        \
    rc_word_t map_  = (map);                                                \
    rc_word_t word_ = (word) & map_;              /* Fill word           */ \
    if (word_ != map_) {                                                    \
        rc_word_t wp_ = (wp);                     /* Previous row word   */ \
        rc_word_t w1_;                            /* Fill word temp      */ \
        rc_word_t w2_ = (RC_WORD_SHL(wl,  8*RC_WORD_SIZE - 1) |             \
                         RC_WORD_SHL(wpl, 8*RC_WORD_SIZE - 1) |             \
                         RC_WORD_SHR(wr,  8*RC_WORD_SIZE - 1) |             \
                         RC_WORD_SHR(wpr, 8*RC_WORD_SIZE - 1) |             \
                         (wp_ << 1) |                                       \
                         (wp_ >> 1) |                                       \
                          wp_ | word_) & map_;    /* New fill word       */ \
        int        ch_ = w2_ != word_;            /* Change counter      */ \
        do {                                                                \
            ch_++;                                /* One more change     */ \
            w1_ = w2_;                            /* Save temp word      */ \
            w2_ = ((w1_ << 1) | w1_ | (w1_ >> 1)) & map_; /* Widen word  */ \
        } while (w1_ != w2_);                     /* Until no change     */ \
        (word)   |= w2_;                          /* Save fill word      */ \
        (change) += ch_ - 1;                      /* Update change count */ \
    }                                                                       \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  4-connectivity forward fill iteration.
 */
int
rc_fill_4conn_fwd_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict map, int map_dim,
                      int width, int height)
{
    int len    = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int change = 0;
    int y;

    for (y = 0; y < height; y++) {
        int       x;
        int       i    = y*map_dim;
        int       j1   = y*dst_dim;
        int       j2   = MAX(y - 1, 0)*dst_dim;
        rc_word_t row  = 0;
        rc_word_t left = 0;

        for (x = 0;
             x < len;
             x++, i += RC_WORD_SIZE, j1 += RC_WORD_SIZE, j2 += RC_WORD_SIZE)
        {
            rc_word_t word, up, mask;

            /* Load words */
            word = RC_WORD_LOAD(&dst[j1]);
            up   = RC_WORD_LOAD(&dst[j2]);
            mask = RC_WORD_LOAD(&map[i ]);

            /* Fill word and store */
            RC_FILL_4CONN(word, left, 0, up, mask, change);
            RC_WORD_STORE(&dst[j1], word);

            /* Update sliding variables */
            left = word;
            row |= word;
        }

        if (!row) {
            /* Empty row - break */
            break;
        }
    }

    return change ? y : 0;
}


/**
 *  4-connectivity reversed fill iteration.
 */
int
rc_fill_4conn_rev_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict map, int map_dim,
                      int width, int height)
{
    int len    = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int end    = (len - 1)*RC_WORD_SIZE;
    int change = 0;
    int y;

    for (y = height - 1; y >= 0; y--) {
        int       x;
        int       i     = y*map_dim + end;
        int       j1    = y*dst_dim + end;
        int       j2    = MIN(y + 1, height - 1)*dst_dim + end;
        rc_word_t row   = 0;
        rc_word_t right = 0;

        for (x = len - 1;
             x >= 0;
             x--, i -= RC_WORD_SIZE, j1 -= RC_WORD_SIZE, j2 -= RC_WORD_SIZE)
        {
            rc_word_t word, down, mask;

            /* Load words */
            word = RC_WORD_LOAD(&dst[j1]);
            down = RC_WORD_LOAD(&dst[j2]);
            mask = RC_WORD_LOAD(&map[i ]);

            /* Fill word and store */
            RC_FILL_4CONN(word, 0, right, down, mask, change);
            RC_WORD_STORE(&dst[j1], word);

            /* Update sliding variables */
            right = word;
            row  |= word;
        }

        if (!row) {
            /* Empty row - break */
            break;
        }
    }

    return change ? height - y - 1 : 0;
}


/**
 *  8-connectivity forward fill iteration.
 */
int
rc_fill_8conn_fwd_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict map, int map_dim,
                      int width, int height)
{
    int len    = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int change = 0;
    int y;

    for (y = 0; y < height; y++) {
        int       x;
        int       i      = y*map_dim;
        int       j1     = y*dst_dim;
        int       j2     = MAX(y - 1, 0)*dst_dim;
        rc_word_t row    = 0;                      /* Change accumulator   */
        rc_word_t left   = 0;                      /* Sliding left word    */
        rc_word_t up     = RC_WORD_LOAD(&dst[j2]); /* Sliding upper word   */
        rc_word_t upleft = 0;                      /* Sliding up-left word */

        for (x = 0, j2 += RC_WORD_SIZE;
             x < len;
             x++, i += RC_WORD_SIZE, j1 += RC_WORD_SIZE, j2 += RC_WORD_SIZE)
        {
            rc_word_t word, upright, mask;

            /* Load new words */
            mask    = RC_WORD_LOAD(&map[i ]);
            word    = RC_WORD_LOAD(&dst[j1]);
            upright = (x < len - 1) ? RC_WORD_LOAD(&dst[j2]) : 0;

            /* Fill word and store */
            RC_FILL_8CONN(word, left, 0, up, upleft, upright, mask, change);
            RC_WORD_STORE(&dst[j1], word);

            /* Update sliding variables */
            upleft = up;
            up     = upright;
            left   = word;
            row   |= word;
        }

        if (!row) {
            /* Empty row - break */
            break;
        }
    }

    return change ? y : 0;
}


/**
 *  8-connectivity reversed fill iteration.
 */
int
rc_fill_8conn_rev_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict map, int map_dim,
                      int width, int height)
{
    int len    = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int end    = (len - 1)*RC_WORD_SIZE;
    int change = 0;
    int y;

    for (y = height - 1; y >= 0; y--) {
        int       x;
        int       i       = y*map_dim + end;
        int       j1      = y*dst_dim + end;
        int       j2      = MIN(y + 1, height - 1)*dst_dim + end;
        rc_word_t row     = 0;                      /* Change accumulator   */
        rc_word_t right   = 0;                      /* Sliding left word    */
        rc_word_t down    = RC_WORD_LOAD(&dst[j2]); /* Sliding upper word   */
        rc_word_t dnright = 0;                      /* Sliding up-left word */

        for (x = len - 1, j2 -= RC_WORD_SIZE;
             x >= 0;
             x--, i -= RC_WORD_SIZE, j1 -= RC_WORD_SIZE, j2 -= RC_WORD_SIZE)
        {
            rc_word_t word, dnleft, mask;

            /* Load words */
            mask   = RC_WORD_LOAD(&map[i ]);
            word   = RC_WORD_LOAD(&dst[j1]);
            dnleft = (x > 0) ? RC_WORD_LOAD(&dst[j2]) : 0;

            /* Fill word and store */
            RC_FILL_8CONN(word, 0, right, down, dnleft, dnright, mask, change);
            RC_WORD_STORE(&dst[j1], word);

            /* Update sliding variables */
            dnright = down;
            down    = dnleft;
            right   = word;
            row    |= word;
        }

        if (!row) {
            /* Empty row - break */
            break;
        }
    }

    return change ? height - y - 1 : 0;
}
