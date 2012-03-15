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
 *  @file   rc_vec_mmx.h
 *  @brief  RAPP Compute layer vector operations
 *          using 64-bit MMX instructions.
 */

#ifndef RC_VEC_MMX_H
#define RC_VEC_MMX_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <mmintrin.h>   /* MMX intrinsics */

/* See the porting documentation for generic comments. */

#define RC_VEC_HINT_AVGT

typedef __m64 rc_vec_t;

#define RC_VEC_SIZE 8

/**
 *  No global declarations is needed for MMX registers.
 */
#define RC_VEC_DECLARE()

/**
 *  Clean up MMX registers.
 */
#define RC_VEC_CLEANUP() \
    _mm_empty()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = *(const rc_vec_t*)(ptr))

#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))

#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                          \
do {                                                                        \
    const uint8_t *sptr__, *uptr__;                                         \
    unsigned       off__;                                                   \
    sptr__ = (ptr);                              /* Cache source pointer */ \
    uptr__ = (uint8_t*)((uintptr_t)sptr__ & ~7); /* Align down           */ \
    off__  = (uintptr_t)sptr__ & 7;              /* Alignment offset     */ \
    (vec1) = _mm_cvtsi32_si64(8*off__);          /* Alignment bit shift  */ \
    (vec2) = _mm_cvtsi32_si64(64 - 8*off__);     /* Complementary shift  */ \
    RC_VEC_LOAD(vec3, uptr__);                   /* Load first vector    */ \
    (uptr) = uptr__ + RC_VEC_SIZE;               /* Advance data pointer */ \
} while (0)

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)   \
do {                                                 \
    rc_vec_t vec__;                                  \
    RC_VEC_LOAD(vec__, uptr);                        \
    (dstv) = _mm_or_si64(_mm_srl_si64(vec3,  vec1),  \
                         _mm_sll_si64(vec__, vec2)); \
    (vec3) = vec__;                                  \
} while (0)

#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = _mm_cvtsi32_si64(8*(bytes)))

#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = _mm_srl_si64(srcv, shv))

#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = _mm_sll_si64(srcv, shv))

#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = _mm_srli_si64(srcv, 8*(bytes)))

#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = _mm_slli_si64(srcv, 8*(bytes)))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)      \
do {                                                  \
    rc_vec_t sv1__, sv2__;                            \
    RC_VEC_SHLC(sv1__, srcv1, bytes);                 \
    RC_VEC_SHRC(sv2__, srcv2, RC_VEC_SIZE - (bytes)); \
    (dstv) = _mm_or_si64(sv1__, sv2__);               \
} while (0)

#define RC_VEC_ZERO(vec) \
    ((vec) = _mm_setzero_si64())

#define RC_VEC_NOT(dstv, srcv) \
   ((dstv) = _mm_andnot_si64(srcv, _mm_set1_pi8(0xff)))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = _mm_and_si64(srcv1, srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_or_si64(srcv1, srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = _mm_xor_si64(srcv1, srcv2))

#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = _mm_andnot_si64(srcv2, srcv1))

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
    ((vec) = _mm_set1_pi8(scal))

#define RC_VEC_ABS(dstv, srcv)                    \
do {                                              \
    rc_vec_t sv__   = (srcv);                     \
    rc_vec_t bias__ = _mm_set1_pi8(0x80);         \
    rc_vec_t pos__  = _mm_subs_pu8(sv__, bias__); \
    rc_vec_t neg__  = _mm_subs_pu8(bias__, sv__); \
    pos__  = _mm_or_si64(pos__, neg__);           \
    (dstv) = _mm_adds_pu8(pos__, pos__);          \
} while (0)

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = _mm_adds_pu8(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2)              \
do {                                                 \
    rc_vec_t sv1__ = (srcv1);                        \
    rc_vec_t sv2__ = (srcv2);                        \
    rc_vec_t xv__  = _mm_xor_si64(sv1__, sv2__);     \
    rc_vec_t av__  = _mm_and_si64(sv1__, sv2__);     \
    xv__   = _mm_srli_si64(xv__, 1);                 \
    xv__   = _mm_and_si64(xv__, _mm_set1_pi8(0x7f)); \
    (dstv) = _mm_add_pi8(xv__, av__);                \
} while (0)

#define RC_VEC_AVGR(dstv, srcv1, srcv2)                      \
do {                                                         \
    rc_vec_t sv1__ = (srcv1);                                \
    rc_vec_t sv2__ = (srcv2);                                \
    rc_vec_t xv__  = _mm_xor_si64(sv1__, sv2__);             \
    rc_vec_t av__  = _mm_and_si64(sv1__, sv2__);             \
    rc_vec_t rv__  = _mm_and_si64(xv__, _mm_set1_pi8(0x01)); \
    xv__   = _mm_srli_si64(xv__, 1);                         \
    xv__   = _mm_and_si64(xv__, _mm_set1_pi8(0x7f));         \
    xv__   = _mm_add_pi8(xv__, av__);                        \
    (dstv) = _mm_add_pi8(xv__, rv__);                        \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = _mm_subs_pu8(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2)               \
    ((dstv) = _mm_or_si64(_mm_subs_pu8(srcv1, srcv2), \
                          _mm_subs_pu8(srcv2, srcv1)))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, _mm_sub_pi8(_mm_set1_pi8(0xff), srcv2))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_VEC_AVGR(dstv, srcv1, _mm_sub_pi8(_mm_set1_pi8(0xff), srcv2))

#define RC_VEC_GETMASKV(maskv, vec)                                          \
do {                                                                         \
    rc_vec_t vec__ = (vec);                                                  \
    vec__   = _mm_srli_si64(vec__, 7);                      /* Shift MSBs */ \
    vec__   = _mm_and_si64(vec__, _mm_set1_pi8(0x01));      /* Mask MSBs  */ \
    vec__   = _mm_or_si64(vec__, _mm_srli_si64(vec__,  7)); /* 1:st fold  */ \
    vec__   = _mm_or_si64(vec__, _mm_srli_si64(vec__, 14)); /* 2:nd fold  */ \
    (maskv) = _mm_or_si64(vec__, _mm_srli_si64(vec__, 28)); /* 3:rd fols  */ \
} while (0)

#define RC_VEC_CNTN 1024 /* 8191 untestable */

#define RC_VEC_CNTV(accv, srcv)                                 \
do {                                                            \
    rc_vec_t c1__  = _mm_set1_pi8(0x55);                        \
    rc_vec_t c2__  = _mm_set1_pi8(0x33);                        \
    rc_vec_t c4__  = _mm_set1_pi8(0x0f);                        \
    rc_vec_t c8__  = _mm_set1_pi16(0x00ff);                     \
    rc_vec_t acc__ = (srcv);                                    \
    rc_vec_t tmp__;                                             \
    tmp__  = _mm_srli_pi32(acc__, 1);    /* tmp  = acc >> 1  */ \
    tmp__  = _mm_and_si64(tmp__, c1__);  /* tmp &= 0x55...   */ \
    acc__  = _mm_and_si64(acc__, c1__);  /* acc &= 0x55...   */ \
    acc__  = _mm_add_pi32(acc__, tmp__); /* acc += tmp       */ \
    tmp__  = _mm_srli_pi32(acc__, 2);    /* tmp  = acc >> 2  */ \
    tmp__  = _mm_and_si64(tmp__, c2__);  /* tmp &= 0x33...   */ \
    acc__  = _mm_and_si64(acc__, c2__);  /* acc &= 0x33...   */ \
    acc__  = _mm_add_pi32(acc__, tmp__); /* acc += tmp       */ \
    tmp__  = _mm_srli_pi32(acc__, 4);    /* tmp  = acc >> 4  */ \
    acc__  = _mm_add_pi32(acc__, tmp__); /* acc += tmp       */ \
    acc__  = _mm_and_si64(acc__, c4__);  /* acc &= 0x0f...   */ \
    tmp__  = _mm_srli_pi32(acc__, 8);    /* tmp  = acc >> 8  */ \
    acc__  = _mm_add_pi32(acc__, tmp__); /* acc += tmp       */ \
    acc__  = _mm_and_si64(acc__, c8__);  /* acc &= 0x00ff... */ \
    (accv) = _mm_add_pi32(accv, acc__);                         \
} while (0)

#define RC_VEC_CNTR(cnt, accv) \
    RC_VEC_SUMR(cnt, accv)

#define RC_VEC_SUMN 128

#define RC_VEC_SUMV(accv, srcv)                                \
do {                                                           \
    rc_vec_t sv__ = (srcv);                                    \
    rc_vec_t zv__ = _mm_setzero_si64();                        \
    (accv) = _mm_add_pi16(accv, _mm_unpacklo_pi8(sv__, zv__)); \
    (accv) = _mm_add_pi16(accv, _mm_unpackhi_pi8(sv__, zv__)); \
} while (0)

#define RC_VEC_SUMR(sum, accv)                            \
do {                                                      \
    rc_vec_t av__ = (accv);                               \
    rc_vec_t zv__ = _mm_setzero_si64();                   \
    rc_vec_t rv__;                                        \
    rv__  = _mm_add_pi32(_mm_unpacklo_pi16(av__, zv__),   \
                         _mm_unpackhi_pi16(av__, zv__));  \
    rv__  = _mm_add_pi32(rv__, _mm_srli_si64(rv__, 32));  \
    (sum) = _mm_cvtsi64_si32(rv__);                       \
} while (0)

#endif /* RC_VEC_MMX_H */
