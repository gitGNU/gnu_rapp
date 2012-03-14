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


/*
 * -------------------------------------------------------------
 *  Performance hints
 * -------------------------------------------------------------
 */

/**
 *  Use CMPGT instead of CMPGE when possible.
 */
#define RC_VEC_HINT_CMPGT

/**
 *  Use AVGT/SUBHT instead of AVGR/SUBHR when possible.
 */
#define RC_VEC_HINT_AVGT


/*
 * -------------------------------------------------------------
 *  The vector type
 * -------------------------------------------------------------
 */

/**
 *  The vector type definition.
 */
typedef rc_word_t rc_vec_t;

/**
 *  The number of bytes in a vector.
 */
#define RC_VEC_SIZE RC_WORD_SIZE


/*
 * -------------------------------------------------------------
 *  Vector state
 * -------------------------------------------------------------
 */

/**
 *  No global declarations is needed for SWAR registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for SWAR registers.
 */
#define RC_VEC_CLEANUP()


/*
 * -------------------------------------------------------------
 *  Memory access
 * -------------------------------------------------------------
 */

/**
 *  Load a vector from memory.
 */
#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = RC_WORD_LOAD(ptr))

/**
 *  Store a vector in memory.
 */
#define RC_VEC_STORE(ptr, vec) \
    RC_WORD_STORE(ptr, vec)


/*
 * -------------------------------------------------------------
 *  Misaligned memory access
 * -------------------------------------------------------------
 */

/**
 *  Initialize a sequence of misaligned loads.
 */
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

/**
 *  Misaligned vector load from memory.
 */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
do {                                               \
    rc_vec_t vec__ = RC_WORD_LOAD(uptr);           \
    (dstv) = RC_WORD_SHL(vec3,  vec1) |            \
             RC_WORD_SHR(vec__, vec2);             \
    (vec3) = vec__;                                \
} while (0)


/*
 * -------------------------------------------------------------
 *  Field relocation
 * -------------------------------------------------------------
 */

/**
 *  Initialize a field shift.
 */
#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = (bytes))

/**
 *  Shift all fields to the logical left.
 */
#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = RC_WORD_SHL(srcv, 8*(shv)))

/**
 *  Shift fields to the logical right.
 */
#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = RC_WORD_SHR(srcv, 8*(shv)))

/**
 *  Shift all fields to the logical left by a constant value.
 */
#define RC_VEC_SHLC(dstv, srcv, bytes) \
    RC_VEC_SHL(dstv, srcv, bytes)

/**
 *  Shift fields to the logical right by a constant value.
 */
#define RC_VEC_SHRC(dstv, srcv, bytes) \
    RC_VEC_SHR(dstv, srcv, bytes)

/**
 *  Align srcv1 and srcv2 to dstv, starting at field @e bytes
 *  into concatenation of srcv1 and srcv2. The alignment
 *  value @e bytes must be a constant.
 */
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = RC_WORD_ALIGN(srcv1, srcv2, 8*(bytes)))

/**
 *  Pack the even fields of srcv1 and srcv2 into one vector,
 *  with fields 0, ..., RC_VEC_SIZE/2 - 1 from srcv1 and
 *  fields RC_VEC_SIZE/2, ..., RC_VEC_SIZE - 1 from srcv2.
 */
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


/*
 * -------------------------------------------------------------
 *  Bitwise logical operations
 * -------------------------------------------------------------
 */

/**
 *  Set all bits to zero.
 */
#define RC_VEC_ZERO(vec) \
    ((vec) = 0)

/**
 *  Bitwise NOT.
 */
#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = ~(srcv))

/**
 *  Bitwise AND.
 */
#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) & (srcv2))

/**
 *  Bitwise OR.
 */
#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) | (srcv2))

/**
 *  Bitwise XOR.
 */
#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) ^ (srcv2))

/**
 *  Bitwise AND NOT.
 */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) & ~(srcv2))

/**
 *  Bitwise OR NOT.
 */
#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) | ~(srcv2))

/**
 *  Bitwise XOR NOT.
 */
#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) ^ ~(srcv2))


/*
 * -------------------------------------------------------------
 *  Arithmetic operations on 8-bit fields
 * -------------------------------------------------------------
 */

/**
 *  Set a value to all fields.
 *  Sets the scalar value 'scal' in each 8-bit field.
 */
#define RC_VEC_SPLAT(vec, scal) \
do {                            \
    (vec)  = (scal) & 0xff;     \
    RC_VEC_LFOLD__(vec,  8);    \
    RC_VEC_LFOLD__(vec, 16);    \
    RC_VEC_LFOLD__(vec, 32);    \
} while (0)

/**
 *  Saturating addition.
 *  Computes dstv = MIN(srcv1 + srcv2, 0xff) for each 8-bit field.
 */
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

/**
 *  Average value, truncated.
 *  Computes dstv = (srcv1 + srcv2) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGT(dstv, srcv1, srcv2)                               \
do {                                                                  \
    rc_vec_t v1__ = (srcv1);                                          \
    rc_vec_t v2__ = (srcv2);                                          \
    (dstv) = (v1__ & v2__) + (((v1__ ^ v2__) >> 1) & RC_WORD_C8(7f)); \
} while (0)

/**
 *  Average value, rounded.
 *  Computes dstv = (srcv1 + srcv2 + 1) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGR(dstv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_vec_t sv1__ = (srcv1);                                           \
    rc_vec_t sv2__ = (srcv2);                                           \
    rc_vec_t xv__  = sv1__ ^ sv2__;                                     \
    (dstv) = (sv1__ & sv2__) +                                          \
             ((xv__ >> 1) & RC_WORD_C8(7f)) + /* Truncated average   */ \
             ( xv__       & RC_WORD_C8(01));  /* Rounding correction */ \
} while (0)

/**
 *  Average value, rounded towards srcv1.
 *  Computes dstv = (srcv1 + srcv2 + (srcv1 > srcv2)) >> 1
 *  for each 8-bit field.
 */
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

/**
 *  Saturating subtraction.
 *  Computes dstv = MAX(srvc1 - srcv2, 0) for each 8-bit field.
 */
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

/**
 *  Absolute-value subtraction.
 *  Computes dstv = ABS(srcv1 - srcv2) for each 8-bit field.
 */
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

/**
 *  Half subtraction with bias, truncated.
 *  Computes dstv = (srcv1 - srcv2 + 0xff) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, ~(srcv2))

/**
 *  Half subtraction with bias, rounded.
 *  Computes dstv = (srcv1 - srcv2 + 0x100) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_VEC_AVGR(dstv, srcv1, ~(srcv2))

/**
 *  Absolute value.
 *  Computes dstv = 2*abs(srcv - 0x80) for each 8-bit field.
 *  The result is saturated to [0,0xff].
 */
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

/**
 *  Comparison.
 *  Computes dstv = srcv1 > srcv2 ? 0x80 : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, ~(srcv2))

/**
 *  Comparison.
 *  Computes dst = srcv1 >= srcv2 ? 0x80 : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
do {                                     \
    rc_vec_t lt__;                       \
    RC_VEC_CMPGT(lt__, srcv2, srcv1);    \
    (dstv) = ~lt__;                      \
} while (0)

/**
 *  Minimum value.
 *  Computes dstv = MIN(srcv1, srcv2) for each 8-bit field.
 */
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

/**
 *  Maximum value.
 *  Computes dstv = MAX(srcv1, srcv2) for each 8-bit field.
 */
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

/**
 *  Generate the blend vector needed by RC_VEC_LERP().
 */
#define RC_VEC_BLEND(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

/**
 *  Linear interpolation.
 *  Computes dstv = srcv1 + ((blend8*(srcv2 - srcv1) + 0x80) >> 8) for each
 *  8-bit field. The Q.8 blend factor @e blend8 must be in the range [0,0x7f].
 */
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

/**
 *  Generate the blend vector needed by RC_VEC_LERPZ().
 */
#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

/**
 *  Linear interpolation rounded towards srcv1.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded towards zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 */
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

/**
 *  Generate the blend vector needed by RC_VEC_LERPN().
 */
#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

/**
 *  Linear interpolation rounded towards srcv2.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded away from zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 */
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


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

/**
 *  Pack the most significant bits in each 8-bit field to the
 *  physically left-most bits in a binary mask word.
 *  The unused mask bits are set to zero.
 */
#define RC_VEC_GETMASKW(maskw, vec)                                        \
do {                                                                       \
    rc_vec_t w__ = ((vec) >> 7) & RC_WORD_C8(01); /* Extract MSBs       */ \
    RC_VEC_RFOLD__(w__,  7);                      /* 1:st fold - 2 bits */ \
    RC_VEC_RFOLD__(w__, 14);                      /* 2:nd fold - 4 bits */ \
    RC_VEC_RFOLD__(w__, 28);                      /* 3:rd fold - 8 bits */ \
    (maskw) = w__ & ((1 << RC_VEC_SIZE) - 1);     /* Mask and store     */ \
} while (0)


/*
 * -------------------------------------------------------------
 *  Reductions
 * -------------------------------------------------------------
 */

/**
 *  Count bits in all vector fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_CNTV() and RC_VEC_CNTR() together computes the bit count.
 *  The accumulation step can be iterated at most RC_VEC_CNTN times
 *  before the reduction step.
 */
#define RC_VEC_CNTN 28  /* 31 not divisible by 4 */

/**
 *  Count bits in all vector fields, accumulation step.
 */
#define RC_VEC_CNTV(accv, srcv)                        \
do {                                                   \
    rc_vec_t acc__ = (srcv);                           \
    acc__   = ( acc__       & RC_WORD_C8(55)) +        \
              ((acc__ >> 1) & RC_WORD_C8(55));         \
    acc__   = ( acc__       & RC_WORD_C8(33)) +        \
              ((acc__ >> 2) & RC_WORD_C8(33));         \
    (accv) += (acc__ + (acc__ >> 4)) & RC_WORD_C8(0f); \
} while (0)

/**
 *  Count bits in all vector fields, reduction step.
 */
#define RC_VEC_CNTR(cnt, accv) \
do {                           \
    rc_vec_t sum__ = 0;        \
    RC_VEC_SUMV(sum__, accv);  \
    RC_VEC_SUMR(cnt, sum__);   \
} while (0)

/**
 *  Sum all 8-bit vector fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_SUMV() and RC_VEC_SUMR() together computes the sum.
 *  The accumulation step can be iterated at most RC_VEC_SUMN times
 *  before the reduction step.
 */
#define RC_VEC_SUMN 128

/**
 *  Sum all 8-bit vector fields, accumulation step.
 */
#define RC_VEC_SUMV(accv, srcv)                   \
do {                                              \
    rc_vec_t sv__ = (srcv);                       \
    (accv) += ( sv__       & RC_WORD_C16(00ff)) + \
              ((sv__ >> 8) & RC_WORD_C16(00ff));  \
} while (0)

/**
 *  Sum all 8-bit vector fields, reduction step.
 */
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
