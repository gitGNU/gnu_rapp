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
 *  @file   rc_vec_swar.h
 *  @brief  RAPP Compute layer vector operations using SWAR
 *          (SIMD Within A Register) techniques.
 */

#ifndef RC_VEC_SWAR_H
#define RC_VEC_SWAR_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <stdint.h>   /* UINT*_C()       */
#include "rc_word.h"  /* Word operations */

/* See the porting documentation for generic comments. */

#define RC_VEC_HINT_CMPGT

#define RC_VEC_HINT_AVGT

typedef rc_word_t rc_vec_t;

#define RC_VEC_SIZE RC_WORD_SIZE

/**
 *  No global declarations is needed for SWAR registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for SWAR registers.
 */
#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = RC_WORD_LOAD(ptr))

#define RC_VEC_STORE(ptr, vec) \
    RC_WORD_STORE(ptr, vec)

#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                           \
do {                                                                         \
    const uint8_t *sptr__, *uptr__;                                          \
    unsigned       off__;                                                    \
    sptr__ = (ptr);                           /* Cache source pointer     */ \
    uptr__ = (uint8_t*)((uintptr_t)sptr__ &                                  \
                        ~(RC_VEC_SIZE - 1));  /* Align down               */ \
    off__  = (uintptr_t)sptr__ % RC_VEC_SIZE; /* Compute alignment offset */ \
    (vec1) = 8*off__;                         /* Alignment bit shift      */ \
    (vec2) = 8*(RC_VEC_SIZE - off__);         /* Complementary shift      */ \
    (vec3) = RC_WORD_LOAD(uptr__);            /* Load first vector        */ \
    (uptr) = uptr__ + RC_VEC_SIZE;            /* Advance data pointer     */ \
} while (0)

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
do {                                               \
    rc_vec_t vec__ = RC_WORD_LOAD(uptr);           \
    (dstv) = RC_WORD_SHL(vec3,  vec1) |            \
             RC_WORD_SHR(vec__, vec2);             \
    (vec3) = vec__;                                \
} while (0)

#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = (bytes))

#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = RC_WORD_SHL(srcv, 8*(shv)))

#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = RC_WORD_SHR(srcv, 8*(shv)))

#define RC_VEC_SHLC(dstv, srcv, bytes) \
    RC_VEC_SHL(dstv, srcv, bytes)

#define RC_VEC_SHRC(dstv, srcv, bytes) \
    RC_VEC_SHR(dstv, srcv, bytes)

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = RC_WORD_ALIGN(srcv1, srcv2, 8*(bytes)))

#if   RC_VEC_SIZE == 2
#define RC_VEC_PACK(dstv, srcv1, srcv2)        \
do {                                           \
    rc_vec_t sv1__ = (srcv1) & RC_VEC_EVEN8__; \
    rc_vec_t sv2__;                            \
    RC_VEC_SHRC(sv2__, srcv2, 1);              \
    (dstv) = sv1__ | sv2__;                    \
} while (0)

#elif RC_VEC_SIZE == 4
#define RC_VEC_PACK(dstv, srcv1, srcv2)         \
do {                                            \
    rc_vec_t sv1__ = (srcv1) & RC_VEC_EVEN8__;  \
    rc_vec_t sv2__ = (srcv2) & RC_VEC_EVEN8__;  \
    rc_vec_t tmp__;                             \
    RC_VEC_SHLC(tmp__, sv1__, 1);               \
    sv1__  = (sv1__ | tmp__) & RC_VEC_EVEN16__; \
    RC_VEC_SHLC(tmp__, sv2__, 1);               \
    sv2__ |= tmp__;                             \
    RC_VEC_SHRC(sv2__, sv2__, 2);               \
    (dstv) = sv1__ | sv2__;                     \
} while (0)

#elif RC_VEC_SIZE == 8
#define RC_VEC_PACK(dstv, srcv1, srcv2)         \
do {                                            \
    rc_vec_t sv1__ = (srcv1) & RC_VEC_EVEN8__;  \
    rc_vec_t sv2__ = (srcv2) & RC_VEC_EVEN8__;  \
    rc_vec_t tmp__;                             \
    RC_VEC_SHLC(tmp__, sv1__, 1);               \
    sv1__  = (sv1__ | tmp__) & RC_VEC_EVEN16__; \
    RC_VEC_SHLC(tmp__, sv2__, 1);               \
    sv2__  = (sv2__ | tmp__) & RC_VEC_EVEN16__; \
    RC_VEC_SHLC(tmp__, sv1__, 2);               \
    sv1__  = (sv1__ | tmp__) & RC_VEC_EVEN32__; \
    RC_VEC_SHLC(tmp__, sv2__, 2);               \
    sv2__ |= tmp__;                             \
    RC_VEC_SHRC(sv2__, sv2__, 4);               \
    (dstv) = sv1__ | sv2__;                     \
} while (0)
#endif

#define RC_VEC_ZERO(vec) \
    ((vec) = 0)

#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = ~(srcv))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) & (srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) | (srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) ^ (srcv2))

#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) & ~(srcv2))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) | ~(srcv2))

#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) ^ ~(srcv2))

#define RC_VEC_SPLAT(vec, scal) \
do {                            \
    (vec)  = (scal) & 0xff;     \
    RC_VEC_LFOLD__(vec,  8);    \
    RC_VEC_LFOLD__(vec, 16);    \
    RC_VEC_LFOLD__(vec, 32);    \
} while (0)

#define RC_VEC_ADDS(dstv, srcv1, srcv2)                               \
do {                                                                  \
    rc_vec_t sv1__  = (srcv1);                                        \
    rc_vec_t sv2__  = (srcv2);                                        \
    rc_vec_t sum__  = sv1__ + sv2__;   /* Unpartitioned sum        */ \
    rc_vec_t mask__;                                                  \
    RC_VEC_AVGT(mask__, sv1__, sv2__); /* Truncated average        */ \
    mask__ &= RC_WORD_C8(80);          /* Overflow bits            */ \
    sum__  -= mask__ << 1;             /* Revert inter-field carry */ \
    mask__  = RC_VEC_MASK__(mask__);   /* Overflow mask            */ \
    (dstv)  = sum__ | mask__;          /* Saturate                 */ \
} while (0)

#define RC_VEC_AVGT(dstv, srcv1, srcv2)                               \
do {                                                                  \
    rc_vec_t v1__ = (srcv1);                                          \
    rc_vec_t v2__ = (srcv2);                                          \
    (dstv) = (v1__ & v2__) + (((v1__ ^ v2__) >> 1) & RC_WORD_C8(7f)); \
} while (0)

#define RC_VEC_AVGR(dstv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_vec_t sv1__ = (srcv1);                                           \
    rc_vec_t sv2__ = (srcv2);                                           \
    rc_vec_t xv__  = sv1__ ^ sv2__;                                     \
    (dstv) = (sv1__ & sv2__) +                                          \
             ((xv__ >> 1) & RC_WORD_C8(7f)) + /* Truncated average   */ \
             ( xv__       & RC_WORD_C8(01));  /* Rounding correction */ \
} while (0)

#define RC_VEC_AVGZ(dstv, srcv1, srcv2)                                   \
do {                                                                      \
    rc_vec_t sv1__ = (srcv1);                                             \
    rc_vec_t sv2__ = (srcv2);                                             \
    rc_vec_t xv__  = sv1__ ^ sv2__;                                       \
    rc_vec_t gt__;                                                        \
    RC_VEC_CMPGT(gt__, sv1__, sv2__);                                     \
    (dstv) = (sv1__ & sv2__) +                                            \
             ((xv__ >> 1)         & RC_WORD_C8(7f)) + /* Truncated avg */ \
             ( xv__ & (gt__ >> 7) & RC_WORD_C8(01));  /* Rounding corr */ \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2)                                \
do {                                                                   \
    rc_vec_t sv1__  = (srcv1);                                         \
    rc_vec_t sv2__  = (srcv2);                                         \
    rc_vec_t diff__ = sv1__ - sv2__;    /* Unpartitioned difference */ \
    rc_vec_t mask__;                                                   \
    RC_VEC_CMPGT(mask__, sv2__, sv1__); /* Truncated half diff      */ \
    mask__ &= RC_WORD_C8(80);           /* Underflow bits           */ \
    diff__ += mask__ << 1;              /* Revert inter-field carry */ \
    mask__  = RC_VEC_MASK__(mask__);    /* Underflow mask           */ \
    (dstv)  = diff__ & ~mask__;         /* Saturate                 */ \
} while (0)

#define RC_VEC_SUBA(dstv, srcv1, srcv2)                                    \
do {                                                                       \
    rc_vec_t sv1__ = (srcv1);                                              \
    rc_vec_t sv2__ = (srcv2);                                              \
    rc_vec_t gt__, lt__;                                                   \
    RC_VEC_CMPGT(gt__, sv1__, sv2__);            /* Compare             */ \
    gt__  &= RC_WORD_C8(80);                     /* Comparison MSBs     */ \
    gt__   = RC_VEC_MASK__(gt__);                /* Comparison mask     */ \
    lt__   = ~gt__;                              /* Complement          */ \
    (dstv) = ((sv1__ & gt__) - (sv2__ & gt__)) | /* Subtract and select */ \
             ((sv2__ & lt__) - (sv1__ & lt__));                            \
} while (0)

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, ~(srcv2))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_VEC_AVGR(dstv, srcv1, ~(srcv2))

#define RC_VEC_ADD16(dstv, srcv1, srcv2)                        \
do {                                                            \
    rc_vec_t sv1_  = (srcv1);                                   \
    rc_vec_t sv2_  = (srcv2);                                   \
    rc_vec_t masklo_ = RC_WORD_C16(7fff);                       \
    rc_vec_t maskhi_ = RC_WORD_C16(8000);                       \
    rc_vec_t sumlo_ = (sv1_ & masklo_) + (sv2_ & masklo_);      \
    rc_vec_t sumhi_ = (sv1_ ^ sv2_) & maskhi_;                  \
    (dstv)  = sumlo_ ^ sumhi_;                                  \
} while (0)

#define RC_VEC_SUB16(dstv, srcv1, srcv2)                \
do {                                                    \
    rc_vec_t not2_ = ~(srcv2);                          \
    rc_vec_t neg2_;                                     \
    RC_VEC_ADD16(neg2_, not2_, RC_WORD_C16(0001));      \
    RC_VEC_ADD16(dstv, srcv1, neg2_);                   \
} while (0)

#if RC_VEC_SIZE >= 4
#define RC_VEC_ADD32(dstv, srcv1, srcv2)                        \
do {                                                            \
    rc_vec_t sv1_  = (srcv1);                                   \
    rc_vec_t sv2_  = (srcv2);                                   \
    rc_vec_t masklo_ = RC_WORD_C32(7fffffff);                   \
    rc_vec_t maskhi_ = RC_WORD_C32(80000000);                   \
    rc_vec_t sumlo_ = (sv1_ & masklo_) + (sv2_ & masklo_);      \
    rc_vec_t sumhi_ = (sv1_ ^ sv2_) & maskhi_;                  \
    (dstv)  = sumlo_ ^ sumhi_;                                  \
} while (0)

#define RC_VEC_SUB32(dstv, srcv1, srcv2)                \
do {                                                    \
    rc_vec_t not2_ = ~(srcv2);                          \
    rc_vec_t neg2_;                                     \
    RC_VEC_ADD32(neg2_, not2_, RC_WORD_C32(00000001));  \
    RC_VEC_ADD32(dstv, srcv1, neg2_);                   \
} while (0)
#endif /* RC_VEC_SIZE >= 4 */

#define RC_VEC_ABS(dstv, srcv)                                          \
do {                                                                    \
    rc_vec_t sv__   = (srcv);                                           \
    rc_vec_t msb__  = sv__ & RC_WORD_C8(80);  /* Mask MSBs           */ \
    rc_vec_t mask__ = RC_VEC_MASK__(msb__);   /* Create field mask   */ \
    sv__   = ((sv__  &  mask__) - msb__) |    /* Select              */ \
              ((msb__ ^ RC_WORD_C8(80)) - (sv__ & ~mask__));            \
    mask__ = (sv__ & RC_WORD_C8(80)) >> 7;    /* Mask MSBs           */ \
    (dstv) = ((sv__ - mask__) << 1) | mask__; /* Double and saturate */ \
} while (0)

#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, ~(srcv2))

#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
do {                                     \
    rc_vec_t lt__;                       \
    RC_VEC_CMPGT(lt__, srcv2, srcv1);    \
    (dstv) = ~lt__;                      \
} while (0)

#define RC_VEC_MIN(dstv, srcv1, srcv2)                  \
do {                                                    \
    rc_vec_t sv1__  = (srcv1);                          \
    rc_vec_t sv2__  = (srcv2);                          \
    rc_vec_t mask__;                                    \
    RC_VEC_CMPGT(mask__, sv1__, sv2__); /* Compare   */ \
    mask__ &= RC_WORD_C8(80);           /* Mask MSBs */ \
    mask__ = RC_VEC_MASK__(mask__);     /* Widen     */ \
    (dstv) = (sv1__ & ~mask__) |        /* Select    */ \
             (sv2__ &  mask__);                         \
} while (0)

#define RC_VEC_MAX(dstv, srcv1, srcv2)                  \
do {                                                    \
    rc_vec_t sv1__  = (srcv1);                          \
    rc_vec_t sv2__  = (srcv2);                          \
    rc_vec_t mask__;                                    \
    RC_VEC_CMPGT(mask__, sv1__, sv2__); /* Compare   */ \
    mask__ &= RC_WORD_C8(80);           /* Mask MSBs */ \
    mask__  = RC_VEC_MASK__(mask__);    /* Widen     */ \
    (dstv)  = (sv1__ &  mask__) |       /* Select    */ \
              (sv2__ & ~mask__);                        \
} while (0)

#define RC_VEC_BLEND(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)                      \
do {                                                                         \
    rc_vec_t sv1__ = (srcv1);                                                \
    rc_vec_t sv2__ = (srcv2);                                                \
    rc_vec_t u1__, u2__;                                                     \
    u1__ = RC_VEC_SUB_EVEN__(sv2__, sv1__);             /* Sub even fields */\
    u1__ = (((blend8)*u1__ + RC_WORD_C16(0080)) >> 8) & /* Mult w/ blend8  */\
           RC_WORD_C16(00ff);                                                \
    u2__ = RC_VEC_SUB_EVEN__(sv2__ >> 8, sv1__ >> 8);   /* Sub odd fields  */\
    u2__ = ( (blend8)*u2__ + RC_WORD_C16(0080)) &       /* Mult w/ blend8  */\
           RC_WORD_C16(ff00);                                                \
    (dstv) = sv1__ + (u1__ | u2__) - (blendv);          /* Merge & unbias  */\
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)                     \
do {                                                                         \
    rc_vec_t sv1__ = (srcv1);                                                \
    rc_vec_t sv2__ = (srcv2);                                                \
    rc_vec_t u1__, u2__, rb__;                                               \
    u1__  = RC_VEC_SUB_EVEN__(sv2__, sv1__);           /* Sub w/ bias     */ \
    rb__  = ~u1__ & RC_WORD_C16(0100);                 /* Mask MSBs       */ \
    rb__ -= rb__ >> 8;                                 /* Rounding bias   */ \
    u1__  = (((blend8)*u1__ + rb__) >> 8) & RC_WORD_C16(00ff); /* Mult    */ \
    u2__  = RC_VEC_SUB_EVEN__(sv2__ >> 8, sv1__ >> 8); /* Sub with bias   */ \
    rb__  = ~u2__ & RC_WORD_C16(0100);                 /* Mask MSBs       */ \
    rb__ -= rb__ >> 8;                                 /* Rounding bias   */ \
    u2__  =  ((blend8)*u2__ + rb__)       & RC_WORD_C16(ff00); /* Mult    */ \
    (dstv) = sv1__ + (u1__ | u2__) - (blendv);         /* Merge & unbias  */ \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)                     \
do {                                                                         \
    rc_vec_t sv1__ = (srcv1);                                                \
    rc_vec_t sv2__ = (srcv2);                                                \
    rc_vec_t u1__, u2__, rb__;                                               \
    u1__  = RC_VEC_SUB_EVEN__(sv2__, sv1__);            /* Sub w/ bias    */ \
    rb__  = u1__ & RC_WORD_C16(0100);                   /* Mask MSBs      */ \
    rb__ -= rb__ >> 8;                                  /* Rounding bias  */ \
    u1__  = (((blend8)*u1__ + rb__) >> 8) & RC_WORD_C16(00ff); /* Mult    */ \
    u2__  = RC_VEC_SUB_EVEN__(sv2__ >> 8, sv1__ >> 8);  /* Sub w/ bias    */ \
    rb__  = u2__ & RC_WORD_C16(0100);                   /* Mask MSBs      */ \
    rb__ -= rb__ >> 8;                                  /* Rounding bias  */ \
    u2__  =  ((blend8)*u2__ + rb__)       & RC_WORD_C16(ff00); /* Mult    */ \
    (dstv) = sv1__ + (u1__ | u2__) - (blendv);          /* Merge & unbias */ \
} while (0)

#define RC_VEC_GETMASKW(maskw, vec)                                        \
do {                                                                       \
    rc_vec_t w__ = ((vec) >> 7) & RC_WORD_C8(01); /* Extract MSBs       */ \
    RC_VEC_RFOLD__(w__,  7);                      /* 1:st fold - 2 bits */ \
    RC_VEC_RFOLD__(w__, 14);                      /* 2:nd fold - 4 bits */ \
    RC_VEC_RFOLD__(w__, 28);                      /* 3:rd fold - 8 bits */ \
    (maskw) = w__ & ((1 << RC_VEC_SIZE) - 1);     /* Mask and store     */ \
} while (0)

/* A definition is useful only when RC_VEC_SIZE is divisible by 8. */
#if RC_VEC_SIZE == 8
#define RC_VEC_SETMASKV(vec, maskv)                     \
do {                                                    \
    rc_vec_t m_, v_, av_, nz_, b_;                      \
    m_ = RC_WORD_EXTRACT(maskv, 0, 8);                  \
    RC_VEC_SPLAT(v_, m_);                               \
    RC_VEC_AND(av_, v_, RC_WORD_C64(8040201008040201)); \
    RC_VEC_CMPGT(nz_, av_, 0);                          \
    nz_ &= RC_WORD_C8(80);                              \
    b_ = RC_VEC_MASK__(nz_);                            \
    (vec) = b_;                                         \
} while (0)
#endif /* RC_VEC_SIZE == 8 */

#define RC_VEC_CNTN 28  /* 31 not divisible by 4 */

#define RC_VEC_CNTV(accv, srcv)                        \
do {                                                   \
    rc_vec_t acc__ = (srcv);                           \
    acc__   = ( acc__       & RC_WORD_C8(55)) +        \
              ((acc__ >> 1) & RC_WORD_C8(55));         \
    acc__   = ( acc__       & RC_WORD_C8(33)) +        \
              ((acc__ >> 2) & RC_WORD_C8(33));         \
    (accv) += (acc__ + (acc__ >> 4)) & RC_WORD_C8(0f); \
} while (0)

#define RC_VEC_CNTR(cnt, accv) \
do {                           \
    rc_vec_t sum__ = 0;        \
    RC_VEC_SUMV(sum__, accv);  \
    RC_VEC_SUMR(cnt, sum__);   \
} while (0)

#define RC_VEC_SUMN 128

#define RC_VEC_SUMV(accv, srcv)                   \
do {                                              \
    rc_vec_t sv__ = (srcv);                       \
    (accv) += ( sv__       & RC_WORD_C16(00ff)) + \
              ((sv__ >> 8) & RC_WORD_C16(00ff));  \
} while (0)

#if   RC_VEC_SIZE == 2
#define RC_VEC_SUMR(sum, accv) \
    ((sum) = (accv))

#elif RC_VEC_SIZE == 4
#define RC_VEC_SUMR(sum, accv) \
    ((sum) = ((accv) >> 16) +  \
             ((accv) & RC_WORD_C32(0000ffff)))

#elif RC_VEC_SIZE == 8
#define RC_VEC_SUMR(sum, accv)                          \
do {                                                    \
    rc_vec_t acc__;                                     \
    acc__  = ( (accv)        & RC_WORD_C32(0000ffff)) + \
             (((accv) >> 16) & RC_WORD_C32(0000ffff));  \
    acc__ += acc__ >> 32;                               \
    (sum)  = acc__ & RC_WORD_C64(00000000ffffffff);     \
} while (0)
#endif

/*
 * -------------------------------------------------------------
 *  Internal support macros
 * -------------------------------------------------------------
 */

/*
 *  Compute an 8-bit mask given an MSB word.
 */
#define RC_VEC_MASK__(msbv) \
    (((msbv) - ((msbv) >> 7)) | (msbv))

/*
 *  Mask out and subtract even fields and add bias.
 *  The result will leak to odd fields.
 */
#define RC_VEC_SUB_EVEN__(op1, op2) \
    (((op1) & RC_WORD_C16(00ff)) -  \
     ((op2) & RC_WORD_C16(00ff)) + RC_WORD_C16(0100))

/**
 *  Fold a vector to the left.
 *  Computes vec |= vec << bits. The shift amount should be a constant.
 */
#define RC_VEC_LFOLD__(vec, bits) \
    ((vec) |= ((bits) < 8*RC_VEC_SIZE) ? (vec) << (bits) : 0)

/**
 *  Fold a vector to the right.
 *  Computes vec |= vec >> bits. The shift amount should be a constant.
 */
#define RC_VEC_RFOLD__(vec, bits) \
    ((vec) |= ((bits) < 8*RC_VEC_SIZE) ? (vec) >> (bits) : 0)

/**
 *  Even field selection masks.
 */
#if RC_BIG_ENDIAN
#define RC_VEC_EVEN8__  RC_WORD_C16(ff00)
#define RC_VEC_EVEN16__ RC_WORD_C32(ffff0000)
#define RC_VEC_EVEN32__ RC_WORD_C64(ffffffff00000000)
#else
#define RC_VEC_EVEN8__  RC_WORD_C16(00ff)
#define RC_VEC_EVEN16__ RC_WORD_C32(0000ffff)
#define RC_VEC_EVEN32__ RC_WORD_C64(00000000ffffffff)
#endif

#endif /* RC_VEC_SWAR_H */
