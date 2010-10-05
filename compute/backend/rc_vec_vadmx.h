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
 *  @file   rc_vec_vadmx.h
 *  @brief  RAPP Compute layer vector operations
 *          using 64-bit VADMX instructions.
 */

#ifndef RC_VEC_VADMX_H
#define RC_VEC_VADMX_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */


/*
 * -------------------------------------------------------------
 *  The vector type
 * -------------------------------------------------------------
 */

/**
 *  The vector type definition.
 */
typedef uint8_t rc_vec_t __attribute__ ((__vector_size__(8)));

/**
 *  The number of bytes in a vector.
 */
#define RC_VEC_SIZE 8


/*
 * -------------------------------------------------------------
 *  Vector state
 * -------------------------------------------------------------
 */

/**
 *  No global declarations is needed for VADMX registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for VADMX registers.
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
    ((vec) = *(const rc_vec_t*)(ptr))

/**
 *  Store a vector in memory.
 */
#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))


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
    const uint8_t *sptr__, *uptr__;                                         \
    unsigned       off__;                                                   \
    sptr__ = (ptr);                              /* Cache source pointer */ \
    uptr__ = (const uint8_t*)((uintptr_t)sptr__ & ~7); /* Align down     */ \
    off__  = (uintptr_t)sptr__ & 7;              /* Alignment offset     */ \
    RC_VEC_SPLAT(vec1, off__);                   /* Alignment byte shift */ \
    RC_VEC_SPLAT(vec2, 8 - off__);               /* Complementary shift  */ \
    RC_VEC_LOAD(vec3, uptr__);                   /* Load first vector    */ \
    (uptr) = uptr__ + 8;                         /* Advance data pointer */ \
} while (0)

/**
 *  Misaligned vector load from memory.
 */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)            \
do {                                                          \
    rc_vec_t vec__;                                           \
    RC_VEC_LOAD(vec__, uptr);                                 \
    RC_VEC_OR(dstv, __builtin_ax_vsr(vec3,  vec1),            \
                    __builtin_ax_vsl(vec__, vec2));           \
    (vec3) = vec__;                                           \
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
    RC_VEC_SPLAT(shv, bytes)

/**
 *  Shift all fields to the logical left.
 */
#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = __builtin_ax_vsr(srcv, shv))

/**
 *  Shift all fields to the logical right.
 */
#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = __builtin_ax_vsl(srcv, shv))

/**
 *  Shift all fields to the logical left by a constant value.
 */
#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = __builtin_ax_vsri(srcv, bytes))

/**
 *  Shift fields to the logical right by a constant value.
 */
#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = __builtin_ax_vsli(srcv, bytes))

/**
 *  Align srcv1 and srcv2 to dstv, starting at field @e bytes
 *  into concatenation of srcv1 and srcv2. The alignment
 *  value @e bytes must be a constant.
 */
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = __builtin_ax_valign(srcv1, srcv2, bytes))

/**
 *  Pack the even fields of srcv1 and srcv2 into one vector,
 *  with fields 0, ..., RC_VEC_SIZE/2 - 1 from srcv1 and
 *  fields RC_VEC_SIZE/2, ..., RC_VEC_SIZE - 1 from srcv2.
 */
#define RC_VEC_PACK(dstv, srcv1, srcv2)                   \
do {                                                      \
    rc_vec_t perm__ = (rc_vec_t){0, 2, 4, 6, 0, 0, 0, 0}; \
    rc_vec_t sv1__  = __builtin_ax_vshfl(srcv1, perm__); \
    rc_vec_t sv2__  = __builtin_ax_vshfl(srcv2, perm__); \
    (dstv) = __builtin_ax_vcomb(sv1__, sv2__, 4);         \
} while (0)


/*
 * -------------------------------------------------------------
 *  Bitwise logical operations
 * -------------------------------------------------------------
 */

/**
 *  Set all bits to zero.
 */
#define RC_VEC_ZERO(vec) \
    RC_VEC_SPLAT(vec, 0)

/**
 *  Bitwise NOT.
 *  We could write these trivial operators as open code expressions
 *  (e.g. ((dstv) = ~(srcv)), but keeping them as builtins apparently
 *  has positive effects on scheduling and code size (at least for RAPP,
 *  possibly by chance), so let's be consistent, at least until insn
 *  combination works for vector types and we see an actual improvement.
 */
#define RC_VEC_NOT(dstv, srcv) \
   ((dstv) = __builtin_ax_vnot(srcv))

/**
 *  Bitwise AND.
 */
#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vand(srcv1, srcv2))

/**
 *  Bitwise OR.
 */
#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vor(srcv1, srcv2))

/**
 *  Bitwise XOR.
 */
#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vxor(srcv1, srcv2))

/**
 *  Bitwise AND NOT.
 *  In gcc-4.3.x (at least), combine doesn't happen for vector
 *  operations, so don't express these as open coded expressions, or
 *  we'll get two instructions.
 */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vandn(srcv1, srcv2))

/**
 *  Bitwise OR NOT.
 */
#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vorn(srcv1, srcv2))

/**
 *  Bitwise XOR NOT.
 */
#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vxorn(srcv1, srcv2))


/*
 * -------------------------------------------------------------
 *  Arithmetic operations on 8-bit fields
 * -------------------------------------------------------------
 */

/**
 *  Set a value to all fields.
 *  Sets the scalar value 'scal' in each 8-bit field.
 *  Don't express this as a constant vector until GCC generates the
 *  equivalent code for such vector constants.
 */
#define RC_VEC_SPLAT(vec, scal) \
    ((vec) = __builtin_ax_splat(scal))

/**
 *  Absolute value.
 *  Computes dstv = 2*abs(srcv - 0x80) for each 8-bit field.
 *  The result is saturated to [0,0xff].
 */
#define RC_VEC_ABS(dstv, srcv) \
    ((dstv) = __builtin_ax_vabs(srcv))

/**
 *  Saturating addition.
 *  Computes dstv = MIN(srcv1 + srcv2, 0xff) for each 8-bit field.
 */
#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vadds(srcv1, srcv2))

/**
 *  Average value, truncated.
 *  Computes dstv = (srcv1 + srcv2) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vavgt(srcv1, srcv2))

/**
 *  Average value, rounded.
 *  Computes dstv = (srcv1 + srcv2 + 1) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vavgr(srcv1, srcv2))

/**
 *  Average value, rounded towards srcv1.
 *  Computes dstv = (srcv1 + srcv2 + (srcv1 > srcv2)) >> 1
 *  for each 8-bit field.
 */
#define RC_VEC_AVGZ(dstv, srcv1, srcv2)            \
do {                                               \
    rc_vec_t blendv__;                             \
    RC_VEC_SPLAT(blendv__, 0x80);                  \
    RC_VEC_LERPZ(dstv, srcv1, srcv2, 0, blendv__); \
} while (0)

/**
 *  Saturating subtraction.
 *  Computes dstv = MAX(srcv1 - srcv2, 0) for each 8-bit field.
 */
#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubs(srcv1, srcv2))

/**
 *  Absolute-value subtraction.
 *  Computes dstv = ABS(srcv1 - srcv2) for each 8-bit field.
 */
#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsuba(srcv1, srcv2))

/**
 *  Half subtraction with bias, truncated.
 *  Computes dstv = (srcv1 - srcv2 + 0xff) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubht(srcv1, srcv2))

/**
 *  Half subtraction with bias, rounded.
 *  Computes dstv = (srcv1 - srcv2 + 0x100) >> 1 for each 8-bit field.
 */
#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubhr(srcv1, srcv2))

/**
 *  Comparison.
 *  Computes dstv = srcv1 > srcv2 ? 0xff : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vcmpgt(srcv1, srcv2))

/**
 *  Comparison.
 *  Computes dstv = srcv1 >= srcv2 ? 0xff : 0 for each 8-bit field.
 */
#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vcmpge(srcv1, srcv2))

/**
 *  Minimum value.
 *  Computes dstv = MIN(srcv1, srcv2) for each 8-bit field.
 */
#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vmin(srcv1, srcv2))

/**
 *  Maximum value.
 *  Computes dstv = MAX(srcv1, srcv2) for each 8-bit field.
 */
#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vmax(srcv1, srcv2))

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
#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                    \
    rc_vec_t lo__ = __builtin_ax_vzipl(srcv1, srcv2);   \
    rc_vec_t hi__ = __builtin_ax_vziph(srcv1, srcv2);   \
    lo__   = __builtin_ax_vlerp(lo__, blendv);          \
    hi__   = __builtin_ax_vlerp(hi__, blendv);          \
    (dstv) = __builtin_ax_vcomb(lo__, hi__, 4);         \
} while (0)

/**
 *  Generate the blend vector needed by RC_VEC_LERPZ().
 */
#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_SPLAT(blendv, 0x100 - (blend8))

/**
 *  Linear interpolation rounded towards srcv1.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded towards zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 */
#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv) \
    RC_VEC_LERPN(dstv, srcv2, srcv1, 0, blendv)

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
#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                     \
    rc_vec_t lo__ = __builtin_ax_vzipl(srcv1, srcv2);    \
    rc_vec_t hi__ = __builtin_ax_vziph(srcv1, srcv2);    \
    lo__   = __builtin_ax_vlerpnz(lo__, blendv);         \
    hi__   = __builtin_ax_vlerpnz(hi__, blendv);         \
    (dstv) = __builtin_ax_vcomb(lo__, hi__, 4);          \
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
#define RC_VEC_GETMASKV(maskv, vec) \
    ((maskv) = __builtin_ax_rpack(vec))


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
#define RC_VEC_CNTN  28

/**
 *  Count bits in all vector fields, accumulation step.
 *  In gcc-4.3.x (at least), __builtin_popcount doesn't take vector
 *  type arguments and return value.
 */
#define RC_VEC_CNTV(accv, srcv) \
    ((accv) = __builtin_ax_vadd(accv, __builtin_ax_vpcnt(srcv)))

/**
 *  Count bits in all vector fields, reduction step.
 */
#define RC_VEC_CNTR(cnt, accv)                 \
do {                                           \
    rc_vec_t sumv__ = __builtin_ax_rsum(accv); \
    RC_VEC_SUMR(cnt, sumv__);                  \
} while (0)

/**
 *  Sum all 8-bit fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_SUMV() and RC_VEC_SUMR() together computes the sum.
 *  The accumulation step can be iterated at most RC_VEC_SUMN times
 *  before the reduction step.
 */
#define RC_VEC_SUMN 1

/**
 *  Sum all 8-bit fields, accumulation step.
 */
#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = __builtin_ax_rsum(srcv))

/**
 *  Sum all 8-bit fields, reduction step.
 */
#define RC_VEC_SUMR(sum, accv) \
    ((sum) = ((union { int i; rc_vec_t v; })(accv)).i)

/**
 *  Multiply and accumulate all 8-bit fields.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_MACV() and RC_VEC_MACR() together computes the sum.
 *  The accumulation step can be iterated at most RC_VEC_MACN times
 *  before the reduction step.
 */
#define RC_VEC_MACN 1

/**
 *  Multiply and accumulate all 8-bit fields, accumulation step.
 */
#define RC_VEC_MACV(accv, srcv1, srcv2) \
    ((accv) = __builtin_ax_rdot(srcv1, srcv2))

/**
 *  Multiply and accumulate all 8-bit fields, reduction step.
 */
#define RC_VEC_MACR(mac, accv) \
    RC_VEC_SUMR(mac, accv)

#endif /* RC_VEC_VADMX_H */
