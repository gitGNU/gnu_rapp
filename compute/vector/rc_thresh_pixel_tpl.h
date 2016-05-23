/*  Copyright (C) 2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_thresh_pixel_tpl.h
 *  @brief  RAPP Compute layer pixelwise thresholding templates.
 */

#ifndef RC_THRESH_PIXEL_TPL_H
#define RC_THRESH_PIXEL_TPL_H

#include <stdlib.h> /* Need NULL. */
#include "rc_util.h" /* RC_DIV_CEIL(). */
#include "rc_word.h" /* Word operations. */
#include "rc_vector.h" /* Vector operations. */

/*
 * -------------------------------------------------------------
 *  Exported pixelwise thresholding template.
 * -------------------------------------------------------------
 */
#define RC_THRESH_PIXEL_TEMPLATE_SINGLE(dst, dst_dim, src, src_dim,     \
                                        thresh, thresh_dim,             \
                                        width, height, cmp, unroll)     \
do {                                                                    \
    const uint8_t *dummy = NULL;                                        \
    RC_THRESH_PIXEL_TEMPLATE_X(dst, dst_dim, src, src_dim,              \
                               thresh, thresh_dim, dummy, 0,            \
                               width, height, cmp, unroll,              \
                               RC_THRESH_PIXEL_SINGLE_ARG);             \
} while (0)

#define RC_THRESH_PIXEL_TEMPLATE_DOUBLE(dst, dst_dim, src, src_dim,     \
                                        low, low_dim, high, high_dim,   \
                                        width, height, cmp, unroll)     \
    RC_THRESH_PIXEL_TEMPLATE_X(dst, dst_dim, src, src_dim,              \
                               low, low_dim, high, high_dim,            \
                               width, height, cmp, unroll,              \
                               RC_THRESH_PIXEL_DOUBLE_ARG)


/*
 * -------------------------------------------------------------
 *  Internal thresholding templates.
 * -------------------------------------------------------------
 */

#define RC_THRESH_PIXEL_SINGLE_ARG (1)
#define RC_THRESH_PIXEL_DOUBLE_ARG (2)

/**
 *  The pixelwise thresholding template.
 *  Use the word version unless it's unavailable or if hinted otherwise.
 */

#if RC_VEC_SIZE >= 8 && defined RC_VEC_GETMASKV                && \
    (defined RC_VEC_HINT_GETMASKV || !defined RC_VEC_GETMASKW) && \
     defined RC_VEC_SPLAT         &&  defined RC_VEC_ALIGNC    && \
     defined RC_VEC_SHINIT        &&  defined RC_VEC_SHL       && \
     defined RC_VEC_ZERO
#define RC_THRESH_PIXEL_TEMPLATE_X RC_THRESH_PIXEL_VEC_X

#elif defined RC_VEC_SPLAT && defined RC_VEC_GETMASKW
#define RC_THRESH_PIXEL_TEMPLATE_X RC_THRESH_PIXEL_WORD_X
#endif

/**
 *  To keep "exported" macros first, without mixing with internal
 *  macros, we have to optimistically define them first, then undefine
 *  them if (accumulated) prerequisites aren't met.
 */
#ifndef RC_THRESH_PIXEL_TEMPLATE_X
#undef RC_THRESH_PIXEL_TEMPLATE_SINGLE
#undef RC_THRESH_PIXEL_TEMPLATE_DOUBLE
#endif

/**
 *  Pixelwise threshold-to-binary template using a word accumulator.
 */
#define RC_THRESH_PIXEL_WORD_X(dst, dst_dim, src, src_dim,                        \
                               low, low_dim, high, high_dim,                      \
                               width, height, cmp, unroll, num_args)              \
do {                                                                              \
    int      div_ = (8*RC_WORD_SIZE) / RC_VEC_SIZE;  /* Vectors per word. */      \
    int      tot_ = RC_DIV_CEIL(width, RC_VEC_SIZE); /* Num src vectors. */       \
    int      blk_ = tot_ / div_;                     /* Full dst blocks. */       \
    int      end_ = tot_ % div_;                     /* Partial dst blocks. */    \
                                                                                  \
    RC_VEC_DECLARE();                                                             \
                                                                                  \
    if ((unroll) == 4 &&                             /* Constant. */              \
        (8*RC_WORD_SIZE) / RC_VEC_SIZE >= 4 &&       /* Constant. */              \
        (blk_ > 0 || end_ >= 4))                     /* Variable. */              \
    {                                                                             \
        int len_ = end_ / 4;                                                      \
        int rem_ = end_ % 4;                                                      \
        RC_THRESH_PIXEL_WORD_DRV_(dst, dst_dim, src, src_dim,                     \
                                  low, low_dim, high, high_dim,                   \
                                  height, blk_, len_, rem_,                       \
                                  cmp, RC_THRESH_PIXEL_WORD_BLK_X4_,              \
                                  RC_THRESH_PIXEL_WORD_REM_X4_, num_args);        \
    }                                                                             \
    else if ((unroll) >= 2 &&                       /* Constant. */               \
             (8*RC_WORD_SIZE) / RC_VEC_SIZE >= 2 && /* Constant. */               \
             (blk_ > 0 || end_ >= 2))               /* Variable. */               \
    {                                                                             \
        int len_ = end_ / 2;                                                      \
        int rem_ = end_ % 2;                                                      \
        RC_THRESH_PIXEL_WORD_DRV_(dst, dst_dim, src, src_dim,                     \
                                  low, low_dim, high, high_dim,                   \
                                  height, blk_, len_, rem_,                       \
                                  cmp, RC_THRESH_PIXEL_WORD_BLK_X2_,              \
                                  RC_THRESH_PIXEL_WORD_REM_X2_, num_args);        \
    }                                                                             \
    else {                                                                        \
        RC_THRESH_PIXEL_WORD_DRV_(dst, dst_dim, src, src_dim,                     \
                                  low, low_dim, high, high_dim,                   \
                                  height, blk_, 0, end_,                          \
                                  cmp, RC_THRESH_PIXEL_WORD_BLK_X1_,              \
                                  RC_THRESH_PIXEL_WORD_REM_NONE_, num_args);      \
    }                                                                             \
    RC_VEC_CLEANUP();                                                             \
} while (0)


/**
 *  Pixelwise threshold-to-binary template using a vector accumulator.
 */
#define RC_THRESH_PIXEL_VEC_X(dst, dst_dim, src, src_dim,                        \
                              low, low_dim, high, high_dim,                      \
                              width, height, cmp, unroll, num_args)              \
do {                                                                             \
    int      tot_ = RC_DIV_CEIL(width, RC_VEC_SIZE); /* Num src vectors. */      \
    int      blk_ = tot_ / 8;                        /* Full dst blocks. */      \
    int      end_ = tot_ % 8;                        /* Partial dst blocks. */   \
                                                                                 \
    RC_VEC_DECLARE();                                                            \
                                                                                 \
    if ((unroll) == 4 &&                             /* Constant. */             \
        (blk_ > 0 || end_ >= 4))                     /* Variable. */             \
    {                                                                            \
        int len_ = end_ / 4;                                                     \
        int rem_ = end_ % 4;                                                     \
        RC_THRESH_PIXEL_VEC_DRV_(dst, dst_dim, src, src_dim,                     \
                                 low, low_dim, high, high_dim,                   \
                                 height, blk_, end_, len_, rem_, cmp,            \
                                 RC_THRESH_PIXEL_VEC_BLK_X4_,                    \
                                 RC_THRESH_PIXEL_VEC_REM_X4_, num_args);         \
    }                                                                            \
    else if ((unroll) >= 2 &&                        /* Constant. */             \
             (blk_ > 0 || end_ >= 2))                /* Variable. */             \
    {                                                                            \
        int len_ = end_ / 2;                                                     \
        int rem_ = end_ % 2;                                                     \
        RC_THRESH_PIXEL_VEC_DRV_(dst, dst_dim, src, src_dim,                     \
                                 low, low_dim, high, high_dim,                   \
                                 height, blk_, end_, len_, rem_, cmp,            \
                                 RC_THRESH_PIXEL_VEC_BLK_X2_,                    \
                                 RC_THRESH_PIXEL_VEC_REM_X2_, num_args);         \
    }                                                                            \
    else {                                                                       \
        RC_THRESH_PIXEL_VEC_DRV_(dst, dst_dim, src, src_dim,                     \
                                 low, low_dim, high, high_dim,                   \
                                 height, blk_, end_, 0, end_, cmp,               \
                                 RC_THRESH_PIXEL_VEC_BLK_X1_,                    \
                                 RC_THRESH_PIXEL_VEC_REM_NONE_, num_args);       \
    }                                                                            \
    RC_VEC_CLEANUP();                                                            \
} while (0)


/*
 * ----------------------------------------------------------------
 *  Internal support macros for word-based thresholding template.
 * ----------------------------------------------------------------
 */

/**
 *  Pixelwise thresholding template word driver.
 */
#define RC_THRESH_PIXEL_WORD_DRV_(dst, dst_dim, src, src_dim,                      \
                                  thr1, thr1_dim, thr2, thr2_dim,                  \
                                  height, blk, len, rem,                           \
                                  cmp, loop_blk, loop_rem, num_args)               \
do {                                                                               \
    int y_;                                                                        \
    for (y_ = 0; y_ < (height); y_++) {                                            \
        int i_ = y_*(src_dim);                                                     \
        int j_ = y_*(dst_dim);                                                     \
        int m_ = y_*(thr1_dim);                                                    \
        int n_ = y_*(thr2_dim);                                                    \
        int x_;                                                                    \
                                                                                   \
        /* Handle all full destination word blocks. */                             \
        for (x_ = 0; x_ < (blk); x_++, j_ += RC_WORD_SIZE) {                       \
            loop_blk(&(dst)[j_], src, i_, cmp, thr1, m_, thr2, n_, num_args);      \
        }                                                                          \
                                                                                   \
        /* Handle partial destination words. */                                    \
        if ((len) || (rem)) {                                                      \
            rc_word_t acc_ = RC_WORD_ZERO;                                         \
            int       pos_ = 0;                                                    \
                                                                                   \
            /* Handle unrolled source vectors. */                                  \
            loop_rem(src, acc_, i_, pos_, len, cmp, thr1, m_, thr2, n_, num_args); \
                                                                                   \
            /* Handle any remaining source vectors. */                             \
            for (x_ = 0; x_ < (rem); x_++) {                                       \
                RC_THRESH_PIXEL_WORD_ITER_(src, acc_, i_, pos_,                    \
                                           cmp, thr1, m_, thr2, n_, num_args);     \
            }                                                                      \
                                                                                   \
            /* Store the partial word. */                                          \
            RC_WORD_STORE(&(dst)[j_], acc_);                                       \
        }                                                                          \
    }                                                                              \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, no unrolling.
 */
#define RC_THRESH_PIXEL_WORD_BLK_X1_(dst, src, idx, cmp,                       \
                                     thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                           \
    rc_word_t acc_ = RC_WORD_ZERO;                                             \
    int       k_, b_;                                                          \
    for (k_ = 0, b_ = 0;                                                       \
         k_ < (int)((8*RC_WORD_SIZE) / RC_VEC_SIZE);                           \
         k_++)                                                                 \
    {                                                                          \
        RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                    \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
    }                                                                          \
    RC_WORD_STORE(dst, acc_);                                                  \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, unrolled two times.
 */
#define RC_THRESH_PIXEL_WORD_BLK_X2_(dst, src, idx, cmp,                       \
                                     thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                           \
    rc_word_t acc_ = RC_WORD_ZERO;                                             \
    int       k_, b_;                                                          \
    for (k_ = 0, b_ = 0;                                                       \
         k_ < (int)((8*RC_WORD_SIZE) / RC_VEC_SIZE);                           \
         k_ += 2)                                                              \
    {                                                                          \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                   \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                   \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args); \
    }                                                                          \
    RC_WORD_STORE(dst, acc_);                                                  \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, unrolled four times.
 */
#define RC_THRESH_PIXEL_WORD_BLK_X4_(dst, src, idx, cmp,                       \
                                     thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                           \
    rc_word_t acc_ = RC_WORD_ZERO;                                             \
    int       k_, b_;                                                          \
    for (k_ = 0, b_ = 0;                                                       \
         k_ < (int)((8*RC_WORD_SIZE) / RC_VEC_SIZE);                           \
         k_ += 4)                                                              \
    {                                                                          \
        RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                    \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
        RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                    \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
        RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                    \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
        RC_THRESH_PIXEL_WORD_ITER_(src, acc_, idx, b_, cmp,                    \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
    }                                                                          \
    RC_WORD_STORE(dst, acc_);                                                  \
} while (0)

/**
 *  Pixelwise thresholding template empty remainder iterator.
 */
#define RC_THRESH_PIXEL_WORD_REM_NONE_(src, acc, idx, pos,        \
                                       len, cmp, thr1, thr1_idx,  \
                                       thr2, thr2_idx, num_args)

/**
 *  Pixelwise thresholding template remainder iterator, unrolled two times.
 */
#define RC_THRESH_PIXEL_WORD_REM_X2_(src, acc, idx, pos,                       \
                                     len, cmp, thr1, thr1_idx,                 \
                                     thr2, thr2_idx, num_args)                 \
do {                                                                           \
    int k_;                                                                    \
    for (k_ = 0; k_ < (len); k_++) {                                           \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                   \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                   \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args); \
    }                                                                          \
} while (0)

/**
 *  Pixelwise thresholding template remainder iterator, unrolled four times.
 */
#define RC_THRESH_PIXEL_WORD_REM_X4_(src, acc, idx, pos,                                 \
                                     len, cmp, thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                                     \
    int k_;                                                                              \
    for (k_ = 0; k_ < (len); k_++) {                                                     \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                             \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args);           \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                             \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args);           \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                             \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args);           \
         RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                             \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args);           \
    }                                                                                    \
} while (0)

/**
 *  Pixelwise thresholding template iteration.
 */
#define RC_THRESH_PIXEL_WORD_ITER_(src, acc, idx, pos, cmp,                     \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args)    \
do {                                                                            \
    rc_vec_t vec_;                                                              \
    rc_vec_t thr1_vec_;                                                         \
    rc_vec_t thr2_vec_;                                                         \
    unsigned mask_;                                                             \
                                                                                \
    RC_VEC_LOAD(vec_, &(src)[(idx)]);                   /* Load vector data. */ \
    if ((num_args) == RC_THRESH_PIXEL_DOUBLE_ARG) {                             \
        RC_VEC_LOAD(thr1_vec_, &(thr1)[(thr1_idx)]);    /* Load vector data. */ \
        RC_VEC_LOAD(thr2_vec_, &(thr2)[(thr2_idx)]);    /* Load vector data. */ \
        (thr1_idx) += RC_VEC_SIZE;                                              \
        (thr2_idx) += RC_VEC_SIZE;                                              \
    }                                                                           \
    else {                                                                      \
        RC_VEC_LOAD(thr1_vec_, &(thr1)[(thr1_idx)]);                            \
        (thr1_idx) += RC_VEC_SIZE;                                              \
        (void)thr2_vec_;                                                        \
    }                                                                           \
    cmp(vec_, vec_, thr1_vec_, thr2_vec_);            /* Cmp to thresholds. */  \
    RC_VEC_GETMASKW(mask_, vec_);                     /* Pack to binary. */     \
    (acc) |= RC_WORD_INSERT(mask_, pos, RC_VEC_SIZE); /* Update accum. */       \
    (idx) += RC_VEC_SIZE;                             /* Advance src index. */  \
    (pos) += RC_VEC_SIZE;                             /* Advance acc pos. */    \
} while (0)


/*
 * -------------------------------------------------------------
 *  Internal support macros for vector-based thresholding.
 * -------------------------------------------------------------
 */

/**
 *  Pixelwise thresholding template vector driver.
 */
#define RC_THRESH_PIXEL_VEC_DRV_(dst, dst_dim, src, src_dim,                      \
                                 thr1, thr1_dim, thr2, thr2_dim,                  \
                                 height, blk, end, len, rem,  cmp,                \
                                 loop_blk, loop_rem, num_args)                    \
do {                                                                              \
    rc_vec_t shv_;                                                                \
    int      y_;                                                                  \
                                                                                  \
    /* Set the remainder alignment shift vector. */                               \
    RC_VEC_SHINIT(shv_, RC_VEC_SIZE - (end)*RC_VEC_SIZE/8);                       \
                                                                                  \
    /* Process all rows. */                                                       \
    for (y_ = 0; y_ < (height); y_++) {                                           \
        int i_ = y_*(src_dim);                                                    \
        int j_ = y_*(dst_dim);                                                    \
        int m_ = y_*(thr1_dim);                                                   \
        int n_ = y_*(thr2_dim);                                                   \
        int x_;                                                                   \
                                                                                  \
        /* Handle all full destination vector blocks. */                          \
        for (x_ = 0; x_ < (blk); x_++, j_ += RC_VEC_SIZE) {                       \
            loop_blk(&(dst)[j_], src, i_, cmp, thr1, m_, thr2, n_, num_args);     \
        }                                                                         \
                                                                                  \
        /* Handle partial destination words. */                                   \
        if ((len) || (rem)) {                                                     \
            rc_vec_t acc_;                                                        \
                                                                                  \
            /* Handle unrolled source vectors. */                                 \
            RC_VEC_ZERO(acc_);                                                    \
            loop_rem(src, acc_, i_, len, cmp, thr1, m_, thr2, n_, num_args);      \
                                                                                  \
            /* Handle any remaining source vectors. */                            \
            for (x_ = 0; x_ < (rem); x_++) {                                      \
                RC_THRESH_PIXEL_VEC_ITER_(src, acc_, i_, cmp,                     \
                                          thr1, m_, thr2, n_, num_args);          \
            }                                                                     \
                                                                                  \
            /* Shift and store the partial dst vector. */                         \
            RC_VEC_SHL(acc_, acc_, shv_);                                         \
            RC_VEC_STORE(&(dst)[j_], acc_);                                       \
        }                                                                         \
    }                                                                             \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, no unrolling.
 */
#define RC_THRESH_PIXEL_VEC_BLK_X1_(dst, src, idx, cmp,                        \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args)  \
do {                                                                           \
    rc_vec_t acc_;                                                             \
    int      k_;                                                               \
    RC_VEC_ZERO(acc_);                                                         \
    for (k_ = 0; k_ < 8; k_++) {                                               \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
    }                                                                          \
    RC_VEC_STORE(dst, acc_);                                                   \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, unrolled two times.
 */
#define RC_THRESH_PIXEL_VEC_BLK_X2_(dst, src, idx, cmp,                        \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args)  \
do {                                                                           \
    rc_vec_t acc_;                                                             \
    int      k_;                                                               \
    RC_VEC_ZERO(acc_);                                                         \
    for (k_ = 0; k_ < 4; k_++) {                                               \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
    }                                                                          \
    RC_VEC_STORE(dst, acc_);                                                   \
} while (0)

/**
 *  Pixelwise thresholding template block iterator, unrolled four times.
 */
#define RC_THRESH_PIXEL_VEC_BLK_X4_(dst, src, idx, cmp,                        \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args)  \
do {                                                                           \
    rc_vec_t acc_;                                                             \
    int      k_;                                                               \
    RC_VEC_ZERO(acc_);                                                         \
    for (k_ = 0; k_ < 2; k_++) {                                               \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc_, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args);  \
    }                                                                          \
    RC_VEC_STORE(dst, acc_);                                                   \
} while (0)

/**
 *  Pixelwise thresholding template empty remainder iterator.
 */
#define RC_THRESH_PIXEL_VEC_REM_NONE_(src, acc, idx, len, cmp,                \
                                      thr1, thr1_idx, thr2, thr2_idx, num_args)

/**
 *  Pixelwise thresholding template remainder iterator, unrolled two times.
 */
#define RC_THRESH_PIXEL_VEC_REM_X2_(src, acc, idx, len, cmp,                  \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                          \
    int k_;                                                                   \
    for (k_ = 0; k_ < (len); k_++) {                                          \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
    }                                                                         \
} while (0)

/**
 *  Pixelwise thresholding template remainder iterator, unrolled four times.
 */
#define RC_THRESH_PIXEL_VEC_REM_X4_(src, acc, idx, len, cmp,                  \
                                    thr1, thr1_idx, thr2, thr2_idx, num_args) \
do {                                                                          \
    int k_;                                                                   \
    for (k_ = 0; k_ < (len); k_++) {                                          \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
         RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                        \
                                   thr1, thr1_idx, thr2, thr2_idx, num_args); \
    }                                                                         \
} while (0)

/**
 *  Pixelwise thresholding template iteration.
 */
#define RC_THRESH_PIXEL_VEC_ITER_(src, acc, idx, cmp,                             \
                                  thr1, thr1_idx, thr2, thr2_idx, num_args)       \
do {                                                                              \
    rc_vec_t vec_;                                                                \
    rc_vec_t thr1_vec_;                                                           \
    rc_vec_t thr2_vec_;                                                           \
    rc_vec_t mask_;                                                               \
                                                                                  \
    RC_VEC_LOAD(vec_, &(src)[(idx)]);                     /* Load vector data. */ \
    if ((num_args) == RC_THRESH_PIXEL_DOUBLE_ARG) {                               \
        RC_VEC_LOAD(thr1_vec_, &(thr1)[(thr1_idx)]);      /* Load vector data. */ \
        RC_VEC_LOAD(thr2_vec_, &(thr2)[(thr2_idx)]);      /* Load vector data. */ \
        (thr1_idx) += RC_VEC_SIZE;                                                \
        (thr2_idx) += RC_VEC_SIZE;                                                \
    }                                                                             \
    else {                                                                        \
        RC_VEC_LOAD(thr1_vec_, &(thr1)[(thr1_idx)]);                              \
        (thr1_idx) += RC_VEC_SIZE;                                                \
        (void)thr2_vec_;                                                          \
    }                                                                             \
    cmp(vec_, vec_, thr1_vec_, thr2_vec_);           /* Cmp to thresholds. */     \
    RC_VEC_GETMASKV(mask_, vec_);                    /* Pack to binary. */        \
    RC_VEC_ALIGNC(acc, acc, mask_, RC_VEC_SIZE / 8); /* Update accum. */          \
    (idx) += RC_VEC_SIZE;                            /* Advance src index. */     \
} while (0)

#endif /* RC_THRESH_PIXEL_TPL_H */
