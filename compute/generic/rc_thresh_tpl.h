/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_thresh_tpl.h
 *  @brief  RAPP Compute layer thresholding templates, generic implementation.
 */

#ifndef RC_THRESH_TPL_H
#define RC_THRESH_TPL_H

#include "rc_word.h" /* Word operations */

/*
 * -------------------------------------------------------------
 *  Thresholding templates
 * -------------------------------------------------------------
 */

/**
 *  Threshold-to-binary template.
 */
#define RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim,              \
                           width, height, low, high, cmp, unroll)   \
do {                                                                \
    int blk_ = (width) / (8*RC_WORD_SIZE); /* Full dst blocks    */ \
    int end_ = (width) % (8*RC_WORD_SIZE); /* Partial dst blocks */ \
                                                                    \
    if ((unroll) == 4 &&         /* Constant */                     \
        (blk_ > 0 || end_ >= 4)) /* Variable */                     \
    {                                                               \
        int len_ = end_ / 4;                                        \
        int rem_ = end_ % 4;                                        \
        RC_TEMPLATE_THRESH_(dst, dst_dim, src, src_dim, height,     \
                            blk_, len_, rem_, low, high, cmp,       \
                            RC_TEMPLATE_THRESH_BLK_X4_,             \
                            RC_TEMPLATE_THRESH_REM_X4_);            \
    }                                                               \
    else if ((unroll) >= 2 &&         /* Constant */                \
             (blk_ > 0 || end_ >= 2)) /* Variable */                \
    {                                                               \
        int len_ = end_ / 2;                                        \
        int rem_ = end_ % 2;                                        \
        RC_TEMPLATE_THRESH_(dst, dst_dim, src, src_dim, height,     \
                            blk_, len_, rem_, low, high, cmp,       \
                            RC_TEMPLATE_THRESH_BLK_X2_,             \
                            RC_TEMPLATE_THRESH_REM_X2_);            \
    }                                                               \
    else {                                                          \
        RC_TEMPLATE_THRESH_(dst, dst_dim, src, src_dim, height,     \
                            blk_, 0, end_, low, high, cmp,          \
                            RC_TEMPLATE_THRESH_BLK_X1_,             \
                            RC_TEMPLATE_THRESH_REM_NONE_);          \
    }                                                               \
} while (0)


/*
 * -------------------------------------------------------------
 *  Internal support macros for thresholding template
 * -------------------------------------------------------------
 */

/**
 *  Thresholding template driver.
 */
#define RC_TEMPLATE_THRESH_(dst, dst_dim, src, src_dim, height,  \
                            blk, len, rem, thr1, thr2, cmp,      \
                            loop_blk, loop_rem)                  \
do {                                                             \
    int y_;                                                      \
    for (y_ = 0; y_ < (height); y_++) {                          \
        int i_ = y_*(src_dim);                                   \
        int j_ = y_*(dst_dim);                                   \
        int x_;                                                  \
                                                                 \
        /* Handle all full destination word blocks */            \
        for (x_ = 0; x_ < (blk); x_++, j_ += RC_WORD_SIZE) {     \
            loop_blk(&(dst)[j_], src, i_, cmp, thr1, thr2);      \
        }                                                        \
                                                                 \
        /* Handle partial destination words */                   \
        if ((len) || (rem)) {                                    \
            rc_word_t acc_ = RC_WORD_ZERO;                       \
            int       pos_ = 0;                                  \
                                                                 \
            /* Handle unrolled source pixels */                  \
            loop_rem(src, acc_, i_, pos_, len, cmp, thr1, thr2); \
                                                                 \
            /* Handle any remaining source pixels */             \
            for (x_ = 0; x_ < (rem); x_++) {                     \
                RC_TEMPLATE_THRESH_ITER_(src, acc_, i_, pos_,    \
                                         cmp, thr1, thr2);       \
            }                                                    \
                                                                 \
            /* Store the partial word */                         \
            RC_WORD_STORE(&(dst)[j_], acc_);                     \
        }                                                        \
    }                                                            \
} while (0)

/**
 *  Thresholding template block iterator, no unrolling.
 */
#define RC_TEMPLATE_THRESH_BLK_X1_(dst, src, idx, cmp, thr1, thr2)      \
do {                                                                    \
    rc_word_t acc_ = RC_WORD_ZERO;                                      \
    int       k_, b_;                                                   \
    for (k_ = 0, b_ = 0; k_ < (int)(8*RC_WORD_SIZE); k_++) {            \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
    }                                                                   \
    RC_WORD_STORE(dst, acc_);                                           \
} while (0)

/**
 *  Thresholding template block iterator, unrolled two times.
 */
#define RC_TEMPLATE_THRESH_BLK_X2_(dst, src, idx, cmp, thr1, thr2)      \
do {                                                                    \
    rc_word_t acc_ = RC_WORD_ZERO;                                      \
    int       k_, b_;                                                   \
    for (k_ = 0, b_ = 0; k_ < (int)(8*RC_WORD_SIZE); k_ += 2) {         \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
    }                                                                   \
    RC_WORD_STORE(dst, acc_);                                           \
} while (0)

/**
 *  Thresholding template block iterator, unrolled four times.
 */
#define RC_TEMPLATE_THRESH_BLK_X4_(dst, src, idx, cmp, thr1, thr2)      \
do {                                                                    \
    rc_word_t acc_ = RC_WORD_ZERO;                                      \
    int       k_, b_;                                                   \
    for (k_ = 0, b_ = 0; k_ < (int)(8*RC_WORD_SIZE); k_ += 4) {         \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc_, idx, b_, cmp, thr1, thr2); \
    }                                                                   \
    RC_WORD_STORE(dst, acc_);                                           \
} while (0)

/**
 *  Thresholding template empty remainder iterator.
 */
#define RC_TEMPLATE_THRESH_REM_NONE_(src, acc, idx, pos,  \
                                     len, cmp, thr1, thr2)

/**
 *  Thresholding template remainder iterator, unrolled two times.
 */
#define RC_TEMPLATE_THRESH_REM_X2_(src, acc, idx, pos,                  \
                                   len, cmp, thr1, thr2)                \
do {                                                                    \
    int k_;                                                             \
    for (k_ = 0; k_ < (len); k_++) {                                    \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
    }                                                                   \
} while (0)

/**
 *  Thresholding template remainder iterator, unrolled four times.
 */
#define RC_TEMPLATE_THRESH_REM_X4_(src, acc, idx, pos,                  \
                                   len, cmp, thr1, thr2)                \
do {                                                                    \
    int k_;                                                             \
    for (k_ = 0; k_ < (len); k_++) {                                    \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
         RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2); \
    }                                                                   \
} while (0)

/**
 *  Thresholding template iteration.
 */
#define RC_TEMPLATE_THRESH_ITER_(src, acc, idx, pos, cmp, thr1, thr2) \
do {                                                                  \
    int bit_ = cmp((src)[idx], thr1, thr2);                           \
    (acc) |= RC_WORD_INSERT(bit_, pos, 1);                            \
    (idx)++;                                                          \
    (pos)++;                                                          \
} while (0)

#endif /* RC_THRESH_TPL_H */
