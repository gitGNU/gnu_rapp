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
 *  @file   rc_vec_sse2.h
 *  @brief  RAPP Compute layer vector operations
 *          using 128-bit SSE2 instructions.
 */

#ifndef RC_VEC_SSE2_H
#define RC_VEC_SSE2_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <emmintrin.h>  /* SSE2 intrinsics */

/* See the porting documentation for generic comments. */

#define RC_VEC_HINT_CMPGE

#define RC_VEC_HINT_AVGR

typedef __m128i rc_vec_t;

#define RC_VEC_SIZE 16

/**
 *  No global declarations is needed for SSE2 registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for SSE2 registers.
 */
#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = _mm_load_si128((const rc_vec_t*)(ptr)))

#define RC_VEC_STORE(ptr, vec) \
    _mm_store_si128((rc_vec_t*)(ptr), vec)

#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr) \
do {                                               \
    (void)(vec1);                                  \
    (void)(vec2);                                  \
    (void)(vec3);                                  \
    (uptr) = (ptr);                                \
} while (0)

/* We only need to use the adjusted data pointer uptr. */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
    ((dstv) = _mm_loadu_si128((const rc_vec_t*)(uptr)))

#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = _mm_srli_si128(srcv, bytes))

#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = _mm_slli_si128(srcv, bytes))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)                   \
do {                                                               \
    rc_vec_t sv1__ = _mm_srli_si128(srcv1, bytes);                 \
    rc_vec_t sv2__ = _mm_slli_si128(srcv2, RC_VEC_SIZE - (bytes)); \
    (dstv) = _mm_or_si128(sv1__, sv2__);                           \
} while (0)

#define RC_VEC_PACK(dstv, srcv1, srcv2)           \
do {                                              \
    rc_vec_t sel__ = _mm_set1_epi16(0x00ff);      \
    rc_vec_t sv1__ = _mm_and_si128(srcv1, sel__); \
    rc_vec_t sv2__ = _mm_and_si128(srcv2, sel__); \
    (dstv) = _mm_packus_epi16(sv1__, sv2__);      \
} while (0)

#define RC_VEC_ZERO(vec) \
    ((vec) = _mm_setzero_si128())

#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = _mm_andnot_si128(srcv, _mm_set1_epi8(0xff)))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = _mm_and_si128(srcv1, srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_or_si128(srcv1, srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_xor_si128(srcv1, srcv2))

#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = _mm_andnot_si128(srcv2, srcv1))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
do {                                     \
    rc_vec_t inv__;                      \
    RC_VEC_NOT(inv__, srcv2);            \
    RC_VEC_OR(dstv, srcv1, inv__);       \
} while (0)

#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
do {                                      \
    rc_vec_t inv__;                       \
    RC_VEC_NOT(inv__, srcv2);             \
    RC_VEC_XOR(dstv, srcv1, inv__);       \
} while (0)

#define RC_VEC_SPLAT(vec, scal) \
    ((vec) = _mm_set1_epi8(scal))

#define RC_VEC_ABS(dstv, srcv)                     \
do {                                               \
    rc_vec_t sv__   = (srcv);                      \
    rc_vec_t bias__ = _mm_set1_epi8(0x80);         \
    rc_vec_t pos__  = _mm_subs_epu8(sv__, bias__); \
    rc_vec_t neg__  = _mm_subs_epu8(bias__, sv__); \
    pos__  = _mm_or_si128(pos__, neg__);           \
    (dstv) = _mm_adds_epu8(pos__, pos__);          \
} while (0)

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = _mm_adds_epu8(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2)                 \
do {                                                    \
    rc_vec_t sv1__ = (srcv1);                           \
    rc_vec_t sv2__ = (srcv2);                           \
    rc_vec_t adj__;                                     \
    adj__  = _mm_xor_si128(sv1__, sv2__);               \
    sv1__  = _mm_avg_epu8(sv1__, sv2__);                \
    adj__  = _mm_and_si128(adj__, _mm_set1_epi8(0x01)); \
    (dstv) = _mm_sub_epi8(sv1__, adj__);                \
} while (0)

#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_avg_epu8(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2) \
    RC_VEC_LERPZ(dstv, srcv1, srcv2, 0x80, _mm_set1_epi16(0x8000))

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = _mm_subs_epu8(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2)                 \
    ((dstv) = _mm_or_si128(_mm_subs_epu8(srcv1, srcv2), \
                           _mm_subs_epu8(srcv2, srcv1)))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, _mm_sub_epi8(_mm_set1_epi8(0xff), srcv2))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_VEC_AVGR(dstv, srcv1, _mm_sub_epi8(_mm_set1_epi8(0xff), srcv2))

#define RC_VEC_ADD16(dstv, srcv1, srcv2) \
    ((dstv) = (rc_vec_t)_mm_add_epi16(srcv1, srcv2))

#define RC_VEC_SUB16(dstv, srcv1, srcv2) \
    ((dstv) = (rc_vec_t)_mm_sub_epi16(srcv1, srcv2))

#define RC_VEC_ADD32(dstv, srcv1, srcv2) \
    ((dstv) = (rc_vec_t)_mm_add_epi32(srcv1, srcv2))

#define RC_VEC_SUB32(dstv, srcv1, srcv2) \
    ((dstv) = (rc_vec_t)_mm_sub_epi32(srcv1, srcv2))

#define RC_VEC_CMPGT(dstv, srcv1, srcv2)                                \
do {                                                                    \
    rc_vec_t sv1__ = (srcv1);                                           \
    rc_vec_t lte__ = _mm_cmpeq_epi8(sv1__, _mm_min_epu8(srcv1, srcv2)); \
    (dstv) = _mm_andnot_si128(lte__, _mm_set1_epi8(0xff));              \
} while (0)

#define RC_VEC_CMPGE(dstv, srcv1, srcv2)                      \
do {                                                          \
    rc_vec_t sv__ = (srcv1);                                  \
    (dstv) = _mm_cmpeq_epi8(sv__, _mm_max_epu8(sv__, srcv2)); \
} while (0)

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = _mm_min_epu8(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = _mm_max_epu8(srcv1, srcv2))

#define RC_VEC_BLEND(blendv, blend8) \
    ((blendv) = _mm_set1_epi16((blend8) << 8))

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)                     \
do {                                                                        \
    rc_vec_t srcv1__ = (srcv1);                                             \
    rc_vec_t srcv2__ = (srcv2);                                             \
    rc_vec_t blend__ = (blendv);                                            \
    rc_vec_t zero__  = _mm_setzero_si128();                                 \
    rc_vec_t bias__  = _mm_set1_epi16(0x80);                                \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, bias__, bias__, zero__); \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    ((blendv) = _mm_set1_epi16((blend8) << 8))

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)                  \
do {                                                                      \
    rc_vec_t srcv1__ = (srcv1);                                           \
    rc_vec_t srcv2__ = (srcv2);                                           \
    rc_vec_t blend__ = (blendv);                                          \
    rc_vec_t zero__  = _mm_setzero_si128();                               \
    rc_vec_t bias__  = _mm_cmpeq_epi8(srcv1__,                            \
                                      _mm_max_epu8(srcv1__, srcv2__));    \
    rc_vec_t blo__   = _mm_unpacklo_epi8(bias__, zero__);                 \
    rc_vec_t bhi__   = _mm_unpackhi_epi8(bias__, zero__);                 \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, blo__, bhi__, zero__); \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_BLENDZ(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)                  \
do {                                                                      \
    rc_vec_t srcv1__ = (srcv1);                                           \
    rc_vec_t srcv2__ = (srcv2);                                           \
    rc_vec_t blend__ = (blendv);                                          \
    rc_vec_t zero__  = _mm_setzero_si128();                               \
    rc_vec_t bias__  = _mm_cmpeq_epi8(srcv2__,                            \
                                      _mm_max_epu8(srcv1__, srcv2__));    \
    rc_vec_t blo__   = _mm_unpacklo_epi8(bias__, zero__);                 \
    rc_vec_t bhi__   = _mm_unpackhi_epi8(bias__, zero__);                 \
    RC_VEC_LERP__(dstv, srcv1__, srcv2__, blend__, blo__, bhi__, zero__); \
} while (0)

#define RC_VEC_GETMASKW(maskw, vec) \
    ((maskw) = _mm_movemask_epi8(vec))

#define RC_VEC_SETMASKV(vec, maskv)                                     \
do {                                                                    \
    rc_vec_t v_ = (maskv);                                              \
    /* From {a, b, x, ...}, make {a, a, ..., [7] = a, b, b, ..., b} */  \
    rc_vec_t vh16l16_ = _mm_unpacklo_epi8(v_, v_);                      \
    rc_vec_t vh32l32_ = _mm_unpacklo_epi8(vh16l16_, vh16l16_);          \
    rc_vec_t vh64l64_ = _mm_unpacklo_epi8(vh32l32_, vh32l32_);          \
    rc_vec_t mask_ = _mm_setr_epi8(1<<0, 1<<1, 1<<2, 1<<3,              \
                                   1<<4, 1<<5, 1<<6, 1<<7,              \
                                   1<<0, 1<<1, 1<<2, 1<<3,              \
                                   1<<4, 1<<5, 1<<6, 1<<7);             \
    rc_vec_t andv_ = _mm_and_si128(vh64l64_, mask_);                    \
    (vec) = _mm_cmpeq_epi8(andv_, mask_);                               \
} while (0)

#define RC_VEC_8S16(dvecl, dvecr, srcv)                 \
do {                                                    \
    rc_vec_t s_ = (srcv);                               \
    rc_vec_t zero_ = _mm_setzero_si128();               \
    rc_vec_t signs_ = _mm_cmpgt_epi8(zero_, s_);        \
    (dvecl) = (rc_vec_t)_mm_unpacklo_epi8(s_, signs_);  \
    (dvecr) = (rc_vec_t)_mm_unpackhi_epi8(s_, signs_);  \
} while (0)

#define RC_VEC_8U16(dvecl, dvecr, srcv)                 \
do {                                                    \
    rc_vec_t s_ = (srcv);                               \
    rc_vec_t zero_ = _mm_setzero_si128();               \
    (dvecl) = (rc_vec_t)_mm_unpacklo_epi8(s_, zero_);   \
    (dvecr) = (rc_vec_t)_mm_unpackhi_epi8(s_, zero_);   \
} while (0)

#define RC_VEC_16S32(dvecl, dvecr, srcv)                \
do {                                                    \
    rc_vec_t s_ = (srcv);                               \
    rc_vec_t zero_ = _mm_setzero_si128();               \
    rc_vec_t signs_ = _mm_cmpgt_epi16(zero_, s_);       \
    (dvecl) = (rc_vec_t)_mm_unpacklo_epi16(s_, signs_); \
    (dvecr) = (rc_vec_t)_mm_unpackhi_epi16(s_, signs_); \
} while (0)

#define RC_VEC_16U32(dvecl, dvecr, srcv)                \
do {                                                    \
    rc_vec_t s_ = (srcv);                               \
    rc_vec_t zero_ = _mm_setzero_si128();               \
    (dvecl) = (rc_vec_t)_mm_unpacklo_epi16(s_, zero_);  \
    (dvecr) = (rc_vec_t)_mm_unpackhi_epi16(s_, zero_);  \
} while (0)

#define RC_VEC_CNTN 1024 /* 8191 untestable */

#define RC_VEC_CNTV(accv, srcv)                                  \
do {                                                             \
    rc_vec_t c1__  = _mm_set1_epi8(0x55);                        \
    rc_vec_t c2__  = _mm_set1_epi8(0x33);                        \
    rc_vec_t c4__  = _mm_set1_epi8(0x0f);                        \
    rc_vec_t c8__  = _mm_set1_epi16(0x00ff);                     \
    rc_vec_t acc__ = (srcv);                                     \
    rc_vec_t tmp__;                                              \
    tmp__  = _mm_srli_epi32(acc__, 1);    /* tmp  = acc >> 1  */ \
    tmp__  = _mm_and_si128(tmp__, c1__);  /* tmp &= 0x55...   */ \
    acc__  = _mm_and_si128(acc__, c1__);  /* acc &= 0x55...   */ \
    acc__  = _mm_add_epi32(acc__, tmp__); /* acc += tmp       */ \
    tmp__  = _mm_srli_epi32(acc__, 2);    /* tmp  = acc >> 2  */ \
    tmp__  = _mm_and_si128(tmp__, c2__);  /* tmp &= 0x33...   */ \
    acc__  = _mm_and_si128(acc__, c2__);  /* acc &= 0x33...   */ \
    acc__  = _mm_add_epi32(acc__, tmp__); /* acc += tmp       */ \
    tmp__  = _mm_srli_epi32(acc__, 4);    /* tmp  = acc >> 4  */ \
    acc__  = _mm_add_epi32(acc__, tmp__); /* acc += tmp       */ \
    acc__  = _mm_and_si128(acc__, c4__);  /* acc &= 0x0f...   */ \
    tmp__  = _mm_srli_epi32(acc__, 8);    /* tmp  = acc >> 8  */ \
    acc__  = _mm_add_epi32(acc__, tmp__); /* acc += tmp       */ \
    acc__  = _mm_and_si128(acc__, c8__);  /* acc &= 0x00ff... */ \
    (accv) = _mm_add_epi32(accv, acc__);                         \
} while (0)

#define RC_VEC_CNTR(cnt, accv) \
    RC_VEC_SUMR(cnt, accv)

#define RC_VEC_SUMN 128

#define RC_VEC_SUMV(accv, srcv)                                  \
do {                                                             \
    rc_vec_t sv__ = (srcv);                                      \
    rc_vec_t zv__ = _mm_setzero_si128();                         \
    (accv) = _mm_add_epi16(accv, _mm_unpacklo_epi8(sv__, zv__)); \
    (accv) = _mm_add_epi16(accv, _mm_unpackhi_epi8(sv__, zv__)); \
} while (0)

#define RC_VEC_SUMR(sum, accv)                             \
do {                                                       \
    rc_vec_t av__ = (accv);                                \
    rc_vec_t zv__ = _mm_setzero_si128();                   \
    rc_vec_t rv__;                                         \
    rv__  = _mm_add_epi32(_mm_unpacklo_epi16(av__, zv__),  \
                          _mm_unpackhi_epi16(av__, zv__)); \
    rv__  = _mm_add_epi32(rv__, _mm_srli_si128(rv__, 4));  \
    rv__  = _mm_add_epi32(rv__, _mm_srli_si128(rv__, 8));  \
    (sum) = _mm_cvtsi128_si32(rv__);                       \
} while (0)

#define RC_VEC_MACN 1024 /* 16512 untestable */

#define RC_VEC_MACV(accv, srcv1, srcv2)              \
do {                                                 \
    rc_vec_t sv1__ = (srcv1);                        \
    rc_vec_t sv2__ = (srcv2);                        \
    rc_vec_t zv__  = _mm_setzero_si128();            \
    rc_vec_t lo1__ = _mm_unpacklo_epi8(sv1__, zv__); \
    rc_vec_t hi1__ = _mm_unpackhi_epi8(sv1__, zv__); \
    rc_vec_t lo2__ = _mm_unpacklo_epi8(sv2__, zv__); \
    rc_vec_t hi2__ = _mm_unpackhi_epi8(sv2__, zv__); \
    lo1__  = _mm_madd_epi16(lo1__, lo2__);           \
    hi1__  = _mm_madd_epi16(hi1__, hi2__);           \
    hi1__  = _mm_add_epi32(lo1__, hi1__);            \
    (accv) = _mm_add_epi32(accv, hi1__);             \
} while (0)

#define RC_VEC_MACR(mac, accv)                             \
do {                                                       \
    rc_vec_t av__ = (accv);                                \
    rc_vec_t zv__ = _mm_setzero_si128();                   \
    rc_vec_t rv__;                                         \
    rv__  = _mm_add_epi64(_mm_unpacklo_epi32(av__, zv__),  \
                          _mm_unpackhi_epi32(av__, zv__)); \
    rv__  = _mm_add_epi64(rv__, _mm_srli_si128(rv__, 8));  \
    (mac) = _mm_cvtsi128_si32(rv__);                       \
} while (0)

/*
 * -------------------------------------------------------------
 *  Internal support macros
 * -------------------------------------------------------------
 */

/**
 *  Linear interpolation, common functionality.
 */
#define RC_VEC_LERP__(dstv, srcv1, srcv2, blendv, bias1, bias2, zero)         \
do {                                                                          \
    rc_vec_t lo1__  = _mm_unpacklo_epi8(zero, srcv1); /* Expand to 16 bits */ \
    rc_vec_t hi1__  = _mm_unpackhi_epi8(zero, srcv1);                         \
    rc_vec_t lo2__  = _mm_unpacklo_epi8(zero, srcv2);                         \
    rc_vec_t hi2__  = _mm_unpackhi_epi8(zero, srcv2);                         \
    rc_vec_t ulo__, uhi__;                                                    \
    ulo__  = _mm_mulhi_epu16(lo1__, blendv);  /* Mult with blend factor    */ \
    uhi__  = _mm_mulhi_epu16(hi1__, blendv);                                  \
    lo2__  = _mm_mulhi_epu16(lo2__, blendv);                                  \
    hi2__  = _mm_mulhi_epu16(hi2__, blendv);                                  \
    lo1__  = _mm_sub_epi16(lo1__, ulo__);     /* Subtract srcv1*blend term */ \
    hi1__  = _mm_sub_epi16(hi1__, uhi__);                                     \
    lo1__  = _mm_add_epi16(lo1__, lo2__);     /* Add srcv2*blend term      */ \
    hi1__  = _mm_add_epi16(hi1__, hi2__);                                     \
    lo1__  = _mm_add_epi16(lo1__, bias1);     /* Add roundoff bias         */ \
    hi1__  = _mm_add_epi16(hi1__, bias2);                                     \
    lo1__  = _mm_srli_epi16(lo1__, 8);        /* Reduce to 8 bits          */ \
    hi1__  = _mm_srli_epi16(hi1__, 8);                                        \
    (dstv) = _mm_packus_epi16(lo1__, hi1__);  /* Pack into one vector      */ \
} while (0)

#endif /* RC_VEC_SSE2_H */
