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
 *  @file   rc_vec_ref.h
 *  @brief  Vector operation reference implementation.
 */

#ifndef RC_VEC_REF_H
#define RC_VEC_REF_H

#include <string.h>    /* memcpy(), memset() */
#include <stdint.h>    /* uint8_t            */
#include "rc_vector.h" /* RC_VEC_SIZE        */

/**
 *  Define the vector size to something if not set. Setting it to 1
 *  causes compiler warnings from (never called) reference functions
 *  that only make sense for vectors with multiple fields, and we don't
 *  want to litter the code with #if's, so just set it to 2.
 */
#ifndef RC_VEC_SIZE
#define RC_VEC_SIZE 2
#endif

/*
 * -------------------------------------------------------------
 *  The vector type
 * -------------------------------------------------------------
 */

/**
 *  The reference vector type definition.
 */
typedef union {
    uint8_t  byte[RC_VEC_SIZE];
#if RC_VEC_SIZE == 2
    uint16_t word;
#else
    uint32_t word;
#endif
} rc_vec_ref_t;

/**
 *  Override the previously defined vector type.
 */
#define rc_vec_t rc_vec_ref_t

/*
 * -------------------------------------------------------------
 *  Register clean up
 * -------------------------------------------------------------
 */

/**
 *  No global declarations needed.
 */
#undef  RC_VEC_DECLARE
#define RC_VEC_DECLARE()

/**
 *  No global clean up needed.
 */
#undef  RC_VEC_CLEANUP
#define RC_VEC_CLEANUP()

/*
 * -------------------------------------------------------------
 *  Memory access
 * -------------------------------------------------------------
 */

/**
 *  Load a vector from memory.
 */
#undef  RC_VEC_LOAD
#define RC_VEC_LOAD(vec, ptr) \
    memcpy((vec).byte, ptr, RC_VEC_SIZE)

/**
 *  Store a vector in memory.
 */
#undef  RC_VEC_STORE
#define RC_VEC_STORE(ptr, vec) \
    memcpy(ptr, (vec).byte, RC_VEC_SIZE)

/*
 * -------------------------------------------------------------
 *  Misaligned memory access
 * -------------------------------------------------------------
 */

/**
 *  Initialize a sequence of misaligned loads.
 */
#undef  RC_VEC_LDINIT
#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr) \
do {                                               \
    (void)(vec1);                                  \
    (void)(vec2);                                  \
    (void)(vec3);                                  \
    (uptr) = (ptr);                                \
} while (0)

/**
 *  Misaligned load from memory.
 */
#undef  RC_VEC_LOADU
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
    RC_VEC_LOAD(dstv, uptr)


/*
 * -------------------------------------------------------------
 *  Field relocation
 * -------------------------------------------------------------
 */

/**
 *  Initialize a field shift.
 */
#undef  RC_VEC_SHINIT
#define RC_VEC_SHINIT(shv, bytes) \
    ((shv).word = (bytes))

/**
 *  Shift all fields to the logical left.
 */
#undef  RC_VEC_SHL
#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = rc_vec_shl_ref(srcv, (shv).word))

/**
 *  Shift fields to the logical right.
 */
#undef  RC_VEC_SHR
#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = rc_vec_shr_ref(srcv, (shv).word))

/**
 *  Shift all fields to the logical left by a constant value.
 */
#undef  RC_VEC_SHLC
#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = rc_vec_shl_ref(srcv, bytes))

/**
 *  Shift all fields to the logical right by a constant value.
 */
#undef  RC_VEC_SHRC
#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = rc_vec_shr_ref(srcv, bytes))

/**
 *  Align srcv1 and srcv2 using the right @e bytes bytes from srcv1,
 *  and RC_VEC_SIZE - @e bytes bytes from srcv2. The alignment value
 *  must be a compile-time constant.
 */
#undef  RC_VEC_ALIGNC
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = rc_vec_align_ref(srcv1, srcv2, bytes))

/**
 *  Pack even fields to one vector.
 */
#undef  RC_VEC_PACK
#define RC_VEC_PACK(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_pack_ref(srcv1, srcv2))


/*
 * -------------------------------------------------------------
 *  Bitwise logical operations
 * -------------------------------------------------------------
 */

/**
 *  Set all bits to zero.
 */
#undef  RC_VEC_ZERO
#define RC_VEC_ZERO(vec) \
    memset((vec).byte, 0, RC_VEC_SIZE)

/**
 *  Bitwise NOT.
 */
#undef  RC_VEC_NOT
#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = rc_vec_not_ref(srcv))

/**
 *  Bitwise AND.
 */
#undef  RC_VEC_AND
#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_and_ref(srcv1, srcv2))

/**
 *  Bitwise OR.
 */
#undef  RC_VEC_OR
#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_or_ref(srcv1, srcv2))

/**
 *  Bitwise XOR.
 */
#undef  RC_VEC_XOR
#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_xor_ref(srcv1, srcv2))

/**
 *  Bitwise AND NOT.
 */
#undef  RC_VEC_ANDNOT
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_and_ref(srcv1, rc_vec_not_ref(srcv2)))

/**
 *  Bitwise OR NOT.
 */
#undef  RC_VEC_ORNOT
#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_or_ref(srcv1, rc_vec_not_ref(srcv2)))

/**
 *  Bitwise XOR NOT.
 */
#undef  RC_VEC_XORNOT
#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_xor_ref(srcv1, rc_vec_not_ref(srcv2)))


/*
 * -------------------------------------------------------------
 *  Arithmetic operations on 8-bit fields
 * -------------------------------------------------------------
 */

/**
 *  Set a value to all fields.
 *  Sets the scalar value 'scal' in each 8-bit field.
 */
#undef  RC_VEC_SPLAT
#define RC_VEC_SPLAT(vec, scal) \
    memset((vec).byte, scal, RC_VEC_SIZE)

/**
 *  Saturating addition.
 *  Computes dstv = MIN(srcv1 + srcv2, 0xff) for each 8-bit field.
 */
#undef  RC_VEC_ADDS
#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_adds_ref(srcv1, srcv2))

/**
 *  Average value, truncated.
 *  Computes dstv = (srcv1 + srcv2) >> 1 for each 8-bit field.
 */
#undef  RC_VEC_AVGT
#define RC_VEC_AVGT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_avgt_ref(srcv1, srcv2))

/**
 *  Average value, rounded.
 *  Computes dstv = (srcv1 + srcv2 + 1) >> 1 for each 8-bit field.
 */
#undef  RC_VEC_AVGR
#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_avgr_ref(srcv1, srcv2))

/**
 *  Average value, rounded towards srcv1.
 *  Computes dstv = (srcv1 + srcv2 + (srcv2 > srcv1)) >> 1
 *  for each 8-bit field.
 */
#undef  RC_VEC_AVGZ
#define RC_VEC_AVGZ(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_lerpz_ref(srcv1, srcv2, 0x80))

/**
 *  Saturating subtraction.
 *  Computes dstv = MAX(srvc1 - srcv2, 0) for each 8-bit field.
 */
#undef  RC_VEC_SUBS
#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_subs_ref(srcv1, srcv2))

/**
 *  Absolute-value subtraction.
 *  Computes dstv = ABS(srcv1 - srcv2) for each 8-bit field.
 */
#undef  RC_VEC_SUBA
#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_suba_ref(srcv1, srcv2))

/**
 *  Half subtraction with bias, truncated.
 *  Computes dstv = (srcv1 - srcv2 + 0xff) >> 1 for each 8-bit field.
 */
#undef  RC_VEC_SUBHT
#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_subht_ref(srcv1, srcv2))

/**
 *  Half subtraction with bias, rounded.
 *  Computes dstv = (srcv1 - srcv2 + 0x100) >> 1 for each 8-bit field.
 */
#undef  RC_VEC_SUBHR
#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_subhr_ref(srcv1, srcv2))

/**
 *  Absolute value.
 *  Computes dstv = 2*abs(srcv - 0x80) for each 8-bit field.
 *  The result is saturated to [0,0xff].
 */
#undef  RC_VEC_ABS
#define RC_VEC_ABS(dstv, srcv) \
    ((dstv) = rc_vec_abs_ref(srcv))

/**
 *  Comparison.
 *  Computes dstv = srcv1 > srcv2 ? 0xff : 0 for each 8-bit field.
 */
#undef  RC_VEC_CMPGT
#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_cmpgt_ref(srcv1, srcv2))

/**
 *  Comparison.
 *  Computes dst = srcv1 >= srcv2 ? 0xff : 0 for each 8-bit field.
 */
#undef  RC_VEC_CMPGE
#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_cmpge_ref(srcv1, srcv2))

/**
 *  Minimum value.
 *  Computes dstv = MIN(srcv1, srcv2) for each 8-bit field.
 */
#undef  RC_VEC_MIN
#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_min_ref(srcv1, srcv2))

/**
 *  Maximum value.
 *  Computes dstv = MAX(srcv1, srcv2) for each 8-bit field.
 */
#undef  RC_VEC_MAX
#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = rc_vec_max_ref(srcv1, srcv2))

/**
 *  Generate the blend vector needed by RC_VEC_LERP().
 *  No action.
 */
#undef  RC_VEC_BLEND
#define RC_VEC_BLEND(blendv, blend8) \
    (void)(blendv)

/**
 *  Linear interpolation.
 *  Computes dstv = srcv1 + ((blend8*(srcv2 - srcv1) + 0x80) >> 8) for each
 *  8-bit field. The Q.8 blend factor @e blend8 must b in the range [0,0x7f].
 */
#undef  RC_VEC_LERP
#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blend8v) \
    ((dstv) = rc_vec_lerp_ref(srcv1, srcv2, blend8))

/**
 *  Generate the blend vector needed by RC_VEC_LERPZ().
 *  No action.
 */
#undef  RC_VEC_BLENDZ
#define RC_VEC_BLENDZ(blendv, blend8) \
    (void)(blendv)

/**
 *  Linear interpolation rounded towards srcv1.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded towards zero. The Q.8 blend factor
 *  @e blend8 must b in the range [0,0x7f].
 */
#undef  RC_VEC_LERPZ
#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blend8v) \
    ((dstv) = rc_vec_lerpz_ref(srcv1, srcv2, blend8))

/**
 *  Generate the blend vector needed by RC_VEC_LERPN().
 *  No action.
 */
#undef  RC_VEC_BLENDN
#define RC_VEC_BLENDN(blendv, blend8) \
    (void)(blendv)

/**
 *  Linear interpolation rounded towards srcv2.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded away from zero. The Q.8 blend factor
 *  @e blend8 must b in the range [0,0x7f].
 */
#undef  RC_VEC_LERPN
#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blend8v) \
    ((dstv) = rc_vec_lerpz_ref(srcv2, srcv1, 0x100 - blend8))


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

/**
 *  Pack an 8-bit vector to a binary mask word.
 */
#undef  RC_VEC_GETMASKW
#define RC_VEC_GETMASKW(maskw, srcv) \
    ((maskw) = rc_vec_getmaskw_ref(srcv))

/**
 *  Pack an 8-bit vector to a binary mask vector.
 */
#undef  RC_VEC_GETMASKV
#define RC_VEC_GETMASKV(maskv, srcv) \
    ((maskv) = rc_vec_getmaskv_ref(srcv))


/*
 * -------------------------------------------------------------
 *  Reductions
 * -------------------------------------------------------------
 */

#ifndef RC_VEC_CNTN
#define RC_VEC_CNTN 0
#endif

/**
 *  Count bits across all fields, accumulation step.
 */
#undef  RC_VEC_CNTV
#define RC_VEC_CNTV(accv, srcv) \
    ((accv) = rc_vec_cntv_ref(accv, srcv))

/**
 *  Count bits across all fields, reduction step.
 */
#undef  RC_VEC_CNTR
#define RC_VEC_CNTR(sum, accv) \
    ((sum) = (accv).word)

#ifndef RC_VEC_SUMN
#define RC_VEC_SUMN 0
#endif

/**
 *  Sum across all fields, accumulation step.
 */
#undef  RC_VEC_SUMV
#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = rc_vec_sumv_ref(accv, srcv))

/**
 *  Sum across all fields, accumulation step.
 */
#undef  RC_VEC_SUMR
#define RC_VEC_SUMR(sum, accv) \
    ((sum) = (accv).word)

#ifndef RC_VEC_MACN
#define RC_VEC_MACN 0
#endif

/**
 *  Multiply and accumulate across all fields, accumulation step.
 */
#undef  RC_VEC_MACV
#define RC_VEC_MACV(accv, srcv1, srcv2) \
    ((accv) = rc_vec_macv_ref(accv, srcv1, srcv2))

/**
 *  Multiply and accumulate across all fields, reduction step.
 */
#undef  RC_VEC_MACR
#define RC_VEC_MACR(sum, accv) \
    ((sum) = (accv).word)


/*
 * -------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------
 */

rc_vec_ref_t
rc_vec_not_ref(rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_and_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_or_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_xor_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_shl_ref(rc_vec_ref_t srcv, int bytes);

rc_vec_ref_t
rc_vec_shr_ref(rc_vec_ref_t srcv, int bytes);

rc_vec_ref_t
rc_vec_align_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int bytes);

rc_vec_ref_t
rc_vec_pack_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_adds_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_avgt_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_avgr_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_subs_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_suba_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_subht_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_subhr_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_abs_ref(rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_cmpgt_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_cmpge_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_min_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_max_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

rc_vec_ref_t
rc_vec_lerp_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int blend8);

rc_vec_ref_t
rc_vec_lerpz_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int blend8);

unsigned
rc_vec_getmaskw_ref(rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_getmaskv_ref(rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_cntv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_sumv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv);

rc_vec_ref_t
rc_vec_macv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv1, rc_vec_ref_t srcv2);

#endif /* RC_VEC_REF_H */
