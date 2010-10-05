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
 *  @file   rc_contour.c
 *  @brief  RAPP Compute layer contour chain code generation.
 *
 *  Algorithm (8-connectivity)
 *  --------------------------
 *  We start at the first pixel set in the input image when
 *  traversing it in row-major order. The 2x3 neighborhood
 *  around this pixel is now
 *
 *    01x
 *    xxx
 *
 *  where x pixels can be either 0 or 1. There are thus 2**4 = 16
 *  possible start configurations. There are also at most two independent
 *  contour loops that pass the start pixel at the transition from
 *  the first loop to the second. For example, the image
 *
 *    011
 *    100
 *
 *  will give the chain code 0451, with 04 being the first loop
 *  and 51 the second.
 *
 *  Each loop is traced independently. For each new position,
 *  we want to turn left as much as we can. We construct a bit mask
 *  from the eight neigboring pixels, in clockwise order from the
 *  most significant bit. This mask is then normalized via rotate shift
 *  to sort the bits in decreasing priority from the most significant
 *  bit, i.e. the most significant bit correspond to the left-most turn.
 *  The next normalized, or relative, chain code is then the bit position
 *  of the first set pixel in the mask. This is equal to the leading-zero
 *  count of the mask. Finally, the normalized chain code is then
 *  translated back to an absolute chain code by adding an offset depending
 *  on the previous code. The 4-connectivity algorithm is analoguous.
 *
 *  For simplicity, the word size is fixed to 32 bits on all platforms.
 *  Big endian pixel data is converted to little-endian by is reversing
 *  the three bits in each neighborhood row as they are read from the
 *  row words. This can be done efficiently by a 3-bit lookup table.
 *  By hiding the endian-specific handling at a low level, we can reuse
 *  all the bit-manipulation code for the actual contour tracing.
 */

#include <assert.h>       /* assert()             */
#include "rc_platform.h"  /* Endianness           */
#define RC_WORD_SIZE 4    /* Forcing 4-byte words */
#include "rc_word.h"      /* Word operations      */
#include "rc_pixel.h"     /* RC_PIXEL_GET_BIN()   */
#include "rc_crop.h"      /* rc_crop_seek_bin()   */
#include "rc_contour.h"   /* Contour API          */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  32-bit misaligned word read.
 */
#ifndef RC_BIG_ENDIAN
#define RC_CONTOUR_WORD(buf, idx)                        \
    (((uint32_t)((const uint16_t*)(buf))[idx    ]) |     \
     ((uint32_t)((const uint16_t*)(buf))[idx + 1] << 16))

#else
#define RC_CONTOUR_WORD(buf, idx)                          \
    (((uint32_t)((const uint16_t*)(buf))[idx    ] << 16) | \
     ((uint32_t)((const uint16_t*)(buf))[idx + 1]))
#endif

/**
 *  Get the endian-independent 3-bit mask with the pixels
 *  at logical positions pos, pos + 1 and pos + 2.
 */
#ifdef RC_BIG_ENDIAN
#define RC_CONTOUR_ROW3(word, pos) \
    rc_contour_rev3_tab[RC_WORD_EXTRACT(word, pos, 3)]

#else
#define RC_CONTOUR_ROW3(word, pos) \
    RC_WORD_EXTRACT(word, pos, 3)
#endif


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rc_contour_4conn_loop(char *restrict contour, int len,
                      const uint8_t *restrict buf, int dim,
                      int xpos, int ypos, int dir, int *last);
static int
rc_contour_8conn_loop(char *restrict contour, int len,
                      const uint8_t *restrict buf, int dim,
                      int xpos, int ypos, int dir, int *last);

/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  Bit reversal table.
 */
#ifdef RC_BIG_ENDIAN
static const uint8_t rc_contour_rev3_tab[] = {0, 4, 2, 6, 1, 5, 3, 7};
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  4-connectivity contour chain code.
 */
int
rc_contour_4conn_bin(unsigned origin[2], char *restrict contour, int len,
                     const uint8_t *restrict buf, int dim,
                     int width, int height)
{
    static const uint8_t dir_tab[4][2][2] = {
      /* Loop 1    loop 2    */
      /* Start end start end */
        {{0,   0}, {0,   0}}, /* Pattern 0: no  loops */
        {{0,   2}, {0,   0}}, /* Pattern 1: one loop  */
        {{3,   1}, {0,   0}}, /* Pattern 2: one loop  */
        {{0,   2}, {3,   1}}  /* Pattern 3: two loops */
    };

    int pos = 0;
    int pat;
    int k;

    /* Find the first pixel set */
    if (!rc_crop_seek_bin(buf, dim, width, height, origin)) {
        goto Done;
    }

    /* Construct the start configuration pattern */
    pat = (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0] + 1, origin[1]    ) << 0) |
          (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0]    , origin[1] + 1) << 1);

    /* Process the at most two independent loops */
    for (k = 0; k < 2; k++) {
        int code = dir_tab[pat][k][0];
        int tail = dir_tab[pat][k][1];
        int last;

        if (code || tail) {
            /* Call loop tracing function to trace this loop */
            pos += rc_contour_4conn_loop(&contour[pos], len - pos, buf, dim,
                                         origin[0], origin[1], code, &last);

            /* Check if we got back from the same loop */
            if (last != tail) {
                /* First loop connected to the second - we are done */
                assert(k == 0);
                assert(last == dir_tab[pat][1][1]);
                break;
            }
        }
    }

Done:
    /* Add NUL termination */
    if (pos < len) {
        contour[pos] = '\0';
    }

    return pos;
}

/**
 *  8-connectivity contour chain code.
 */
int
rc_contour_8conn_bin(unsigned origin[2], char *restrict contour, int len,
                     const uint8_t *restrict buf, int dim,
                     int width, int height)
{
    static const uint8_t dir_tab[16][2][2] = {
      /* Loop 1    loop 2    */
      /* Start end start end */
        {{0,   0}, {0,   0}}, /* Pattern  0: no  loops */
        {{0,   4}, {0,   0}}, /* Pattern  1: one loop  */
        {{7,   3}, {0,   0}}, /* Pattern  2: one loop  */
        {{0,   3}, {0,   0}}, /* Pattern  3: one loop  */
        {{6,   2}, {0,   0}}, /* Pattern  4: one loop  */
        {{0,   2}, {0,   0}}, /* Pattern  5: one loop  */
        {{7,   2}, {0,   0}}, /* Pattern  6: one loop  */
        {{0,   2}, {0,   0}}, /* Pattern  7: one loop  */
        {{5,   1}, {0,   0}}, /* Pattern  8: one loop  */
        {{0,   4}, {5,   1}}, /* Pattern  9: two loops */
        {{7,   3}, {5,   1}}, /* Pattern 10: two loops */
        {{0,   3}, {5,   1}}, /* Pattern 11: two loops */
        {{6,   1}, {0,   0}}, /* Pattern 12: one loop  */
        {{0,   1}, {0,   0}}, /* Pattern 13: one loop  */
        {{7,   1}, {0,   0}}, /* Pattern 14: one loop  */
        {{0,   1}, {0,   0}}  /* Pattern 15: one loop  */
    };

    int pos = 0;
    int pat;
    int k;

    /* Find the first pixel set */
    if (!rc_crop_seek_bin(buf, dim, width, height, origin)) {
        goto Done;
    }

    /* Construct the start configuration pattern */
    pat = (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0] + 1, origin[1]    ) << 0) |
          (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0] + 1, origin[1] + 1) << 1) |
          (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0]    , origin[1] + 1) << 2) |
          (RC_PIXEL_GET_BIN(buf, dim, 0, origin[0] - 1, origin[1] + 1) << 3);

    /* Process the at most two independent loops */
    for (k = 0; k < 2; k++) {
        int code = dir_tab[pat][k][0];
        int tail = dir_tab[pat][k][1];
        int last;

        if (code || tail) {
            /* Call loop tracing function to trace this loop */
            pos += rc_contour_8conn_loop(&contour[pos], len - pos, buf, dim,
                                         origin[0], origin[1], code, &last);

            /* Check if we got back from the same loop */
            if (last != tail) {
                /* First loop connected to the second - we are done */
                assert(k == 0);
                assert(last == dir_tab[pat][1][1]);
                break;
            }
        }
    }

Done:
    /* Add NUL termination */
    if (pos < len) {
        contour[pos] = '\0';
    }

    return pos;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rc_contour_4conn_loop(char *restrict contour, int len,
                      const uint8_t *restrict buf, int dim,
                      int xpos, int ypos, int dir, int *last)
{
    static const uint8_t clz3[8] = {3, 2, 1, 1, 0, 0, 0, 0};  /* 3-bit clz */
    static const int8_t  xoff[4] = {1,  0, -1, 0};
    static const int8_t  yoff[4] = {0, -1,  0, 1};

    rc_word_t word1;                           /* Last row word        */
    rc_word_t word2;                           /* This row word        */
    rc_word_t word3;                           /* Next row word        */
    int       dim2  = dim / 2;                 /* 16-bit buffer dim.   */
    int       code  = dir;                     /* Previous chain code  */
    int       dx    = xoff[dir];               /* Relative x position  */
    int       dy    = yoff[dir];               /* Relative y position  */
    int       pos   = (xpos + dx) / 16 +       /* 16-bit buffer pos    */
                      (ypos + dy)*dim2;
    int       shift = (xpos + dx) % 16 - 1;    /* 32-bit word shift    */
    int       idx   = 1;                       /* Contour buffer index */

    /* Adjust initial position at word boundaries */
    if (shift > 29) {
        shift -= 16;
        pos++;
    }
    else if (shift < 0) {
        shift += 16;
        pos--;
    }

    /* Set the neighborhood words */
    word1 = RC_CONTOUR_WORD(buf, pos - dim2);
    word2 = RC_CONTOUR_WORD(buf, pos       );
    word3 = RC_CONTOUR_WORD(buf, pos + dim2);

    /* Set the initial chain code in ASCII format */
    if (len > 0) {
        contour[0] = '0' + dir;
    }

    /* Trace the contour */
    while (dx != 0 || dy != 0) {
        unsigned row1 = RC_CONTOUR_ROW3(word1, shift); /* Upper  3-pixel row */
        unsigned row2 = RC_CONTOUR_ROW3(word2, shift); /* Middle 3-pixel row */
        unsigned row3 = RC_CONTOUR_ROW3(word3, shift); /* Lower  3-pixel row */
        unsigned mask;
        int      norm;
        int      sh;

        /* The current pixel must be set */
        assert(row2 & 2);

        /* Compute the neighborhood bit mask */
        mask = (row1 & 2) |                        /* Bit  1    ->  1    */
               (((row2 >> 2) | (row2 << 2)) & 7) | /* Bit [0,2] -> [2,0] */
               ((row3 & 2) << 2);                  /* Bit  1    ->  3    */

        /* Compute the mask normalization shift */
        sh = (code + 2) & 3;

        /* Normalize the neighborhood mask */
        mask = (mask >> sh) | (mask << (4 - sh));

        /* Look up the next normalized chain code from the upper 3 bits */
        norm = clz3[(mask >> 1) & 7];

        /* Compute the next actual chain code */
        code = (code - norm + 1) & 3;

        /* Update position */
        switch (code) {
            case 0:
                /* Positive x direction */
                dx++, shift++;
                if (shift == 30) {
                    pos++;
                    shift -= 16;
                    word1  = RC_CONTOUR_WORD(buf, pos - dim2);
                    word2  = RC_CONTOUR_WORD(buf, pos       );
                    word3  = RC_CONTOUR_WORD(buf, pos + dim2);
                }
                break;

            case 1:
                /* Negative y direction */
                dy--;
                pos  -= dim2;
                word3 = word2;
                word2 = word1;
                word1 = RC_CONTOUR_WORD(buf, pos - dim2);
                break;

            case 2:
                /* Negative x direction */
                dx--, shift--;
                if (shift == -1) {
                    pos--;
                    shift += 16;
                    word1  = RC_CONTOUR_WORD(buf, pos - dim2);
                    word2  = RC_CONTOUR_WORD(buf, pos       );
                    word3  = RC_CONTOUR_WORD(buf, pos + dim2);
                }
                break;

            case 3:
                /* Positiv y direction */
                dy++;
                pos  += dim2;
                word1 = word2;
                word2 = word3;
                word3 = RC_CONTOUR_WORD(buf, pos + dim2);
                break;
        }

        /* Save the contour chain code in ASCII format */
        if (idx < len) {
            contour[idx] = '0' + code;
        }

        /* One more chain code */
        idx++;
    }

    /* Set the last chain code */
    *last = code;

    return idx;
}

static int
rc_contour_8conn_loop(char *restrict contour, int len,
                      const uint8_t *restrict buf, int dim,
                      int xpos, int ypos, int dir, int *last)
{
    static const uint8_t clz6[64] = {6, 5, 4, 4, 3, 3, 3, 3,
                                     2, 2, 2, 2, 2, 2, 2, 2,
                                     1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1,
                                     0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0}; /* 6-bit clz */
    static const int8_t  xoff[8]  = {1,  1,  0, -1, -1, -1, 0, 1};
    static const int8_t  yoff[8]  = {0, -1, -1, -1,  0,  1, 1, 1};

    rc_word_t word1;                           /* Last row word        */
    rc_word_t word2;                           /* This row word        */
    rc_word_t word3;                           /* Next row word        */
    int       dim2  = dim / 2;                 /* 16-bit buffer dim.   */
    int       code  = dir;                     /* Previous chain code  */
    int       dx    = xoff[dir];               /* Relative x position  */
    int       dy    = yoff[dir];               /* Relative y position  */
    int       pos   = (xpos + dx) / 16 +       /* 16-bit buffer pos    */
                      (ypos + dy)*dim2;
    int       shift = (xpos + dx) % 16 - 1;    /* 32-bit word shift    */
    int       idx   = 1;                       /* Contour buffer index */

    /* Adjust initial position at word boundaries */
    if (shift > 29) {
        shift -= 16;
        pos++;
    }
    else if (shift < 0) {
        shift += 16;
        pos--;
    }

    /* Set the neighborhood words */
    word1 = RC_CONTOUR_WORD(buf, pos - dim2);
    word2 = RC_CONTOUR_WORD(buf, pos       );
    word3 = RC_CONTOUR_WORD(buf, pos + dim2);

    /* Set the initial chain code in ASCII format */
    if (len > 0) {
        contour[0] = '0' + dir;
    }

    /* Trace the contour */
    while (dx != 0 || dy != 0) {
        unsigned row1 = RC_CONTOUR_ROW3(word1, shift); /* Upper  3-pixel row */
        unsigned row2 = RC_CONTOUR_ROW3(word2, shift); /* Middle 3-pixel row */
        unsigned row3 = RC_CONTOUR_ROW3(word3, shift); /* Lower  3-pixel row */
        unsigned mask;
        int      norm;
        int      sh;

        /* The current pixel must be set */
        assert(row2 & 2);

        /* Compute the neighborhood bit mask */
        mask = ((0xe6a2c480UL >> 4*row1) & 0xf)     | /* Bit [0-2] -> [3-1] */
               (((row2 >> 2) | (row2 << 4)) & 0x11) | /* Bit [0,2] -> [4,0] */
               (row3 << 5);                           /* Bit [0-2] -> [5-7] */

        /* Compute the mask shift */
        sh = (code + 3) & 7;

        /* Normalize the neighborhood mask */
        mask = (mask >> sh) | (mask << (8 - sh));

        /* Look up the next normalized chain code from the upper 6 bits */
        norm = clz6[(mask >> 2) & 0x3f];

        /* Compute the next actual chain code */
        code = (code - norm + 2) & 7;

        /* Horizontal update */
        switch (code) {
            case 0:
            case 1:
            case 7:
                /* Positive x direction */
                dx++, shift++;
                if (shift == 30) {
                    pos++;
                    shift -= 16;
                    word1  = RC_CONTOUR_WORD(buf, pos - dim2);
                    word2  = RC_CONTOUR_WORD(buf, pos       );
                    word3  = RC_CONTOUR_WORD(buf, pos + dim2);
                }
                break;

            case 3:
            case 4:
            case 5:
                /* Negative x direction */
                dx--, shift--;
                if (shift == -1) {
                    pos--;
                    shift += 16;
                    word1  = RC_CONTOUR_WORD(buf, pos - dim2);
                    word2  = RC_CONTOUR_WORD(buf, pos       );
                    word3  = RC_CONTOUR_WORD(buf, pos + dim2);
                }
                break;
        }

        /* Vertical update */
        switch (code) {
            case 5:
            case 6:
            case 7:
                dy++;
                pos  += dim2;
                word1 = word2;
                word2 = word3;
                word3 = RC_CONTOUR_WORD(buf, pos + dim2);
                break;

            case 1:
            case 2:
            case 3:
                dy--;
                pos  -= dim2;
                word3 = word2;
                word2 = word1;
                word1 = RC_CONTOUR_WORD(buf, pos - dim2);
                break;
        }

        /* Save the contour chain code in ASCII format */
        if (idx < len) {
            contour[idx] = '0' + code;
        }

        /* One more chain code */
        idx++;
    }

    /* Set the last chain code */
    *last = code;

    return idx;
}
