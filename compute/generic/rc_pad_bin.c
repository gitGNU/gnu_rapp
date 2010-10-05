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
 *  @file   rc_pad_bin.c
 *  @brief  RAPP Compute layer binary image padding.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_word.h"     /* Word operations       */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_pad_bin.h"  /* Binary padding API    */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Pad to the left with a constant value.
 */
void
rc_pad_const_left_bin(uint8_t *buf, int dim, int off,
                      int width, int height, int size, int set)
{
    uint8_t *wbuf = (uint8_t*)((uintptr_t)buf & ~(RC_WORD_SIZE - 1));
    int      woff = 8*((uintptr_t)buf % RC_WORD_SIZE) + off;

    /* The 'width' argument is unused */
    (void)width;

    /* Set all full words */
    if (size > woff) {
        rc_word_t word = set ? RC_WORD_ONE : RC_WORD_ZERO;
        int       full = RC_DIV_CEIL(size - woff, 8*RC_WORD_SIZE);
        int       y;

        /* Set full word pixels according to 'set' */
        for (y = 0; y < height; y++) {
            int i = y*dim - full*RC_WORD_SIZE;
            int x;
            for (x = 0; x < full; x++, i += RC_WORD_SIZE) {
                RC_WORD_STORE(&wbuf[i], word);
            }
        }
    }

    /* Set partial words */
    if (woff) {
        rc_word_t mask = RC_WORD_SHR(RC_WORD_ONE, woff); /* Masking word */
        int       y, i;

        if (set) {
            /* Set pad pixels to ONE by ORing the inverted mask */
            mask = ~mask;
            for (y = 0, i = 0; y < height; y++, i += dim) {
                rc_word_t word = RC_WORD_LOAD(&wbuf[i]);
                RC_WORD_STORE(&wbuf[i], word | mask);
            }
        }
        else {
            /* Set pad pixels to ZERO by ANDing the mask */
            for (y = 0, i = 0; y < height; y++, i += dim) {
                rc_word_t word = RC_WORD_LOAD(&wbuf[i]);
                RC_WORD_STORE(&wbuf[i], word & mask);
            }
        }
    }
}


/**
 *  Pad to the right with a constant value.
 */
void
rc_pad_const_right_bin(uint8_t *buf, int dim, int off,
                       int width, int height, int size, int set)
{
    uint8_t *wbuf = (uint8_t*)((uintptr_t)buf & ~(RC_WORD_SIZE - 1));
    int      woff = 8*((uintptr_t)buf % RC_WORD_SIZE) + off;
    int      wend = (woff + width - 1) % (8*RC_WORD_SIZE);
    int      wrem = 8*RC_WORD_SIZE - wend - 1;
    int      wlen = RC_DIV_CEIL(width + woff, 8*RC_WORD_SIZE);

    /* Set all full words */
    if (size > wrem) {
        rc_word_t word = set ? RC_WORD_ONE : RC_WORD_ZERO;
        int       full = RC_DIV_CEIL(size - wrem, 8*RC_WORD_SIZE);
        int       y;

        /* Set full word pixels according to 'set' */
        for (y = 0; y < height; y++) {
            int i = y*dim + wlen*RC_WORD_SIZE;
            int x;
            for (x = 0; x < full; x++, i += RC_WORD_SIZE) {
                RC_WORD_STORE(&wbuf[i], word);
            }
        }
    }

    /* Set partial words */
    if (wrem) {
        rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE, wrem); /* Masking word */
        int       i    = (wlen - 1)*RC_WORD_SIZE;
        int       y;

        if (set) {
            /* Set pad pixels to ONE by ORing the inverted mask */
            mask = ~mask;
            for (y = 0; y < height; y++, i += dim) {
                rc_word_t word = RC_WORD_LOAD(&wbuf[i]);
                RC_WORD_STORE(&wbuf[i], word | mask);
            }
        }
        else {
            /* Set pad pixels to ZERO by ANDing the mask */
            for (y = 0; y < height; y++, i += dim) {
                rc_word_t word = RC_WORD_LOAD(&wbuf[i]);
                RC_WORD_STORE(&wbuf[i], word & mask);
            }
        }
    }
}


/**
 *  Pad to the left by clamping (replication).
 */
void
rc_pad_clamp_left_bin(uint8_t *buf, int dim, int off,
                      int width, int height, int size)
{
    uint8_t  *wbuf = (uint8_t*)((uintptr_t)buf & ~(RC_WORD_SIZE - 1));
    int       woff = 8*((uintptr_t)buf % RC_WORD_SIZE) + off;
    int       full = RC_DIV_CEIL(size - woff, 8*RC_WORD_SIZE);
    rc_word_t mask = RC_WORD_SHR(RC_WORD_ONE, woff);
    int       y;

    /* The 'width' argument is unused */
    (void)width;

    /* Perform padding for all rows */
    for (y = 0; y < height; y++) {
        rc_word_t word, pad;
        int       i = y*dim;
        int       x;

        /* Create the pad word according to the first pixel value */
        word = RC_WORD_LOAD(&wbuf[i]);
        pad  = ~(RC_WORD_EXTRACT(word, woff, 1) - 1);

        /* Set the partial word */
        RC_WORD_STORE(&wbuf[i], (word & mask) | (pad & ~mask));

        /* Set all full words */
        for (x = 0, i -= RC_WORD_SIZE; x < full; x++, i -= RC_WORD_SIZE) {
            RC_WORD_STORE(&wbuf[i], pad);
        }
    }
}


/**
 *  Pad to the right by clamping (replication).
 */
void
rc_pad_clamp_right_bin(uint8_t *buf, int dim, int off,
                       int width, int height, int size)
{
    uint8_t  *wbuf = (uint8_t*)((uintptr_t)buf & ~(RC_WORD_SIZE - 1));
    int       woff = 8*((uintptr_t)buf % RC_WORD_SIZE) + off;
    int       wend = (woff + width - 1) % (8*RC_WORD_SIZE);
    int       wrem = 8*RC_WORD_SIZE - wend - 1;
    int       wlen = RC_DIV_CEIL(width + woff, 8*RC_WORD_SIZE);
    int       full = RC_DIV_CEIL(size - wrem, 8*RC_WORD_SIZE);
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE, wrem);
    int       y;

    /* Perform padding for all rows */
    for (y = 0; y < height; y++) {
        rc_word_t word, pad;
        int       i = y*dim + (wlen - 1)*RC_WORD_SIZE;
        int       x;

        /* Create the pad word according to the first pixel value */
        word = RC_WORD_LOAD(&wbuf[i]);
        pad  = ~(RC_WORD_EXTRACT(word, wend, 1) - 1);

        /* Set the partial word */
        RC_WORD_STORE(&wbuf[i], (word & mask) | (pad & ~mask));

        /* Set all full words */
        for (x = 0, i += RC_WORD_SIZE; x < full; x++, i += RC_WORD_SIZE) {
            RC_WORD_STORE(&wbuf[i], pad);
        }
    }
}
