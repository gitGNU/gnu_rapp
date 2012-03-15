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
 *  @file   rc_vec_ssse3.h
 *  @brief  RAPP Compute layer vector operations using 128-bit SSSE3
 *          instructions. Augments the 128-bit SSE2 implementation.
 */

#ifndef RC_VEC_SSSE3_H
#define RC_VEC_SSSE3_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <pmmintrin.h>   /* SSE3  intrinsics       */
#include <tmmintrin.h>   /* SSSE3 intrinsics       */
#include "rc_vec_sse2.h" /* SSE2 vector operations */

/* See the porting documentation for generic comments. */

/*
 * -------------------------------------------------------------
 *  Extensions/overrides
 * -------------------------------------------------------------
 */

#undef  RC_VEC_LOADU
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
    ((dstv) = _mm_lddqu_si128((const rc_vec_t*)(uptr)))

#undef  RC_VEC_ALIGNC
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = _mm_alignr_epi8(srcv2, srcv1, bytes))

#undef  RC_VEC_ABS
#define RC_VEC_ABS(dstv, srcv)                   \
do {                                             \
    rc_vec_t msb__ = _mm_set1_epi8(0x80);        \
    rc_vec_t sv__  = _mm_xor_si128(srcv, msb__); \
    sv__   = _mm_abs_epi8(sv__);                 \
    (dstv) = _mm_adds_epu8(sv__, sv__);          \
} while (0)

#undef  RC_VEC_BLEND
#define RC_VEC_BLEND(blendv, blend8) \
     ((blendv) = _mm_set1_epi16((blend8) << 7))

#undef  RC_VEC_LERP
#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                    \
    rc_vec_t sv1__ = (srcv1);                           \
    rc_vec_t sv2__ = (srcv2);                           \
    rc_vec_t bv__  = (blendv);                          \
    rc_vec_t zv__  = _mm_setzero_si128();               \
    rc_vec_t lo1__ = _mm_unpacklo_epi8(sv1__, zv__);    \
    rc_vec_t hi1__ = _mm_unpackhi_epi8(sv1__, zv__);    \
    rc_vec_t lo2__ = _mm_unpacklo_epi8(sv2__, zv__);    \
    rc_vec_t hi2__ = _mm_unpackhi_epi8(sv2__, zv__);    \
    lo2__  = _mm_sub_epi16(lo2__, lo1__);               \
    hi2__  = _mm_sub_epi16(hi2__, hi1__);               \
    lo2__  = _mm_mulhrs_epi16(lo2__, bv__);             \
    hi2__  = _mm_mulhrs_epi16(hi2__, bv__);             \
    lo1__  = _mm_add_epi16(lo1__, lo2__);               \
    hi1__  = _mm_add_epi16(hi1__, hi2__);               \
    (dstv) = _mm_packus_epi16(lo1__, hi1__);            \
} while (0)

#endif /* RC_VEC_SSSE3_H */
