/*  Copyright (C) 2005-2012, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_sse.h
 *  @brief  RAPP Compute layer vector operations using 64-bit SSE
 *          instructions. Augments the 64-bit MMX implementation.
 */

#ifndef RC_VEC_SSE_H
#define RC_VEC_SSE_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <xmmintrin.h>  /* SSE intrinsics        */
#include "rc_vec_mmx.h" /* MMX vector operations */

/* See the porting documentation for generic comments. */

#define RC_VEC_HINT_CMPGE

#undef  RC_VEC_HINT_AVGT
#define RC_VEC_HINT_AVGR

#define RC_VEC_HINT_GETMASKW

#undef  RC_VEC_AVGT
#define RC_VEC_AVGT(dstv, srcv1, srcv2)               \
do {                                                  \
    rc_vec_t sv1__ = (srcv1);                         \
    rc_vec_t sv2__ = (srcv2);                         \
    rc_vec_t adj__;                                   \
    adj__  = _mm_xor_si64(sv1__, sv2__);              \
    sv1__  = _mm_avg_pu8(sv1__, sv2__);               \
    adj__  = _mm_and_si64(adj__, _mm_set1_pi8(0x01)); \
    (dstv) = _mm_sub_pi8(sv1__, adj__);               \
} while (0)

#undef  RC_VEC_AVGR
#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_avg_pu8(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2) \
    RC_VEC_LERPZ(dstv, srcv1, srcv2, 0x80, _mm_set1_pi16(0x8000))

#define RC_VEC_CMPGT(dstv, srcv1, srcv2)                              \
do {                                                                  \
    rc_vec_t sv1__ = (srcv1);                                         \
    rc_vec_t lte__ = _mm_cmpeq_pi8(sv1__, _mm_min_pu8(srcv1, srcv2)); \
    (dstv) = _mm_andnot_si64(lte__, _mm_set1_pi8(0xff));              \
} while (0)

#define RC_VEC_CMPGE(dstv, srcv1, srcv2)                    \
do {                                                        \
    rc_vec_t sv__ = (srcv1);                                \
    (dstv) = _mm_cmpeq_pi8(sv__, _mm_max_pu8(sv__, srcv2)); \
} while (0)

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = _mm_min_pu8(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = _mm_max_pu8(srcv1, srcv2))

#define RC_VEC_BLEND(blendv, blend8) \
    ((blendv) = _mm_set1_pi16((blend8) << 8))

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)                     \
do {                                                                        \
    rc_vec_t srcv1__ = (srcv1);                                             \
    rc_vec_t srcv2__ = (srcv2);                                             \
    rc_vec_t blend__ = (blendv);                                            \
    rc_vec_t zero__  = _mm_setzero_si64();                                  \
    rc_vec_t bias__  = _mm_set1_pi16(0x80);                                 \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, bias__, bias__, zero__); \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    ((blendv) = _mm_set1_pi16((blend8) << 8))

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)                  \
do {                                                                      \
    rc_vec_t srcv1__ = (srcv1);                                           \
    rc_vec_t srcv2__ = (srcv2);                                           \
    rc_vec_t blend__ = (blendv);                                          \
    rc_vec_t zero__  = _mm_setzero_si64();                                \
    rc_vec_t bias__  = _mm_cmpeq_pi8(srcv1__,                             \
                                     _mm_max_pu8(srcv1__, srcv2__));      \
    rc_vec_t blo__   = _mm_unpacklo_pi8(bias__, zero__);                  \
    rc_vec_t bhi__   = _mm_unpackhi_pi8(bias__, zero__);                  \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, blo__, bhi__, zero__); \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_BLENDZ(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)                  \
do {                                                                      \
    rc_vec_t srcv1__ = (srcv1);                                           \
    rc_vec_t srcv2__ = (srcv2);                                           \
    rc_vec_t blend__ = (blendv);                                          \
    rc_vec_t zero__  = _mm_setzero_si64();                                \
    rc_vec_t bias__  = _mm_cmpeq_pi8(srcv2__,                             \
                                     _mm_max_pu8(srcv1__, srcv2__));      \
    rc_vec_t blo__   = _mm_unpacklo_pi8(bias__, zero__);                  \
    rc_vec_t bhi__   = _mm_unpackhi_pi8(bias__, zero__);                  \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, blo__, bhi__, zero__); \
} while (0)

#define RC_VEC_GETMASKW(maskw, vec) \
    ((maskw) = _mm_movemask_pi8(vec))

#define RC_VEC_MACN 1024 /* 16512 untestable */

#define RC_VEC_MACV(accv, srcv1, srcv2)             \
do {                                                \
    rc_vec_t sv1__ = (srcv1);                       \
    rc_vec_t sv2__ = (srcv2);                       \
    rc_vec_t zv__  = _mm_setzero_si64();            \
    rc_vec_t lo1__ = _mm_unpacklo_pi8(sv1__, zv__); \
    rc_vec_t hi1__ = _mm_unpackhi_pi8(sv1__, zv__); \
    rc_vec_t lo2__ = _mm_unpacklo_pi8(sv2__, zv__); \
    rc_vec_t hi2__ = _mm_unpackhi_pi8(sv2__, zv__); \
    lo1__  = _mm_madd_pi16(lo1__, lo2__);           \
    hi1__  = _mm_madd_pi16(hi1__, hi2__);           \
    hi1__  = _mm_add_pi32(lo1__, hi1__);            \
    (accv) = _mm_add_pi32(accv, hi1__);             \
} while (0)

#define RC_VEC_MACR(mac, accv)                           \
do {                                                     \
    rc_vec_t av__ = (accv);                              \
    rc_vec_t rv__;                                       \
    rv__  = _mm_add_pi32(av__, _mm_srli_si64(av__, 32)); \
    (mac) = _mm_cvtsi64_si32(rv__);                      \
} while (0)

/*
 * -------------------------------------------------------------
 *  Internal support macros
 * -------------------------------------------------------------
 */

/**
 *  Linear interpolation, common functionality.
 */
#define RC_VEC_LERP__(dstv, srcv1, srcv2, blendv, bias1, bias2, zero)        \
do {                                                                         \
    rc_vec_t lo1__  = _mm_unpacklo_pi8(zero, srcv1); /* Expand to 16 bits */ \
    rc_vec_t hi1__  = _mm_unpackhi_pi8(zero, srcv1);                         \
    rc_vec_t lo2__  = _mm_unpacklo_pi8(zero, srcv2);                         \
    rc_vec_t hi2__  = _mm_unpackhi_pi8(zero, srcv2);                         \
    rc_vec_t ulo__, uhi__;                                                   \
    ulo__  = _mm_mulhi_pu16(lo1__, blendv);  /* Mult with blend factor    */ \
    uhi__  = _mm_mulhi_pu16(hi1__, blendv);                                  \
    lo2__  = _mm_mulhi_pu16(lo2__, blendv);                                  \
    hi2__  = _mm_mulhi_pu16(hi2__, blendv);                                  \
    lo1__  = _mm_sub_pi16(lo1__, ulo__);     /* Subtract srcv1*blend term */ \
    hi1__  = _mm_sub_pi16(hi1__, uhi__);                                     \
    lo1__  = _mm_add_pi16(lo1__, lo2__);     /* Add srcv2*blend term      */ \
    hi1__  = _mm_add_pi16(hi1__, hi2__);                                     \
    lo1__  = _mm_add_pi16(lo1__, bias1);     /* Add roundoff bias         */ \
    hi1__  = _mm_add_pi16(hi1__, bias2);                                     \
    lo1__  = _mm_srli_pi16(lo1__, 8);        /* Reduce to 8 bits          */ \
    hi1__  = _mm_srli_pi16(hi1__, 8);                                        \
    (dstv) = _mm_packs_pu16(lo1__, hi1__);   /* Pack into one vector      */ \
} while (0)

#endif /* RC_VEC_SSE_H */
