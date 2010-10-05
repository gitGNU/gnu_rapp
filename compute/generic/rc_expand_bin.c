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
 *  @file   rc_expand_bin.c
 *  @brief  RAPP Compute layer binary 2x expansion.
 */

#include "rc_impl_cfg.h"   /* Implementation config */
#include "rc_platform.h"   /* Endianness            */
#include "rc_word.h"       /* Word operations       */
#include "rc_util.h"       /* RC_DIV_CEIL()         */
#include "rc_expand_bin.h" /* Binary expansion API  */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Expand a half word into a full word.
 */
#if RC_WORD_SIZE == 2
#define RC_EXPAND_WORD(we, w, sh)                             \
do {                                                          \
    rc_word_t w1_ = rc_expand_lut[RC_WORD_EXTRACT(w, sh, 8)]; \
    (we) = RC_WORD_INSERT(w1_, 0, 16);                        \
} while (0)

#elif RC_WORD_SIZE == 4
#define RC_EXPAND_WORD(we, w, sh)                                   \
do {                                                                \
    rc_word_t w1_ = rc_expand_lut[RC_WORD_EXTRACT(w,  sh,      8)]; \
    rc_word_t w2_ = rc_expand_lut[RC_WORD_EXTRACT(w, (sh) + 8, 8)]; \
    (we) = RC_WORD_INSERT(w1_,  0, 16) |                            \
           RC_WORD_INSERT(w2_, 16, 16);                             \
} while (0)

#elif RC_WORD_SIZE == 8
#define RC_EXPAND_WORD(we, w, sh)                                    \
do {                                                                 \
    rc_word_t w1_ = rc_expand_lut[RC_WORD_EXTRACT(w,  sh,       8)]; \
    rc_word_t w2_ = rc_expand_lut[RC_WORD_EXTRACT(w, (sh) +  8, 8)]; \
    rc_word_t w3_ = rc_expand_lut[RC_WORD_EXTRACT(w, (sh) + 16, 8)]; \
    rc_word_t w4_ = rc_expand_lut[RC_WORD_EXTRACT(w, (sh) + 24, 8)]; \
    (we) = RC_WORD_INSERT(w1_,  0, 16) |                             \
           RC_WORD_INSERT(w2_, 16, 16) |                             \
           RC_WORD_INSERT(w3_, 32, 16) |                             \
           RC_WORD_INSERT(w4_, 48, 16);                              \
} while (0)
#endif

/**
 *  Expand the lower half of a word.
 */
#define RC_EXPAND_LO(we, w) \
    RC_EXPAND_WORD(we, w, 0)

/**
 *  Expand the upper half of a word.
 */
#define RC_EXPAND_HI(we, w) \
    RC_EXPAND_WORD(we, w, 4*RC_WORD_SIZE)


/*
 * -------------------------------------------------------------
 *  Iteration macros
 * -------------------------------------------------------------
 */

/**
 *  1x2 expansion, full iteration.
 */
#define RC_EXPAND_1X2_ITER(dst, src, i, j)   \
do {                                         \
    rc_word_t sw  = RC_WORD_LOAD(&(src)[i]); \
    rc_word_t dw;                            \
    RC_EXPAND_LO(dw, sw);                    \
    RC_WORD_STORE(&(dst)[j], dw);            \
    (j) += RC_WORD_SIZE;                     \
    RC_EXPAND_HI(dw, sw);                    \
    RC_WORD_STORE(&(dst)[j], dw);            \
    (j) += RC_WORD_SIZE;                     \
    (i) += RC_WORD_SIZE;                     \
} while (0)

/**
 *  1x2 expansion, half iteration.
 */
#define RC_EXPAND_1X2_TAIL(dst, src, i, j)  \
do {                                        \
    rc_word_t sw = RC_WORD_LOAD(&(src)[i]); \
    rc_word_t dw;                           \
    RC_EXPAND_LO(dw, sw);                   \
    RC_WORD_STORE(&(dst)[j], dw);           \
} while (0)

/**
 *  2x1 expansion, full iteration.
 */
#define RC_EXPAND_2X1_ITER(dst, src, i, j1, j2) \
do {                                            \
    rc_word_t w = RC_WORD_LOAD(&(src)[i]);      \
    RC_WORD_STORE(&(dst)[j1], w);               \
    RC_WORD_STORE(&(dst)[j2], w);               \
    (j1) += RC_WORD_SIZE;                       \
    (j2) += RC_WORD_SIZE;                       \
    (i)  += RC_WORD_SIZE;                       \
} while (0)

/**
 *  2x1 expansion, half iteration.
 */
#define RC_EXPAND_2X1_TAIL(dst, src, i, j1, j2) \
do {                                            \
    rc_word_t w = RC_WORD_LOAD(&(src)[i]);      \
    RC_WORD_STORE(&(dst)[j1], w);               \
    RC_WORD_STORE(&(dst)[j2], w);               \
} while (0)

/**
 *  2x2 expansion, full iteration.
 */
#define RC_EXPAND_2X2_ITER(dst, src, i, j1, j2) \
do {                                            \
    rc_word_t sw  = RC_WORD_LOAD(&(src)[i]);    \
    rc_word_t dw;                               \
    RC_EXPAND_LO(dw, sw);                       \
    RC_WORD_STORE(&(dst)[j1], dw);              \
    RC_WORD_STORE(&(dst)[j2], dw);              \
    (j1) += RC_WORD_SIZE;                       \
    (j2) += RC_WORD_SIZE;                       \
    RC_EXPAND_HI(dw, sw);                       \
    RC_WORD_STORE(&(dst)[j1], dw);              \
    RC_WORD_STORE(&(dst)[j2], dw);              \
    (j1) += RC_WORD_SIZE;                       \
    (j2) += RC_WORD_SIZE;                       \
    (i)  += RC_WORD_SIZE;                       \
} while (0)

/**
 *  2x2 expansion, half iteration.
 */
#define RC_EXPAND_2X2_TAIL(dst, src, i, j1, j2) \
do {                                            \
    rc_word_t sw  = RC_WORD_LOAD(&(src)[i]);    \
    rc_word_t dw;                               \
    RC_EXPAND_LO(dw, sw);                       \
    RC_WORD_STORE(&(dst)[j1], dw);              \
    RC_WORD_STORE(&(dst)[j2], dw);              \
} while (0)


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  Byte-to-halfword bit expansion table.
 */
static const uint16_t rc_expand_lut[256] = {
    0x0000, 0x0003, 0x000c, 0x000f, 0x0030, 0x0033, 0x003c, 0x003f,
    0x00c0, 0x00c3, 0x00cc, 0x00cf, 0x00f0, 0x00f3, 0x00fc, 0x00ff,
    0x0300, 0x0303, 0x030c, 0x030f, 0x0330, 0x0333, 0x033c, 0x033f,
    0x03c0, 0x03c3, 0x03cc, 0x03cf, 0x03f0, 0x03f3, 0x03fc, 0x03ff,
    0x0c00, 0x0c03, 0x0c0c, 0x0c0f, 0x0c30, 0x0c33, 0x0c3c, 0x0c3f,
    0x0cc0, 0x0cc3, 0x0ccc, 0x0ccf, 0x0cf0, 0x0cf3, 0x0cfc, 0x0cff,
    0x0f00, 0x0f03, 0x0f0c, 0x0f0f, 0x0f30, 0x0f33, 0x0f3c, 0x0f3f,
    0x0fc0, 0x0fc3, 0x0fcc, 0x0fcf, 0x0ff0, 0x0ff3, 0x0ffc, 0x0fff,
    0x3000, 0x3003, 0x300c, 0x300f, 0x3030, 0x3033, 0x303c, 0x303f,
    0x30c0, 0x30c3, 0x30cc, 0x30cf, 0x30f0, 0x30f3, 0x30fc, 0x30ff,
    0x3300, 0x3303, 0x330c, 0x330f, 0x3330, 0x3333, 0x333c, 0x333f,
    0x33c0, 0x33c3, 0x33cc, 0x33cf, 0x33f0, 0x33f3, 0x33fc, 0x33ff,
    0x3c00, 0x3c03, 0x3c0c, 0x3c0f, 0x3c30, 0x3c33, 0x3c3c, 0x3c3f,
    0x3cc0, 0x3cc3, 0x3ccc, 0x3ccf, 0x3cf0, 0x3cf3, 0x3cfc, 0x3cff,
    0x3f00, 0x3f03, 0x3f0c, 0x3f0f, 0x3f30, 0x3f33, 0x3f3c, 0x3f3f,
    0x3fc0, 0x3fc3, 0x3fcc, 0x3fcf, 0x3ff0, 0x3ff3, 0x3ffc, 0x3fff,
    0xc000, 0xc003, 0xc00c, 0xc00f, 0xc030, 0xc033, 0xc03c, 0xc03f,
    0xc0c0, 0xc0c3, 0xc0cc, 0xc0cf, 0xc0f0, 0xc0f3, 0xc0fc, 0xc0ff,
    0xc300, 0xc303, 0xc30c, 0xc30f, 0xc330, 0xc333, 0xc33c, 0xc33f,
    0xc3c0, 0xc3c3, 0xc3cc, 0xc3cf, 0xc3f0, 0xc3f3, 0xc3fc, 0xc3ff,
    0xcc00, 0xcc03, 0xcc0c, 0xcc0f, 0xcc30, 0xcc33, 0xcc3c, 0xcc3f,
    0xccc0, 0xccc3, 0xcccc, 0xcccf, 0xccf0, 0xccf3, 0xccfc, 0xccff,
    0xcf00, 0xcf03, 0xcf0c, 0xcf0f, 0xcf30, 0xcf33, 0xcf3c, 0xcf3f,
    0xcfc0, 0xcfc3, 0xcfcc, 0xcfcf, 0xcff0, 0xcff3, 0xcffc, 0xcfff,
    0xf000, 0xf003, 0xf00c, 0xf00f, 0xf030, 0xf033, 0xf03c, 0xf03f,
    0xf0c0, 0xf0c3, 0xf0cc, 0xf0cf, 0xf0f0, 0xf0f3, 0xf0fc, 0xf0ff,
    0xf300, 0xf303, 0xf30c, 0xf30f, 0xf330, 0xf333, 0xf33c, 0xf33f,
    0xf3c0, 0xf3c3, 0xf3cc, 0xf3cf, 0xf3f0, 0xf3f3, 0xf3fc, 0xf3ff,
    0xfc00, 0xfc03, 0xfc0c, 0xfc0f, 0xfc30, 0xfc33, 0xfc3c, 0xfc3f,
    0xfcc0, 0xfcc3, 0xfccc, 0xfccf, 0xfcf0, 0xfcf3, 0xfcfc, 0xfcff,
    0xff00, 0xff03, 0xff0c, 0xff0f, 0xff30, 0xff33, 0xff3c, 0xff3f,
    0xffc0, 0xffc3, 0xffcc, 0xffcf, 0xfff0, 0xfff3, 0xfffc, 0xffff
};


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary 1x2 expansion.
 */
#if RC_IMPL(rc_expand_1x2_bin, 1)
void
rc_expand_1x2_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    int len  = RC_DIV_CEIL(2*width, 8*RC_WORD_SIZE);   /* Total #words (dst) */
    int full = len / (2*RC_UNROLL(rc_expand_1x2_bin)); /* Fully unrolled     */
    int rem  = len % (2*RC_UNROLL(rc_expand_1x2_bin)); /* Remainder          */
    int twin = rem / 2;                        /* Remaining double dst words */
    int tail = rem % 2;                        /* Remaining single dst word  */
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all unrolled words */
        for (x = 0; x < full; x++) {
            RC_EXPAND_1X2_ITER(dst, src, i, j);
            if (RC_UNROLL(rc_expand_1x2_bin) >= 2) {
                RC_EXPAND_1X2_ITER(dst, src, i, j);
            }
            if (RC_UNROLL(rc_expand_1x2_bin) == 4) {
                RC_EXPAND_1X2_ITER(dst, src, i, j);
                RC_EXPAND_1X2_ITER(dst, src, i, j);
            }
        }

        /* Handle remaining 1x2 word block */
        for (x = 0; x < twin; x++) {
            RC_EXPAND_1X2_ITER(dst, src, i, j);
        }

        /* Handle any single remaining word */
        if (tail) {
            RC_EXPAND_1X2_TAIL(dst, src, i, j);
        }
    }
}
#endif


/**
 *  Binary 2x1 expansion.
 */
#if RC_IMPL(rc_expand_2x1_bin, 1)
void
rc_expand_2x1_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE); /* Total #words   */
    int full = len / RC_UNROLL(rc_expand_2x1_bin); /* Fully unrolled */
    int rem  = len % RC_UNROLL(rc_expand_2x1_bin); /* Remainder      */
    int y;

    for (y = 0; y < height; y++) {
        int i  =   y*src_dim;
        int j1 = 2*y*dst_dim;
        int j2 = j1 + dst_dim;
        int x;

        /* Handle all unrolled words */
        for (x = 0; x < full; x++) {
            RC_EXPAND_2X1_ITER(dst, src, i, j1, j2);
            if (RC_UNROLL(rc_expand_2x1_bin) >= 2) {
                RC_EXPAND_2X1_ITER(dst, src, i, j1, j2);
            }
            if (RC_UNROLL(rc_expand_2x1_bin) == 4) {
                RC_EXPAND_2X1_ITER(dst, src, i, j1, j2);
                RC_EXPAND_2X1_ITER(dst, src, i, j1, j2);
            }
        }

        /* Handle remaining 2x1 word block */
        for (x = 0; x < rem; x++) {
            RC_EXPAND_2X1_ITER(dst, src, i, j1, j2);
        }
    }
}
#endif


/**
 *  Binary 2x2 expansion.
 */
#if RC_IMPL(rc_expand_2x2_bin, 1)
void
rc_expand_2x2_bin(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height)
{
    int len  = RC_DIV_CEIL(2*width, 8*RC_WORD_SIZE);   /* Total #words (dst) */
    int full = len / (2*RC_UNROLL(rc_expand_2x2_bin)); /* Fully unrolled     */
    int rem  = len % (2*RC_UNROLL(rc_expand_2x2_bin)); /* Remainder          */
    int twin = rem / 2;                        /* Remaining double dst words */
    int tail = rem % 2;                        /* Remaining single dst word  */
    int y;

    for (y = 0; y < height; y++) {
        int i  =   y*src_dim;
        int j1 = 2*y*dst_dim;
        int j2 = j1 + dst_dim;
        int x;

        /* Handle all unrolled words */
        for (x = 0; x < full; x++) {
            RC_EXPAND_2X2_ITER(dst, src, i, j1, j2);
            if (RC_UNROLL(rc_expand_2x2_bin) >= 2) {
                RC_EXPAND_2X2_ITER(dst, src, i, j1, j2);
            }
            if (RC_UNROLL(rc_expand_2x2_bin) == 4) {
                RC_EXPAND_2X2_ITER(dst, src, i, j1, j2);
                RC_EXPAND_2X2_ITER(dst, src, i, j1, j2);
            }
        }

        /* Handle remaining 2x2 word block */
        for (x = 0; x < twin; x++) {
            RC_EXPAND_2X2_ITER(dst, src, i, j1, j2);
        }

        /* Handle any single remaining word */
        if (tail) {
            RC_EXPAND_2X2_TAIL(dst, src, i, j1, j2);
        }
    }
}
#endif
