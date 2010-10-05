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
 *  @file   rc_crop.c
 *  @brief  RAPP Compute layer binary image cropping.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_platform.h" /* Endianness            */
#include "rc_word.h"     /* Word operations       */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_crop.h"     /* Binary cropping API   */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Get the position of the first pixel set in a 4-bit nibble.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_CROP_FIRST(nibble) \
    rc_crop_clz_tab[nibble]
#else
#define RC_CROP_FIRST(nibble) \
    rc_crop_ctz_tab[nibble]
#endif

/**
 *  Get the position of the last pixel set in a 4-bit nibble.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_CROP_LAST(nibble) \
    rc_crop_ctz_tab[nibble]
#else
#define RC_CROP_LAST(nibble) \
    rc_crop_clz_tab[nibble]
#endif


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  4-bit clz table.
 */
static const uint8_t rc_crop_clz_tab[16] = {
    4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

/**
 *  4-bit ctz table.
 */
static const uint8_t rc_crop_ctz_tab[16] = {
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1
};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static rc_word_t
rc_crop_accum_row(const uint8_t *buf, int len);

static rc_word_t
rc_crop_accum_col(const uint8_t *buf, int dim, int height);

static int
rc_crop_get_first(rc_word_t word);

static int
rc_crop_get_last(rc_word_t word);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Find the first non-zero pixel in traversal order.
 */
int
rc_crop_seek_bin(const uint8_t *buf, int dim, int width, int height,
                 unsigned pos[2])
{
    int len = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int x, y;

    for (y = 0; y < height; y++) {
        int i = y*dim;

        for (x = 0; x < len; x++, i += RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&buf[i]);
            if (word) {
                pos[0] = 8*RC_WORD_SIZE*x + rc_crop_get_first(word);
                pos[1] = y;
                return 1;
            }
        }
    }

    return 0;
}


/**
 *  Compute the bounding box.
 */
int
rc_crop_box_bin(const uint8_t *buf, int dim, int width, int height,
                unsigned box[4])
{
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);
    int ymin = 0;
    int ymax = 0;
    int y, i;

    /* Find the upper limit */
    for (y = 0, i = 0; y < height; y++, i += dim) {
        if (rc_crop_accum_row(&buf[i], len)) {
            ymin = y;
            break;
        }
    }

    /* Continue if the image is non-empty */
    if (y < height) {
        rc_word_t wmin = 0;
        rc_word_t wmax = 0;
        int       xmin = 0;
        int       xmax = 0;
        int       x;

        /* Find the lower limit */
        for (y = height - 1, i = y*dim; y >= 0; y--, i -= dim) {
            if (rc_crop_accum_row(&buf[i], len)) {
                ymax = y;
                break;
            }
        }

        /* Find the left limit */
        i = ymin*dim;
        for (x = 0; x < len; x++, i += RC_WORD_SIZE) {
            wmin = rc_crop_accum_col(&buf[i], dim, ymax - ymin + 1);
            if (wmin) {
                xmin = x;
                break;
            }
        }

        /* Find the right limit */
        i = ymin*dim + RC_WORD_SIZE*(len - 1);
        for (x = len - 1; x >= 0; x--, i -= RC_WORD_SIZE) {
            wmax = rc_crop_accum_col(&buf[i], dim, ymax - ymin + 1);
            if (wmax) {
                xmax = x;
                break;
            }
        }

        /* Set bounding-box values */
        box[0] = 8*RC_WORD_SIZE*xmin + rc_crop_get_first(wmin);
        box[1] = ymin;
        box[2] = 8*RC_WORD_SIZE*xmax + rc_crop_get_last(wmax) - box[0] + 1;
        box[3] = ymax - ymin + 1;

        return 1;
    }

    return 0;
}

/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static rc_word_t
rc_crop_accum_row(const uint8_t *buf, int len)
{
    rc_word_t word = 0;
    int       k, i;

    for (k = 0, i = 0; k < len; k++, i += RC_WORD_SIZE) {
        word |= RC_WORD_LOAD(&buf[i]);
    }

    return word;
}

static rc_word_t
rc_crop_accum_col(const uint8_t *buf, int dim, int height)
{
    rc_word_t word = 0;
    int       k, i;

    for (k = 0, i = 0; k < height; k++, i += dim) {
        word |= RC_WORD_LOAD(&buf[i]);
    }

    return word;
}

static int
rc_crop_get_first(rc_word_t word)
{
    int k;

    for (k = 0; k < 8*RC_WORD_SIZE; k += 4) {
        int nibble = RC_WORD_EXTRACT(word, k, 4);
        if (nibble) {
            return k + RC_CROP_FIRST(nibble);
        }
    }

    return 8*RC_WORD_SIZE;
}

static int
rc_crop_get_last(rc_word_t word)
{
    int k;

    for (k = 8*RC_WORD_SIZE - 4; k >= 0; k -= 4) {
        int nibble = RC_WORD_EXTRACT(word, k, 4);
        if (nibble) {
            return k + 3 - RC_CROP_LAST(nibble);
        }
    }

    return 0;
}
