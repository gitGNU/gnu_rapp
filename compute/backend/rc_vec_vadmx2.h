/*  Copyright (C) 2011-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_vadmx2.h
 *  @brief  RAPP Compute layer operations using 128-bit VADMX2 instructions
 *          through GCC builtins.
 */

#ifndef RC_VEC_VADMX2_H
#define RC_VEC_VADMX2_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif

/* No hints defined, all applicable ones should be equivalent. */

typedef uint8_t rc_vec_t __attribute__ ((__vector_size__(16)));
typedef int8_t rc_svec_type __attribute__ ((__vector_size__(16)));

/* One with 16-bit signed integers instead of unsigned bytes. */
typedef int16_t rc_vec16_type __attribute__((__vector_size__(16)));

/* And one with 32-bit signed integers instead of unsigned bytes. */
typedef int32_t rc_vec32_type __attribute__((__vector_size__(16)));

/**
 *  Until we have a version of GCC that supports subscripting vectors
 *  like arrays, we have to play games with asm to avoid writing the
 *  vector to stack when converting to and from an integer, as is what
 *  happens when we type-pun through a union.  FIXME: another way would
 *  be to through builtins: alternative ones for those where we now need
 *  an integer and have a vector, or a builtin just for conversion.
 *  Code generated for the version below usually comes together with a
 *  vector move for a separate register, but no memory traffic.
 */
#define RC_VEC_vec_to_32(x)                                             \
   (__extension__                                                       \
    ({ uint32_t x_; __asm__ (";# %1 -> %0" : "=C" (x_) : "0" (x)); x_;}))

/**
 *  When the source is in a general register, this'll get us two mtc2,
 *  with the high register set from $0.
 */
#define RC_VEC_32_to_vec(x) ((rc_vec_t)(rc_vec32_type) {(x), 0, 0, 0})

#define RC_VEC_SIZE 16

#define RC_VEC_DECLARE()

#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = *(const rc_vec_t*)(ptr))

#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))

#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                          \
do {                                                                        \
    const uint8_t *sptr__, *uptr__;                                         \
    uint32_t off__;                                                         \
    (void)(vec2);                                                           \
    sptr__ = (ptr);                              /* Cache source pointer */ \
    uptr__ = (const uint8_t*)((uintptr_t)sptr__ & ~7); /* Align down     */ \
    off__  = (uint32_t)sptr__ & 7;               /* Alignment offset     */ \
    (vec1) = RC_VEC_32_to_vec(off__);            /* Alignment byte shift */ \
    RC_VEC_LOAD(vec3, uptr__);                   /* Load first vector    */ \
    (uptr) = uptr__ + 16;                        /* Advance data pointer */ \
} while (0)

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)              \
do {                                                            \
    rc_vec_t vec__;                                             \
    RC_VEC_LOAD(vec__, uptr);                                   \
    (dstv) = (rc_vec_t)                                         \
             __builtin_axv_align_s8((rc_svec_type)(vec3),       \
                                    (rc_svec_type)vec__,        \
                                    RC_VEC_vec_to_32(vec1));    \
    (vec3) = vec__;                                             \
} while (0)


#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = RC_VEC_32_to_vec((uint32_t)(bytes)))

#define RC_VEC_SHL(dstv, srcv, shv)                             \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_sr_s8((rc_svec_type)(srcv),         \
                                  RC_VEC_vec_to_32(shv)))

#define RC_VEC_SHR(dstv, srcv, shv)                             \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_sl_s8((rc_svec_type)(srcv),         \
                                  RC_VEC_vec_to_32(shv)))

#define RC_VEC_SHLC(dstv, srcv, bytes)                          \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_sr_s8((rc_svec_type)(srcv), bytes))

#define RC_VEC_SHRC(dstv, srcv, bytes)                          \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_sl_s8((rc_svec_type)(srcv), bytes))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)                \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_align_s8((rc_svec_type)(srcv1),     \
                                     (rc_svec_type)(srcv2),     \
                                     bytes))

#define RC_VEC_PACK(dstv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_svec_type two_ = (rc_svec_type)RC_VEC_x_(2);                     \
    rc_svec_type szero_ = (rc_svec_type)RC_VEC_x_(0);                   \
    rc_svec_type sctr0_ = __builtin_axv_sctr_s8(szero_, szero_);        \
    rc_svec_type packshfl_ = sctr0_ * two_;                             \
    (dstv) = (rc_vec_t)                                                 \
             __builtin_axv_shfl_s8((rc_svec_type)(srcv1),               \
                                   (rc_svec_type)(srcv2), packshfl_);   \
} while (0)

#define RC_VEC_x_(x) \
    ((rc_vec_t) {(x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x), (x)})
#define RC_VEC16_x_(x) \
    ((rc_vec16_type) {(x), (x), (x), (x), (x), (x), (x), (x)})

#define RC_VEC_ZERO(vec) \
    RC_VEC_SPLAT(vec, 0)

#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = ~(srcv))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) & (srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) | (srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = (srcv1) ^ (srcv2))

/* Combination isn't done across vector operations.  FIXME: recheck. */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2)                       \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_andn_s8((rc_svec_type)(srcv1),      \
                                    (rc_svec_type)(srcv2)))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2)                        \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_orn_s8((rc_svec_type)(srcv1),       \
                                   (rc_svec_type)(srcv2)))

#define RC_VEC_XORNOT(dstv, srcv1, srcv2)                       \
    ((dstv) = (rc_vec_t)                                        \
              __builtin_axv_xorn_s8((rc_svec_type)(srcv1),      \
                                    (rc_svec_type)(srcv2)))

#define RC_VEC_SPLAT(vec, scal) \
    ((vec) = RC_VEC_x_(scal))

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_adds_u8(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2)         \
    ((dstv) = __builtin_axv_avgf_u8(srcv1, srcv2))

#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_avg_u8(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2)                         \
do {                                                            \
    rc_vec_t blendv_;                                           \
    RC_VEC_SPLAT(blendv_, 0x80);                                \
    (dstv) = __builtin_axv_blendz_u8(srcv2, srcv1, blendv_);    \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_subs_u8(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_suba_u8(srcv1, srcv2))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_subhf_u8(srcv1, srcv2))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_subhc_u8(srcv1, srcv2))

#define RC_VEC_ABS(dstv, srcv)                          \
do {                                                    \
    rc_vec_t x80_ = RC_VEC_x_(0x80);                    \
    rc_vec_t half_ = __builtin_axv_suba_u8(srcv, x80_); \
    (dstv) = __builtin_axv_adds_u8(half_, half_);       \
} while (0)

#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_cmpgt_u8(srcv1, srcv2))

#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_cmpge_u8(srcv1, srcv2))

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_min_u8(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_axv_max_u8(srcv1, srcv2))

#define RC_VEC_BLEND(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv) \
   ((dstv) = __builtin_axv_blend_u8(srcv2, srcv1, blendv))

/**
 *  We compute lerpX as s1 + ((s2 - s1) * blendv + bias) >> 8,
 *  with bias being the differing factor between X={N,Z}.
 *  Unfortunately, there's no working special instruction that's shorter
 *  than the vanilla SIMD implementation.
 *  Parameters except dstv and blendv are known to be temp variables, no
 *  need to wrap or copy to safeguard side effects or complex expressions.
 */
#define RC_VEC_LERPX_(dstv, s1, s2, blendv, bias, s1gts2)               \
do {                                                                    \
    /**                                                                 \
     *  The subtraction is computed as srcv2 - srcv1 in rc_vec_t        \
     *  (8-bit unsigned) representation with srcv1 > srcv2 ? 255 : 0 as \
     *  sign-extension to 16 bits "on the side", so we don't have to    \
     *  extend both operands to 16 bits before the subtraction.         \
     */                                                                 \
    rc_vec16_type bv_ = (rc_vec16_type)(blendv);                        \
    rc_svec_type szero_ = (rc_svec_type)RC_VEC_x_(0);                   \
    /* Do (s2 - s1) and merge with the sign-part to make it 16 bits. */ \
    rc_vec_t sub8_ = s2 - s1;                                           \
    rc_vec16_type sublo_ = (rc_vec16_type)                              \
                           __builtin_axv_zipl_s8((rc_svec_type)sub8_,   \
                                                 (rc_svec_type)s1gts2); \
    rc_vec16_type subhi_ = (rc_vec16_type)                              \
                           __builtin_axv_ziph_s8((rc_svec_type)sub8_,   \
                                                 (rc_svec_type)s1gts2); \
    /* ...* blendv... */                                                \
    rc_vec16_type mullo_ = sublo_ * bv_;                                \
    rc_vec16_type mulhi_ = subhi_ * bv_;                                \
    rc_vec16_type biaslo_ = (rc_vec16_type)                             \
                            __builtin_axv_zipl_s8((rc_svec_type)bias,   \
                                                  szero_);              \
    rc_vec16_type biashi_ = (rc_vec16_type)                             \
                            __builtin_axv_ziph_s8((rc_svec_type)bias,   \
                                                  szero_);              \
    /* ...+ bias... */                                                  \
    rc_vec16_type blendlo_ = mullo_ + biaslo_;                          \
    rc_vec16_type blendhi_ = mulhi_ + biashi_;                          \
    /* FIXME: check that the shuffle vector is moved outside loops. */  \
    rc_svec_type two_ = (rc_svec_type)RC_VEC_x_(2);                     \
    rc_svec_type one_ = (rc_svec_type)RC_VEC_x_(1);                     \
    rc_svec_type sctr0_ = __builtin_axv_sctr_s8(szero_, szero_);        \
    rc_svec_type packshfl_ = sctr0_ * two_ + one_;                      \
    /* ... >> 8 by shuffling ... */                                     \
    rc_vec_t blendres = (rc_vec_t)                                      \
                        __builtin_axv_shfl_s8((rc_svec_type)(blendlo_), \
                                              (rc_svec_type)(blendhi_), \
                                              packshfl_);               \
    /* Finally, the "s1 +" in s1 + ((s2 - s1) * blendv + bias) >> 8. */ \
    (dstv) = s1 + blendres;                                             \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    ((blendv) = (rc_vec_t)RC_VEC16_x_(blend8))

/* Compute using RC_LERPX_ with bias = srcv2 < srcv1 ? 255 : 0. */
#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)        \
do {                                                            \
    rc_vec_t bias_;                                             \
    rc_vec_t s1_ = (srcv1);                                     \
    rc_vec_t s2_ = (srcv2);                                     \
    RC_VEC_CMPGT(bias_, s1_, s2_);                              \
    RC_VEC_LERPX_(dstv, s1_, s2_, blendv, bias_, bias_);        \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
    ((blendv) = (rc_vec_t)RC_VEC16_x_(blend8))

/* Compute using RC_LERPX_ with bias = srcv1 < srcv2 ? 255 : 0. */
#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)        \
do {                                                            \
    rc_vec_t bias_, s1gts2_;                                    \
    rc_vec_t s1_ = (srcv1);                                     \
    rc_vec_t s2_ = (srcv2);                                     \
    RC_VEC_CMPGT(bias_, s2_, s1_);                              \
    RC_VEC_CMPGT(s1gts2_, s1_, s2_);                            \
    RC_VEC_LERPX_(dstv, s1_, s2_, blendv, bias_, s1gts2_);      \
} while (0)

#define RC_VEC_GETMASKV(maskv, vec) \
    ((maskv) = (rc_vec_t)__builtin_axv_packb_s8((rc_svec_type)(vec)))

#define RC_VEC_SETMASKV(vec, maskv) \
    ((vec) = (rc_vec_t)__builtin_axv_unpackb_s8((rc_svec_type)(maskv)))

/**
 *  The bit-count accumulator is just the rc_vec_t, so the max iteration
 *  count is floor(255/8) - (floor(255/8) % 4) == 28.
 */
#define RC_VEC_CNTN 28

#define RC_VEC_CNTV(accv, srcv) \
    ((accv) += (rc_vec_t)__builtin_axv_pcnt_s8((rc_svec_type)(srcv)))

#define RC_VEC_CNTR(cnt, accv)                          \
do {                                                    \
    rc_vec_t sumv_ = __builtin_axv_sum_u8(accv);        \
    (cnt) = RC_VEC_vec_to_32(sumv_);                    \
} while (0)

/* Floor (65535 / ((255 + 255) * 8)) = 16 */
#define RC_VEC_SUMN 16

#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = (rc_vec_t)__builtin_axv_racc_u8((rc_vec16_type)(accv), srcv))

#define RC_VEC_SUMR(sum, accv)                                          \
do {                                                                    \
    rc_vec_t sumv_ = (rc_vec_t)                                         \
                     __builtin_axv_sum_s16((rc_vec16_type)(accv));      \
    (sum) = RC_VEC_vec_to_32(sumv_);                                    \
} while (0)

#define RC_VEC_MACN 1024

#define RC_VEC_MACV(accv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_vec_t s1_ = (srcv1);                                             \
    rc_vec_t s2_ = (srcv2);                                             \
    rc_vec_t mlo_ = __builtin_axv_mul_u8(s1_, s2_);                     \
    rc_vec_t mhi_ = __builtin_axv_mulh_u8(s1_, s2_);                    \
    rc_vec32_type slo_ = (rc_vec32_type)__builtin_axv_sum_u8(mlo_);     \
    rc_vec32_type shi_ = (rc_vec32_type)__builtin_axv_sum_u8(mhi_);     \
    rc_vec32_type shis_ = (rc_vec32_type)                               \
                          __builtin_axv_sl_s8((rc_svec_type)(shi_), 4); \
    (accv) = (rc_vec_t)(slo_ + shis_ + (rc_vec32_type)(accv));          \
} while (0)

#define RC_VEC_MACR(mac, accv)                          \
do {                                                    \
    union { uint32_t i[4]; rc_vec_t v; } vi_;           \
    vi_.v = (accv);                                     \
    (mac) = vi_.i[0] + vi_.i[1]*256;                    \
} while (0)

#endif /* RC_VEC_VADMX2_H */
