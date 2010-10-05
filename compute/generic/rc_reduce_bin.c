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
 *  @file   rc_reduce_bin.c
 *  @brief  RAPP Compute layer binary 2x reduction.
 */

#include "rc_impl_cfg.h"   /* Implementation config */
#include "rc_platform.h"   /* Endianness            */
#include "rc_word.h"       /* Word operations       */
#include "rc_util.h"       /* RC_DIV_CEIL()         */
#include "rc_reduce_bin.h" /* Binary reduction API  */


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  Even bit word mask.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_REDUCE_EVEN RC_WORD_C8(aa)
#else
#define RC_REDUCE_EVEN RC_WORD_C8(55)
#endif


/*
 * -------------------------------------------------------------
 *  Bit packing macros
 * -------------------------------------------------------------
 */

/**
 *  Pack even bits in a word.
 */
#if RC_WORD_SIZE == 2
#define RC_REDUCE_PACK(dstw, srcw)                             \
do {                                                           \
    rc_word_t word_, part_;                                    \
    /* OR the source with itself shifted 7 steps, */           \
    /* producing bit ordering 0,8,2,10,4,12,...   */           \
    word_  = (srcw) | RC_WORD_SHL(srcw, 7);                    \
    /* Extract the first 8 bits and permute       */           \
    part_  = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_, 0, 8)]; \
    /* Store in the same bit positions            */           \
    (dstw) = RC_WORD_INSERT(part_, 0, 8);                      \
} while (0)

#elif RC_WORD_SIZE == 4
#define RC_REDUCE_PACK(dstw, srcw)                               \
do {                                                             \
    rc_word_t word_ = (srcw) | RC_WORD_SHL(srcw, 7);             \
    rc_word_t part_;                                             \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_,  0, 8)]; \
    (dstw)  = RC_WORD_INSERT(part_, 0, 8);                       \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_, 16, 8)]; \
    (dstw) |= RC_WORD_INSERT(part_, 8, 8);                       \
} while (0)

#elif RC_WORD_SIZE == 8
#define RC_REDUCE_PACK(dstw, srcw)                               \
do {                                                             \
    rc_word_t word_ = (srcw) | RC_WORD_SHL(srcw, 7);             \
    rc_word_t part_;                                             \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_,  0, 8)]; \
    (dstw)  = RC_WORD_INSERT(part_,  0, 8);                      \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_, 16, 8)]; \
    (dstw) |= RC_WORD_INSERT(part_,  8, 8);                      \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_, 32, 8)]; \
    (dstw) |= RC_WORD_INSERT(part_, 16, 8);                      \
    part_   = rc_reduce_perm_lut[RC_WORD_EXTRACT(word_, 48, 8)]; \
    (dstw) |= RC_WORD_INSERT(part_, 24, 8);                      \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  1x2 reduction macros
 * -------------------------------------------------------------
 */

/**
 *  1x2 (or 2x1) word reduction, rank 1.
 */
#define RC_REDUCE_1X2_RK1(w1, w2) \
    ((w1) | (w2))

/**
 *  2x word reduction, rank 2.
 */
#define RC_REDUCE_1X2_RK2(w1, w2) \
    ((w1) & (w2))

/**
 *  1x2 word reduction with packing.
 */
#define RC_REDUCE_1X2_WORD(wr, ws, op)              \
do {                                                \
    rc_word_t w1_ = (ws);                           \
    rc_word_t w2_ = RC_WORD_SHL(w1_, 1);            \
    /* Perform 1x2 averaging to even bits of w1_ */ \
    w1_ = op(w1_, w2_) & RC_REDUCE_EVEN;            \
    /* Pack even bits */                            \
    RC_REDUCE_PACK(wr, w1_);                        \
} while (0)

/**
 *  1x2 reduction iteration.
 */
#define RC_REDUCE_1X2_ITER(dst, src, i, j, op)          \
do {                                                    \
    rc_word_t sw1, sw2, dw1, dw2;                       \
    sw1 = RC_WORD_LOAD(&(src)[i]); (i) += RC_WORD_SIZE; \
    sw2 = RC_WORD_LOAD(&(src)[i]); (i) += RC_WORD_SIZE; \
    RC_REDUCE_1X2_WORD(dw1, sw1, op);                   \
    RC_REDUCE_1X2_WORD(dw2, sw2, op);                   \
    dw1 |= RC_WORD_SHR(dw2, 4*RC_WORD_SIZE);            \
    RC_WORD_STORE(&(dst)[j], dw1);                      \
    (j) += RC_WORD_SIZE;                                \
} while (0)

/**
 *  1x2 reduction tail iteration.
 */
#define RC_REDUCE_1X2_TAIL(dst, src, i, j, op)             \
do {                                                      \
    rc_word_t sw, dw;                                     \
    sw = RC_WORD_LOAD(&(src)[i]); (i) += RC_WORD_SIZE;    \
    RC_REDUCE_1X2_WORD(dw, sw, op);                        \
    RC_WORD_STORE(&(dst)[j], dw);                         \
    (j) += RC_WORD_SIZE;                                  \
} while (0)

/**
 *  1x2 reduction template.
 */
#define RC_REDUCE_1X2_TEMPLATE(dst, dst_dim, src, src_dim,              \
                               width, height, op, unroll)               \
do {                                                                    \
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE); /* Total #words   */ \
    int full = len / (2*(unroll));                 /* Fully unrolled */ \
    int rem  = len % (2*(unroll));                 /* Remainder      */ \
    int twin = rem / 2;                /* Remaining double src words */ \
    int tail = rem % 2;                /* Remaining single src words */ \
    int y;                                                              \
    for (y = 0; y < (height); y++) {                                    \
        int i = y*(src_dim);                                            \
        int j = y*(dst_dim);                                            \
        int x;                                                          \
        /* Handle all unrolled 1x2 word blocks */                       \
        for (x = 0; x < full; x++) {                                    \
            RC_REDUCE_1X2_ITER(dst, src, i, j, op);                     \
            if ((unroll) >= 2) {                                        \
                RC_REDUCE_1X2_ITER(dst, src, i, j, op);                 \
            }                                                           \
            if ((unroll) == 4) {                                        \
                RC_REDUCE_1X2_ITER(dst, src, i, j, op);                 \
                RC_REDUCE_1X2_ITER(dst, src, i, j, op);                 \
            }                                                           \
        }                                                               \
        /* Handle the remaining 1x2 word blocks */                      \
        for (x = 0; x < twin; x++) {                                    \
            RC_REDUCE_1X2_ITER(dst, src, i, j, op);                     \
        }                                                               \
        /* Handle any single remaining source word */                   \
        if (tail) {                                                     \
            RC_REDUCE_1X2_TAIL(dst, src, i, j, op);                     \
        }                                                               \
    }                                                                   \
} while (0)


/*
 * -------------------------------------------------------------
 *  2x1 reduction macros
 * -------------------------------------------------------------
 */

/**
 *  2x1 reduction iteration.
 */
#define RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op)        \
do {                                                      \
    rc_word_t sw1, sw2, dw;                               \
    sw1 = RC_WORD_LOAD(&(src)[i1]); (i1) += RC_WORD_SIZE; \
    sw2 = RC_WORD_LOAD(&(src)[i2]); (i2) += RC_WORD_SIZE; \
    (dw) = op(sw1, sw2);                                  \
    RC_WORD_STORE(&(dst)[j], dw);                         \
    (j) += RC_WORD_SIZE;                                  \
} while (0)

/**
 *  2x1 reduction template.
 */
#define RC_REDUCE_2X1_TEMPLATE(dst, dst_dim, src, src_dim,               \
                               width, height, op, unroll)                \
do {                                                                     \
    int h2   = (height) / 2;                                             \
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE); /* Total #words    */ \
    int full = len / (unroll);                     /* Fully unrolled  */ \
    int rem  = len % (unroll);                     /* Remaining words */ \
    int y;                                                               \
    for (y = 0; y < h2; y++) {                                           \
        int i1 = 2*y*(src_dim);                                          \
        int i2 = i1 + (src_dim);                                         \
        int j  = y*(dst_dim);                                            \
        int x;                                                           \
        /* Handle all unrolled 2x1 word blocks */                        \
        for (x = 0; x < full; x++) {                                     \
            RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op);                 \
            if ((unroll) >= 2) {                                         \
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op);             \
            }                                                            \
            if ((unroll) == 4) {                                         \
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op);             \
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op);             \
            }                                                            \
        }                                                                \
        /* Handle remaining 2x1 word blocks */                           \
        for (x = 0; x < rem; x++) {                                      \
            RC_REDUCE_2X1_ITER(dst, src, i1, i2, j, op);                 \
        }                                                                \
    }                                                                    \
} while (0)


/*
 * -------------------------------------------------------------
 *  2x2 reduction macros
 * -------------------------------------------------------------
 */

/**
 *  2x2 word reduction, rank 1.
 */
#define RC_REDUCE_2X2_RK1(w1, w2, w3, w4) \
    ((w1) | (w2) | (w3) | (w4))

/**
 *  2x2 word reduction, rank 2.
 */
#define RC_REDUCE_2X2_RK2(w1, w2, w3, w4) \
    ( ((w1) & (w3)) | ((w2) & (w4)) |     \
     (((w1) | (w3)) & ((w2) | (w4))))

/**
 *  2x2 word reduction, rank 3.
 */
#define RC_REDUCE_2X2_RK3(w1, w2, w3, w4) \
    ((((w1) & (w3)) & ((w2) | (w4))) |    \
     (((w1) | (w3)) & ((w2) & (w4))))

/**
 *  2x2 word reduction, rank 4.
 */
#define RC_REDUCE_2X2_RK4(w1, w2, w3, w4) \
    ((w1) & (w2) & (w3) & (w4))

/**
 *  2x2 word reduction with packing.
 */
#define RC_REDUCE_2X2_WORD(wr, w1, w2, op)          \
do {                                                \
    rc_word_t w1_ = (w1);                           \
    rc_word_t w2_ = (w2);                           \
    rc_word_t w3_ = RC_WORD_SHL(w1_, 1);            \
    rc_word_t w4_ = RC_WORD_SHL(w2_, 1);            \
    /* Perform 2x2 averaging to even bits of w3_ */ \
    w3_ = op(w1_, w2_, w3_, w4_) & RC_REDUCE_EVEN;  \
    /* Pack even bits */                            \
    RC_REDUCE_PACK(wr, w3_);                        \
} while (0)

/**
 *  2x2 reduction iteration.
 */
#define RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op)        \
do {                                                       \
    rc_word_t sw11, sw12, sw21, sw22, dw1, dw2;            \
    sw11 = RC_WORD_LOAD(&(src)[i1]); (i1) += RC_WORD_SIZE; \
    sw12 = RC_WORD_LOAD(&(src)[i1]); (i1) += RC_WORD_SIZE; \
    sw21 = RC_WORD_LOAD(&(src)[i2]); (i2) += RC_WORD_SIZE; \
    sw22 = RC_WORD_LOAD(&(src)[i2]); (i2) += RC_WORD_SIZE; \
    RC_REDUCE_2X2_WORD(dw1, sw11, sw21, op);               \
    RC_REDUCE_2X2_WORD(dw2, sw12, sw22, op);               \
    dw1 |= RC_WORD_SHR(dw2, 4*RC_WORD_SIZE);               \
    RC_WORD_STORE(&(dst)[j], dw1);                         \
    (j) += RC_WORD_SIZE;                                   \
} while (0)

/**
 *  2x2 reduction tail iteration.
 */
#define RC_REDUCE_2X2_TAIL(dst, src, i1, i2, j, op)       \
do {                                                      \
    rc_word_t sw1, sw2, dw;                               \
    sw1 = RC_WORD_LOAD(&(src)[i1]); (i1) += RC_WORD_SIZE; \
    sw2 = RC_WORD_LOAD(&(src)[i2]); (i2) += RC_WORD_SIZE; \
    RC_REDUCE_2X2_WORD(dw, sw1, sw2, op);                 \
    RC_WORD_STORE(&(dst)[j], dw);                         \
    (j) += RC_WORD_SIZE;                                  \
} while (0)

/**
 *  2x2 reduction template.
 */
#define RC_REDUCE_2X2_TEMPLATE(dst, dst_dim, src, src_dim,              \
                               width, height, op, unroll)               \
do {                                                                    \
    int h2   = (height) / 2;                                            \
    int len  = RC_DIV_CEIL(width, 8*RC_WORD_SIZE); /* Total #words   */ \
    int full = len / (2*(unroll));                 /* Fully unrolled */ \
    int rem  = len % (2*(unroll));                 /* Remainder      */ \
    int twin = rem / 2;                /* Remaining double src words */ \
    int tail = rem % 2;                /* Remaining single src word  */ \
    int y;                                                              \
    for (y = 0; y < h2; y++) {                                          \
        int i1 = 2*y*(src_dim);                                         \
        int i2 = i1 + (src_dim);                                        \
        int j  = y*(dst_dim);                                           \
        int x;                                                          \
        /* Handle all unrolled words */                                 \
        for (x = 0; x < full; x++) {                                    \
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op);                \
            if ((unroll) >= 2) {                                        \
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op);            \
            }                                                           \
            if ((unroll) == 4) {                                        \
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op);            \
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op);            \
            }                                                           \
        }                                                               \
        /* Handle remaining 2x2 word block */                           \
        for (x = 0; x < twin; x++) {                                    \
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j, op);                \
        }                                                               \
        /* Handle any single remaining word */                          \
        if (tail) {                                                     \
            RC_REDUCE_2X2_TAIL(dst, src, i1, i2, j, op);                \
        }                                                               \
    }                                                                   \
} while (0)


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  Permutation table with the bit mapping 0,2,4,6,1,3,5,7.
 */
static const uint8_t rc_reduce_perm_lut[256] = {
    0x00, 0x01, 0x10, 0x11, 0x02, 0x03, 0x12, 0x13,
    0x20, 0x21, 0x30, 0x31, 0x22, 0x23, 0x32, 0x33,
    0x04, 0x05, 0x14, 0x15, 0x06, 0x07, 0x16, 0x17,
    0x24, 0x25, 0x34, 0x35, 0x26, 0x27, 0x36, 0x37,
    0x40, 0x41, 0x50, 0x51, 0x42, 0x43, 0x52, 0x53,
    0x60, 0x61, 0x70, 0x71, 0x62, 0x63, 0x72, 0x73,
    0x44, 0x45, 0x54, 0x55, 0x46, 0x47, 0x56, 0x57,
    0x64, 0x65, 0x74, 0x75, 0x66, 0x67, 0x76, 0x77,
    0x08, 0x09, 0x18, 0x19, 0x0a, 0x0b, 0x1a, 0x1b,
    0x28, 0x29, 0x38, 0x39, 0x2a, 0x2b, 0x3a, 0x3b,
    0x0c, 0x0d, 0x1c, 0x1d, 0x0e, 0x0f, 0x1e, 0x1f,
    0x2c, 0x2d, 0x3c, 0x3d, 0x2e, 0x2f, 0x3e, 0x3f,
    0x48, 0x49, 0x58, 0x59, 0x4a, 0x4b, 0x5a, 0x5b,
    0x68, 0x69, 0x78, 0x79, 0x6a, 0x6b, 0x7a, 0x7b,
    0x4c, 0x4d, 0x5c, 0x5d, 0x4e, 0x4f, 0x5e, 0x5f,
    0x6c, 0x6d, 0x7c, 0x7d, 0x6e, 0x6f, 0x7e, 0x7f,
    0x80, 0x81, 0x90, 0x91, 0x82, 0x83, 0x92, 0x93,
    0xa0, 0xa1, 0xb0, 0xb1, 0xa2, 0xa3, 0xb2, 0xb3,
    0x84, 0x85, 0x94, 0x95, 0x86, 0x87, 0x96, 0x97,
    0xa4, 0xa5, 0xb4, 0xb5, 0xa6, 0xa7, 0xb6, 0xb7,
    0xc0, 0xc1, 0xd0, 0xd1, 0xc2, 0xc3, 0xd2, 0xd3,
    0xe0, 0xe1, 0xf0, 0xf1, 0xe2, 0xe3, 0xf2, 0xf3,
    0xc4, 0xc5, 0xd4, 0xd5, 0xc6, 0xc7, 0xd6, 0xd7,
    0xe4, 0xe5, 0xf4, 0xf5, 0xe6, 0xe7, 0xf6, 0xf7,
    0x88, 0x89, 0x98, 0x99, 0x8a, 0x8b, 0x9a, 0x9b,
    0xa8, 0xa9, 0xb8, 0xb9, 0xaa, 0xab, 0xba, 0xbb,
    0x8c, 0x8d, 0x9c, 0x9d, 0x8e, 0x8f, 0x9e, 0x9f,
    0xac, 0xad, 0xbc, 0xbd, 0xae, 0xaf, 0xbe, 0xbf,
    0xc8, 0xc9, 0xd8, 0xd9, 0xca, 0xcb, 0xda, 0xdb,
    0xe8, 0xe9, 0xf8, 0xf9, 0xea, 0xeb, 0xfa, 0xfb,
    0xcc, 0xcd, 0xdc, 0xdd, 0xce, 0xcf, 0xde, 0xdf,
    0xec, 0xed, 0xfc, 0xfd, 0xee, 0xef, 0xfe, 0xff
};


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Binary 1x2 reduction, rank 1.
 */
#if RC_IMPL(rc_reduce_1x2_rk1_bin, 1)
void
rc_reduce_1x2_rk1_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_1X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_1X2_RK1,
                           RC_UNROLL(rc_reduce_1x2_rk1_bin));
}
#endif


/**
 *  Binary 1x2 reduction, rank 2.
 */
#if RC_IMPL(rc_reduce_1x2_rk2_bin, 1)
void
rc_reduce_1x2_rk2_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_1X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_1X2_RK2,
                           RC_UNROLL(rc_reduce_1x2_rk2_bin));
}
#endif


/**
 *  Binary 2x1 reduction, rank 1.
 */
#if RC_IMPL(rc_reduce_2x1_rk1_bin, 1)
void
rc_reduce_2x1_rk1_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X1_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_1X2_RK1,
                           RC_UNROLL(rc_reduce_2x1_rk1_bin));
}
#endif


/**
 *  Binary 2x1 reduction, rank 2.
 */
#if RC_IMPL(rc_reduce_2x1_rk2_bin, 1)
void
rc_reduce_2x1_rk2_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X1_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_1X2_RK2,
                           RC_UNROLL(rc_reduce_2x1_rk2_bin));
}
#endif


/**
 *  Binary 2x2 reduction, rank 1.
 */
#if RC_IMPL(rc_reduce_2x2_rk1_bin, 1)
void
rc_reduce_2x2_rk1_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_2X2_RK1,
                           RC_UNROLL(rc_reduce_2x2_rk1_bin));
}
#endif


/**
 *  Binary 2x2 reduction, rank 2.
 */
#if RC_IMPL(rc_reduce_2x2_rk2_bin, 1)
void
rc_reduce_2x2_rk2_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_2X2_RK2,
                           RC_UNROLL(rc_reduce_2x2_rk2_bin));
}
#endif


/**
 *  Binary 2x2 reduction, rank 3.
 */
#if RC_IMPL(rc_reduce_2x2_rk3_bin, 1)
void
rc_reduce_2x2_rk3_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_2X2_RK3,
                           RC_UNROLL(rc_reduce_2x2_rk3_bin));
}
#endif


/**
 *  Binary 2x2 reduction, rank 4.
 */
#if RC_IMPL(rc_reduce_2x2_rk4_bin, 1)
void
rc_reduce_2x2_rk4_bin(uint8_t *restrict dst, int dst_dim,
                      const uint8_t *restrict src, int src_dim,
                      int width, int height)
{
    RC_REDUCE_2X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_REDUCE_2X2_RK4,
                           RC_UNROLL(rc_reduce_2x2_rk4_bin));
}
#endif
