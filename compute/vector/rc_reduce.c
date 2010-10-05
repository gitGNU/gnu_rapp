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
 *  @file   rc_reduce.c
 *  @brief  RAPP Compute layer 8-bit 2x reduction, vector implementation.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_util.h"     /* RC_DIV_CEIL()         */
#include "rc_vector.h"   /* Vector operations     */
#include "rc_reduce.h"   /* 8-bit reduction API   */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  The fastest AVG vector operation.
 */
#ifdef RC_VEC_HINT_AVGT
#define RC_REDUCE_AVG RC_VEC_AVGT
#elif defined RC_VEC_AVGR
#define RC_REDUCE_AVG RC_VEC_AVGR
#endif

/**
 *  1x2 reduction vector operation.
 */
#if defined RC_VEC_SHLC && defined RC_REDUCE_AVG
#define RC_REDUCE_1X2_OP(dstv, srcv) \
do {                                 \
    rc_vec_t tmpv;                   \
    RC_VEC_SHLC(tmpv, srcv, 1);      \
    RC_REDUCE_AVG(dstv, srcv, tmpv); \
} while (0)
#endif

/**
 *  2x2 reduction vector operation.
 */
#if defined RC_VEC_AVGT && defined RC_VEC_SHLC && defined RC_VEC_AVGR
#define RC_REDUCE_2X2_OP(dstv, sv1, sv2) \
do {                                     \
    rc_vec_t u1, u2;                     \
    RC_VEC_AVGT(u1, sv1, sv2);           \
    RC_VEC_SHLC(u2, u1, 1);              \
    RC_VEC_AVGR(dstv, u1, u2);           \
} while (0)
#endif

/**
 *  1x2 reduction iteration.
 */
#if defined RC_REDUCE_1X2_OP && defined RC_VEC_PACK && defined RC_VEC_ZERO
#define RC_REDUCE_1X2_ITER(dst, src, i, j)           \
do {                                                 \
    rc_vec_t sv1, sv2, dv1, dv2;                     \
    RC_VEC_LOAD(sv1, &(src)[i]); (i) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv2, &(src)[i]); (i) += RC_VEC_SIZE; \
    RC_REDUCE_1X2_OP(dv1, sv1);                      \
    RC_REDUCE_1X2_OP(dv2, sv2);                      \
    RC_VEC_PACK(dv1, dv1, dv2);                      \
    RC_VEC_STORE(&(dst)[j], dv1);                    \
    (j) += RC_VEC_SIZE;                              \
} while (0)

/**
 *  1x2 reduction tail (partial) iteration.
 */
#define RC_REDUCE_1X2_TAIL(dst, src, i, j)          \
do {                                                \
    rc_vec_t sv, dv, zero;                          \
    RC_VEC_LOAD(sv, &(src)[i]); (i) += RC_VEC_SIZE; \
    RC_VEC_ZERO(zero);                              \
    RC_REDUCE_1X2_OP(dv, sv);                       \
    RC_VEC_PACK(dv, dv, zero);                      \
    RC_VEC_STORE(&(dst)[j], dv);                    \
    (j) += RC_VEC_SIZE;                             \
} while (0)
#endif /* RC_REDUCE_1X2_OP && RC_VEC_PACK && RC_VEC_ZERO */

/**
 *  2x1 reduction iteration.
 */
#ifdef RC_REDUCE_AVG
#define RC_REDUCE_2X1_ITER(dst, src, i1, i2, j)        \
do {                                                   \
    rc_vec_t sv1, sv2, dv;                             \
    RC_VEC_LOAD(sv1, &(src)[i1]); (i1) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv2, &(src)[i2]); (i2) += RC_VEC_SIZE; \
    RC_REDUCE_AVG(dv, sv1, sv2);                       \
    RC_VEC_STORE(&(dst)[j], dv);                       \
    (j) += RC_VEC_SIZE;                                \
} while (0)
#endif

/**
 *  2x2 reduction iteration.
 */
#if defined RC_REDUCE_2X2_OP && defined RC_VEC_PACK && defined RC_VEC_ZERO
#define RC_REDUCE_2X2_ITER(dst, src, i1, i2, j)          \
do {                                                    \
    rc_vec_t sv11, sv12, sv21, sv22, dv1, dv2;          \
    RC_VEC_LOAD(sv11, &(src)[i1]); (i1) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv12, &(src)[i1]); (i1) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv21, &(src)[i2]); (i2) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv22, &(src)[i2]); (i2) += RC_VEC_SIZE; \
    RC_REDUCE_2X2_OP(dv1, sv11, sv21);                  \
    RC_REDUCE_2X2_OP(dv2, sv12, sv22);                  \
    RC_VEC_PACK(dv1, dv1, dv2);                         \
    RC_VEC_STORE(&(dst)[j], dv1);                       \
    (j) += RC_VEC_SIZE;                                 \
} while (0)

/**
 *  2x2 reduction tail (partial) iteration.
 */
#define RC_REDUCE_2X2_TAIL(dst, src, i1, i2, j)         \
do {                                                    \
    rc_vec_t sv11, sv21, dv, zero;                      \
    RC_VEC_LOAD(sv11, &(src)[i1]); (i1) += RC_VEC_SIZE; \
    RC_VEC_LOAD(sv21, &(src)[i2]); (i2) += RC_VEC_SIZE; \
    RC_VEC_ZERO(zero);                                  \
    RC_REDUCE_2X2_OP(dv, sv11, sv21);                   \
    RC_VEC_PACK(dv, dv, zero);                          \
    RC_VEC_STORE(&(dst)[j], dv);                        \
    (j) += RC_VEC_SIZE;                                 \
} while (0)
#endif /* RC_REDUCE_2X2_OP && RC_VEC_PACK && RC_VEC_ZERO */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  8-bit 1x2 (horizontal) reduction.
 */
#if RC_IMPL(rc_reduce_1x2_u8, 1)
#ifdef RC_REDUCE_1X2_ITER
void
rc_reduce_1x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int len  = RC_DIV_CEIL(width, RC_VEC_SIZE);       /* Total #vectors */
    int full = len / (2*RC_UNROLL(rc_reduce_1x2_u8)); /* Fully unrolled */
    int rem  = len % (2*RC_UNROLL(rc_reduce_1x2_u8)); /* Remainder      */
    int twin = rem / 2;                 /* Remaining double src vectors */
    int tail = rem % 2;                 /* Remaining single src vector  */
    int y;

    RC_VEC_DECLARE();

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all unrolled 1x2 vector blocks */
        for (x = 0; x < full; x++) {
            RC_REDUCE_1X2_ITER(dst, src, i, j);
            if (RC_UNROLL(rc_reduce_1x2_u8) >= 2) {
                RC_REDUCE_1X2_ITER(dst, src, i, j);
            }
            if (RC_UNROLL(rc_reduce_1x2_u8) == 4) {
                RC_REDUCE_1X2_ITER(dst, src, i, j);
                RC_REDUCE_1X2_ITER(dst, src, i, j);
            }
        }

        /* Handle the remaining 1x2 vector blocks */
        for (x = 0; x < twin; x++) {
            RC_REDUCE_1X2_ITER(dst, src, i, j);
        }

        /* Handle any single remaining source vector */
        if (tail) {
            RC_REDUCE_1X2_TAIL(dst, src, i, j);
        }
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  8-bit 2x1 (vertical) reduction.
 */
#if RC_IMPL(rc_reduce_2x1_u8, 1)
#ifdef RC_REDUCE_2X1_ITER
void
rc_reduce_2x1_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int h2   = height / 2;
    int len  = RC_DIV_CEIL(width, RC_VEC_SIZE);   /* Total #vectors    */
    int full = len / RC_UNROLL(rc_reduce_2x1_u8); /* Fully unrolled    */
    int rem  = len % RC_UNROLL(rc_reduce_2x1_u8); /* Remaining vectors */
    int y;

    RC_VEC_DECLARE();

    for (y = 0; y < h2; y++) {
        int i1 = 2*y*src_dim;
        int i2 = i1 + src_dim;
        int j  = y*dst_dim;
        int x;

        /* Handle all unrolled 2x1 vector blocks */
        for (x = 0; x < full; x++) {
            RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            if (RC_UNROLL(rc_reduce_2x1_u8) >= 2) {
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            }
            if (RC_UNROLL(rc_reduce_2x1_u8) == 4) {
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
                RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
            }
        }

        /* Handle remaining 2x1 vector blocks */
        for (x = 0; x < rem; x++) {
            RC_REDUCE_2X1_ITER(dst, src, i1, i2, j);
        }
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  8-bit 2x2 reduction.
 */
#if RC_IMPL(rc_reduce_2x2_u8, 1)
#ifdef RC_REDUCE_2X2_ITER
void
rc_reduce_2x2_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    int h2   = height / 2;
    int len  = RC_DIV_CEIL(width, RC_VEC_SIZE);       /* Total #vectors */
    int full = len / (2*RC_UNROLL(rc_reduce_2x2_u8)); /* Fully unrolled */
    int rem  = len % (2*RC_UNROLL(rc_reduce_2x2_u8)); /* Remainder      */
    int twin = rem / 2;                 /* Remaining double src vectors */
    int tail = rem % 2;                 /* Remaining single src vector  */
    int y;

    RC_VEC_DECLARE();

    for (y = 0; y < h2; y++) {
        int i1 = 2*y*src_dim;
        int i2 = i1 + src_dim;
        int j  = y*dst_dim;
        int x;

        /* Handle all unrolled vectors */
        for (x = 0; x < full; x++) {
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            if (RC_UNROLL(rc_reduce_2x2_u8) >= 2) {
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            }
            if (RC_UNROLL(rc_reduce_2x2_u8) == 4) {
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
                RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
            }
        }

        /* Handle remaining 2x2 vector block */
        for (x = 0; x < twin; x++) {
            RC_REDUCE_2X2_ITER(dst, src, i1, i2, j);
        }

        /* Handle any single remaining vector */
        if (tail) {
            RC_REDUCE_2X2_TAIL(dst, src, i1, i2, j);
        }
    }

    RC_VEC_CLEANUP();
}
#endif
#endif
