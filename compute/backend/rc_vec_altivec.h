/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_altivec.h
 *  @brief  RAPP Compute layer vector operations
 *          using the AltiVec instruction set.
 */

#ifndef RC_VEC_ALTIVEC_H
#define RC_VEC_ALTIVEC_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <altivec.h> /* AltiVec intrinsics */

/**
 *  See configure.ac. We stick to standard ISO syntax, deliberately
 *  avoiding GCC extensions.
 */
#if RC_ALTIVEC_CURLY_BRACES
/**
 *  By specifying the minimally used number of arguments - 1, we
 *  catch typos like forgetting the "16" suffix.
 */ 
#define RC_ALTIVEC_INIT(x1, x2, x3, ...) (rc_vec_t){x1, x2, x3, __VA_ARGS__}
#define RC_ALTIVEC_INIT16(x) \
    (rc_vec_t){x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x}
#define RC_ALTIVEC_TINIT(type, x, ...) (type){x, __VA_ARGS__}
#define RC_ALTIVEC_TINIT4(type, x) (type){x, x, x, x}
#define RC_ALTIVEC_TINIT8(type, x) (type){x, x, x, x, x, x, x, x}
#else
#define RC_ALTIVEC_INIT(x1, x2, x3, ...) (rc_vec_t)(x1, x2, x3, __VA_ARGS__)
#define RC_ALTIVEC_INIT16(x) (rc_vec_t)(x)
#define RC_ALTIVEC_TINIT(type, x, ...) (type)(x, __VA_ARGS__)
#define RC_ALTIVEC_TINIT4(type, x) (type)(x)
#define RC_ALTIVEC_TINIT8(type, x) (type)(x)
#endif

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
 *  Use AVGR/SUBHR instead of AVGT/SUBHT when possible.
 */
#define RC_VEC_HINT_AVGR

/*
 * -------------------------------------------------------------
 *  The vector type
 * -------------------------------------------------------------
 */

/**
 *  The vector type definition.
 *
 *  The AltiVec PIM says (in 2.2 New Keywords) we can only use "vector"
 *  in front of basic integer types; specifically not typedefs.
 *  For such use, it mandates using "__vector".
 */
typedef __vector uint8_t rc_vec_t;

/**
 *  The number of bytes in a vector.
 */
#define RC_VEC_SIZE 16


/*
 * -------------------------------------------------------------
 *  Vector state
 * -------------------------------------------------------------
 */

/**
 *  No global declarations is needed for AltiVec registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for AltiVec registers.
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
    ((vec) = vec_ld(0, (const uint8_t*)(ptr)))

/**
 *  Store a vector in memory.
 */
#define RC_VEC_STORE(ptr, vec) \
    vec_st(vec, 0, (uint8_t*)(ptr))


/*
 * -------------------------------------------------------------
 *  Misaligned memory access
 * -------------------------------------------------------------
 */

/**
 *  Initialize a sequence of misaligned loads.
 */
#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                          \
do {                                                                        \
    const uint8_t *sptr__, *uptr__ = (ptr);                                 \
    (void)(vec3);                                /* Unused               */ \
    sptr__ = (ptr);                              /* Cache source pointer */ \
    uptr__ = (uint8_t*)((intptr_t)sptr__ & ~15); /* Align down           */ \
    (vec1) = vec_lvsl(0, sptr__);                /* Generate permutation */ \
    (vec2) = vec_ld(0, uptr__);                  /* Load previous vector */ \
    (uptr) = uptr__ + 16;                        /* Advance data pointer */ \
} while (0)

/**
 *  Misaligned vector load from memory.
 */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)                       \
do {                                                                     \
    rc_vec_t vec__ = vec_ld(0, (const uint8_t*)(uptr)); /* Load next  */ \
    (dstv) = vec_perm(vec2, vec__, vec1);               /* Permute    */ \
    (vec2) = vec__;                                     /* Update old */ \
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
    RC_VEC_SPLAT(shv, 8*(bytes))

/**
 *  Shift all fields to the logical left.
 */
#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = vec_slo(srcv, shv))

/**
 *  Shift all fields to the logical right.
 */
#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = vec_sro(srcv, shv))

/**
 *  Shift all fields to the logical left by a constant value.
 */
#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = vec_slo(srcv, RC_ALTIVEC_INIT16(8*(bytes))))

/**
 *  Shift fields to the logical right by a constant value.
 */
#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = vec_sro(srcv, RC_ALTIVEC_INIT16(8*(bytes))))

/**
 *  Align srcv1 and srcv2 to dstv, starting at field @e bytes
 *  into concatenation of srcv1 and srcv2. The alignment
 *  value @e bytes must be a constant.
 */
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = vec_sld(srcv1, srcv2, bytes))

/**
 *  Pack the even fields of srcv1 and srcv2 into one vector,
 *  with fields 0, ..., RC_VEC_SIZE/2 - 1 from srcv1 and
 *  fields RC_VEC_SIZE/2, ..., RC_VEC_SIZE - 1 from srcv2.
 */
#define RC_VEC_PACK(dstv, srcv1, srcv2)                                 \
    ((dstv) = vec_perm(srcv1, srcv2,                                    \
                       RC_ALTIVEC_INIT( 0,  2,  4,  6,  8, 10, 12, 14,  \
                                       16, 18, 20, 22, 24, 26, 28, 30)))

/*
 * -------------------------------------------------------------
 *  Bitwise logical operations
 * -------------------------------------------------------------
 */

/**
 *  Set all bits to zero.
 */
#define RC_VEC_ZERO(vec) \
    ((vec) = RC_ALTIVEC_INIT16(0))

/**
 *  Bitwise NOT.
 */
#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = vec_nor(srcv, srcv))

/**
 *  Bitwise AND.
 */
#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = vec_and(srcv1, srcv2))

/**
 *  Bitwise OR.
 */
#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = vec_or(srcv1, srcv2))

/**
 *  Bitwise XOR.
 */
#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = vec_xor(srcv1, srcv2))

/**
 *  Bitwise AND NOT.
 */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = vec_andc(srcv1, srcv2))

/**
 *  Bitwise OR NOT.
 */
#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
do {                                     \
    rc_vec_t neg__;                      \
    RC_VEC_NOT(neg__, srcv2);            \
    RC_VEC_OR(dstv, srcv1, neg__);       \
} while (0)

/**
 *  Bitwise XOR NOT.
 */
#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
do {                                      \
    rc_vec_t neg__;                       \
    RC_VEC_NOT(neg__, srcv2);             \
    RC_VEC_XOR(dstv, srcv1, neg__);       \
} while (0)


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
    RC_VEC_SPLAT__(vec, scal, uint8_t)

/**
 *  Saturating addition.
 *  Computes dstv = MIN(srcv1 + srcv2, 0xff) for each 8-bit field.
 */
#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = vec_adds(srcv1, srcv2))

/**
 *  Average value, truncated.
 *  Computes dstv = (srcv1 + srcv2) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGT(dstv, srcv1, srcv2)        \
do {                                           \
    rc_vec_t sv1__ = (srcv1);                  \
    rc_vec_t sv2__ = (srcv2);                  \
    rc_vec_t adj__;                            \
    adj__  = vec_xor(sv1__, sv2__);            \
    sv1__  = vec_avg(sv1__, sv2__);            \
    adj__  = vec_and(adj__,                    \
                     RC_ALTIVEC_INIT16(0x01)); \
    (dstv) = vec_sub(sv1__, adj__);            \
} while (0)

/**
 *  Average value, rounded.
 *  Computes dstv = (srcv1 + srcv2 + 1) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = vec_avg(srcv1, srcv2))

/**
 *  Average value, rounded towards srcv1.
 *  Computes dstv = (srcv1 + srcv2 + (srcv1 > srcv2)) >> 1
 *  for each 8-bit field.
 */
#define RC_VEC_AVGZ(dstv, srcv1, srcv2)        \
do {                                           \
    rc_vec_t sv1__ = (srcv1);                  \
    rc_vec_t sv2__ = (srcv2);                  \
    rc_vec_t avg__ = vec_avg(sv1__, sv2__);    \
    rc_vec_t xor__ = vec_xor(sv1__, sv2__);    \
    rc_vec_t cmp__ = (rc_vec_t)                \
                     vec_cmpgt(sv2__, sv1__);  \
    xor__  = vec_and(xor__, cmp__);            \
    xor__  = vec_and(xor__,                    \
                     RC_ALTIVEC_INIT16(0x01)); \
    (dstv) = vec_sub(avg__, xor__);            \
} while (0)

/**
 *  Saturating subtraction.
 *  Computes dstv = MAX(srvc1 - srcv2, 0) for each 8-bit field.
 */
#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = vec_subs(srcv1, srcv2))

/**
 *  Absolute-value subtraction.
 *  Computes dstv = ABS(srcv1 - srcv2) for each 8-bit field.
 */
#define RC_VEC_SUBA(dstv, srcv1, srcv2)                               \
do {                                                                  \
    rc_vec_t sv1__ = (srcv1);                                         \
    rc_vec_t sv2__ = (srcv2);                                         \
    ((dstv) = vec_sub(vec_max(sv1__, sv2__), vec_min(sv1__, sv2__))); \
} while (0)

/**
 *  Half subtraction with bias, truncated.
 *  Computes dstv = (srcv1 - srcv2 + 0xff) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, vec_nor(srcv2, srcv2))

/**
 *  Half subtraction with bias, rounded.
 *  Computes dstv = (srcv1 - srcv2 + 0x100) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_VEC_AVGR(dstv, srcv1, vec_nor(srcv2, srcv2))

/**
 *  Absolute value.
 *  Computes dstv = 2*abs(srcv - 0x80) for each 8-bit field.
 *  The result is saturated to [0,0xff].
 */
#define RC_VEC_ABS(dstv, srcv)                \
do {                                          \
    rc_vec_t vec__;                           \
    vec__  = vec_xor(srcv,                    \
                     RC_ALTIVEC_INIT16(0x80));\
    vec__  = (rc_vec_t)                       \
             vec_abs((__vector int8_t)vec__); \
    (dstv) = vec_adds(vec__, vec__);          \
} while (0)

/**
 *  Comparison.
 *  Computes dstv = srcv1 > srcv2 ? 0x80 : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = (rc_vec_t)vec_cmpgt(srcv1, srcv2))

/**
 *  Comparison.
 *  Computes dst = srcv1 >= srcv2 ? 0x80 : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGE(dstv, srcv1, srcv2)     \
do {                                         \
    rc_vec_t gt__ = (rc_vec_t)               \
                    vec_cmpgt(srcv2, srcv1); \
    (dstv) = vec_nor(gt__, gt__);            \
} while (0)

/**
 *  Minimum value.
 *  Computes dstv = MIN(srcv1, srcv2) for each 8-bit field.
 */
#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = vec_min(srcv1, srcv2))

/**
 *  Maximum value.
 *  Computes dstv = MAX(srcv1, srcv2) for each 8-bit field.
 */
#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = vec_max(srcv1, srcv2))

/**
 *  Generate the blend vector needed by RC_VEC_LERP().
 */
#define RC_VEC_BLEND(blendv, blend8) \
    RC_VEC_SPLAT__(blendv, (blend8) << 7, int16_t)

/**
 *  Linear interpolation.
 *  Computes dstv = srcv1 + ((blend8*(srcv2 - srcv1) + 0x80) >> 8) for each
 *  8-bit field. The Q.8 blend factor @e blend8 must be in the range [0,0x7f].
 */
#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)      \
do {                                                         \
    rc_vec_t       sv1__ = (srcv1);                          \
    rc_vec_t       sv2__ = (srcv2);                          \
    vector short   bv__  = (vector short)(blendv);           \
    vector short   lo1__ = (vector short)                    \
                           vec_mergel(RC_ALTIVEC_INIT16(0),  \
                                      sv1__);                \
    vector short   hi1__ = (vector short)                    \
                           vec_mergeh(RC_ALTIVEC_INIT16(0),  \
                                      sv1__);                \
    vector short   lo2__ = (vector short)                    \
                           vec_mergel(RC_ALTIVEC_INIT16(0),  \
                                      sv2__);                \
    vector short hi2__   = (vector short)                    \
                           vec_mergeh(RC_ALTIVEC_INIT16(0),  \
                                      sv2__);                \
    lo2__  = vec_sub(lo2__, lo1__);                          \
    hi2__  = vec_sub(hi2__, hi1__);                          \
    lo1__  = vec_mradds(lo2__, bv__, lo1__);                 \
    hi1__  = vec_mradds(hi2__, bv__, hi1__);                 \
    (dstv) = (rc_vec_t)vec_pack(hi1__, lo1__);               \
} while (0)


/**
 *  Generate the blend vector needed by RC_VEC_LERPZ().
 */
#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_SPLAT__(blendv, blend8, int16_t)

/**
 *  Linear interpolation rounded towards srcv1.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded towards zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 */
#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blend8v) \
do {                                                      \
    rc_vec_t sv1__ = (srcv1);                             \
    rc_vec_t sv2__ = (srcv2);                             \
    rc_vec_t bias__ = (rc_vec_t)vec_cmpgt(sv1__, sv2__);  \
    RC_VEC_LERP__(dstv, sv1__, sv2__, blend8v, bias__);   \
} while (0)

/**
 *  Generate the blend vector needed by RC_VEC_LERPN().
 */
#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_SPLAT__(blendv, blend8, int16_t)

/**
 *  Linear interpolation rounded towards srcv2.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded away from zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 */
#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blend8v) \
do {                                                      \
    rc_vec_t sv1__ = (srcv1);                             \
    rc_vec_t sv2__ = (srcv2);                             \
    rc_vec_t bias__ = (rc_vec_t)vec_cmpgt(sv2__, sv1__);  \
    RC_VEC_LERP__(dstv, sv1__, sv2__, blend8v, bias__);   \
} while (0)


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

/**
 *  Pack the most significant bits in each 8-bit field to the
 *  logically left-most bits in a binary vector mask. The unused
 *  fields in the mask vector are undefined.
 */
#define RC_VEC_GETMASKV(maskv, vec)                                     \
do {                                                                    \
    rc_vec_t        vec__  = (vec);                                     \
    vector unsigned mask__ = RC_ALTIVEC_TINIT(vector unsigned,          \
                                              0x80402010, 0x08040201,   \
                                              0x80402010, 0x08040201);  \
    vec__   = (rc_vec_t)vec_cmpgt(vec__, RC_ALTIVEC_INIT16(0x7f));      \
    mask__  = vec_and((vector unsigned)vec__, mask__);                  \
    mask__  = vec_sum4s((rc_vec_t)mask__,                               \
                        RC_ALTIVEC_TINIT4(vector unsigned, 0));         \
    mask__  = vec_sl(mask__,                                            \
                     RC_ALTIVEC_TINIT(vector unsigned, 8, 8, 0, 0));    \
    mask__  = (vector unsigned)                                         \
              vec_sums((vector int)mask__,                              \
                       RC_ALTIVEC_TINIT4(vector int, 0));               \
    (maskv) = vec_slo((rc_vec_t)mask__, RC_ALTIVEC_INIT16(8*14));       \
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
#define RC_VEC_CNTN 1 /* 0xfffffff untestable */

/**
 *  Count bits in all vector fields, accumulation step.
 */
#define RC_VEC_CNTV(accv, srcv)                                          \
do {                                                                     \
    vector int c1__  = RC_ALTIVEC_TINIT4(vector int, 0x55555555);        \
    vector int c2__  = RC_ALTIVEC_TINIT4(vector int, 0x33333333);        \
    vector int c4__  = RC_ALTIVEC_TINIT4(vector int, 0x0f0f0f0f);        \
    vector int acc__ = (vector int)(srcv);                               \
    vector int tmp__;                                                    \
    tmp__  = vec_sr(acc__,                        /* tmp  = acc >> 1  */ \
                    RC_ALTIVEC_TINIT4(vector unsigned, 1));              \
    tmp__  = vec_and(tmp__, c1__);                /* tmp &= 0x55...   */ \
    acc__  = vec_and(acc__, c1__);                /* acc &= 0x55...   */ \
    acc__  = vec_add(acc__, tmp__);               /* acc += tmp       */ \
    tmp__  = vec_sr(acc__,                        /* tmp  = acc >> 2  */ \
                    RC_ALTIVEC_TINIT4(vector unsigned, 2));              \
    tmp__  = vec_and(tmp__, c2__);                /* tmp &= 0x33...   */ \
    acc__  = vec_and(acc__, c2__);                /* acc &= 0x33...   */ \
    acc__  = vec_add(acc__, tmp__);               /* acc += tmp       */ \
    tmp__  = vec_sr(acc__,                        /* tmp  = acc >> 4  */ \
                    RC_ALTIVEC_TINIT4(vector unsigned, 4));              \
    acc__  = vec_add(acc__, tmp__);               /* acc += tmp       */ \
    acc__  = vec_and(acc__, c4__);                /* acc &= 0x0f...   */ \
    RC_VEC_SUMV(accv, (rc_vec_t)acc__);                                  \
} while (0)

/**
 *  Count bits in all vector fields, reduction step.
 */
#define RC_VEC_CNTR(cnt, accv) \
    RC_VEC_SUMR(cnt, accv)

/**
 *  Sum all 8-bit fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_SUMV() and RC_VEC_SUMR() together computes the sum.
 *  The accumulation step can be iterated at most RC_VEC_SUMN times
 *  before the reduction step.
 */
#define RC_VEC_SUMN 1024 /* 4210752 untestable */

/**
 *  Sum all 8-bit fields, accumulation step.
 */
#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = (rc_vec_t)vec_sum4s(srcv, (vector unsigned)(accv)))

/**
 *  Sum all 8-bit fields, reduction step.
 */
#define RC_VEC_SUMR(sum, accv) \
    RC_VEC_VTOI__(sum, vec_sums((vector int)(accv), \
                                RC_ALTIVEC_TINIT4(vector int, 0)), int)

/**
 *  Multiply and accumulate all 8-bit fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_MACV() and RC_VEC_MACR() together computes the sum.
 *  The accumulation step can be iterated at most RC_VEC_MACN times
 *  before the reduction step.
 */
#define RC_VEC_MACN 1024 /* 16512 untestable */

/**
 *  Multiply and accumulate all 8-bit fields, accumulation step.
 */
#define RC_VEC_MACV(accv, srcv1, srcv2) \
    ((accv) = (rc_vec_t)vec_msum(srcv1, srcv2, (vector unsigned)(accv)))

/**
 *  Multiply and accumulate all 8-bit fields, reduction step.
 */
#define RC_VEC_MACR(sum, accv) \
    RC_VEC_SUMR(sum, accv)


/*
 * -------------------------------------------------------------
 *  Internal support macros
 * -------------------------------------------------------------
 */

/**
 *  Splat a scalar into a vector of arbitrary type.
 */
#define RC_VEC_SPLAT__(vec, scal, type)        \
do {                                           \
    type        scal__ = (scal);               \
    rc_vec_t    map__  = vec_lvsl(0, &scal__); \
    __vector type vec__ = vec_lde(0, &scal__); \
    map__ = (rc_vec_t)                         \
            vec_splat((__vector type)map__, 0);\
    (vec) = (rc_vec_t)                         \
            vec_perm(vec__, vec__, map__);     \
} while (0)

/**
 *  Convert the first element of a vector to a scalar.
 */
#define RC_VEC_VTOI__(scal, vec, type)                            \
do {                                                              \
    type        scal__;                                           \
    __vector type vec__;                                          \
    vec__ =                                                       \
        vec_splat((__vector type)(vec), 16 / sizeof(type) - 1);   \
    vec_ste(vec__, 0, &scal__);                                   \
    (scal) = scal__;                                              \
} while (0)

/**
 *  Linear interpolation with custom rounding.
 */
#define RC_VEC_LERP__(dstv, srcv1, srcv2, blend8v, bias)     \
do {                                                         \
    vector short   bv__  = (vector short)(blendv);           \
    vector short   lob__ = (vector short)                    \
                           vec_mergel(RC_ALTIVEC_INIT16(0),  \
                                      bias);                 \
    vector short   hib__ = (vector short)                    \
                           vec_mergeh(RC_ALTIVEC_INIT16(0),  \
                                      bias);                 \
    vector short   lo1__ = (vector short)                    \
                           vec_mergel(RC_ALTIVEC_INIT16(0),  \
                                      srcv1);                \
    vector short   hi1__ = (vector short)                    \
                           vec_mergeh(RC_ALTIVEC_INIT16(0),  \
                                      srcv1);                \
    vector short   lo2__ = (vector short)                    \
                           vec_mergel(RC_ALTIVEC_INIT16(0),  \
                                      srcv2);                \
    vector short   hi2__ = (vector short)                    \
                           vec_mergeh(RC_ALTIVEC_INIT16(0),  \
                                      srcv2);                \
    lo2__  = vec_sub(lo2__, lo1__);                          \
    hi2__  = vec_sub(hi2__, hi1__);                          \
    lo1__  = vec_mladd(lo2__, bv__, lob__);                  \
    hi1__  = vec_mladd(hi2__, bv__, hib__);                  \
    lo1__  = vec_sr(lo1__,                                   \
                    RC_ALTIVEC_TINIT8(vector unsigned short, \
                                      8));                   \
    hi1__  = vec_sr(hi1__,                                   \
                    RC_ALTIVEC_TINIT8(vector unsigned short, \
                                      8));                   \
    (dstv) = (rc_vec_t)vec_pack(hi1__, lo1__);               \
    (dstv) = vec_add(dstv, srcv1);                           \
} while (0)

#endif /* RC_VEC_ALTIVEC_H */
