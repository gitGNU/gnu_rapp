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
 *  @file   rc_vec_neon.h
 *  @brief  RAPP Compute layer vector operations
 *          using the ARM NEON instruction set.
 */

#ifndef RC_VEC_NEON_H
#define RC_VEC_NEON_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <arm_neon.h>

/* Local support macros */

#define RC_SCAL_(x) \
    (((union { rc_vec_t v; uint64_t i; })(uint64_t)(x)).v)

/* Vector to scalar */
#define RC_TVEC_(t, x) \
    (((union {t v; int64_t i; })(t)(x)).i)

#define RC_ZERO_ RC_SCAL_(0)

/*
 *  Workaround for GCC bug.
 *  http://gcc.gnu.org/bugzilla/show_bug.cgi?id=48252
 */
#define RC_NEON_ZIP(dstv1, dstv2, srcv1, srcv2)         \
    __asm__("vzip.8 %P[out0], %P[out1]" :               \
            [out0] "=w" (dstv1), [out1] "=w" (dstv2) :  \
            "0" (srcv1), "1" (srcv2))

#define RC_NEON_UNZIP(dstv1, dstv2, srcv1, srcv2)       \
    __asm__("vuzp.8 %P[out0], %P[out1]" :               \
            [out0] "=w" (dstv1), [out1] "=w" (dstv2) :  \
            "0" (srcv1), "1" (srcv2))

/* See the porting documentation for generic comments. */

/**
 *  NEON has native instructions for all implemented hintable backend macros,
 *  so no hint-macros apply.
 */

/**
 *  For now the NEON registers are used in doubleword configuration.
 *  The backend can be made to use NEON registers in quadword (128)
 *  mode later.
 */
typedef uint8x8_t rc_vec_t;

#define RC_VEC_SIZE 8

#define RC_VEC_DECLARE()

#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = vld1_u8((const uint8_t *)(ptr)))

#define RC_VEC_STORE(ptr, vec) \
    vst1_u8((uint8_t *)(ptr), vec)

#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr) \
do {                                               \
    (void)(vec1);                                  \
    (void)(vec2);                                  \
    (void)(vec3);                                  \
    (uptr) = (ptr);                                \
} while (0)

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr) \
    ((dstv) = vld1_u8(uptr))

#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = (rc_vec_t)(vdup_n_s64(8 * (bytes))))

#define RC_VEC_SHL(dstv, srcv, shv)         \
do {                                        \
    int64x1_t shv_ = (int64x1_t)(shv);      \
    /* Negate shift vector */               \
    shv_ = vsub_s64(vdup_n_s64(0), shv_);   \
    RC_VEC_SHR(dstv, srcv, shv_);           \
} while (0)

#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = (rc_vec_t)vshl_u64((uint64x1_t)(srcv), (int64x1_t)(shv)))

/* NEON doesn't like zero shifts */
#define RC_VEC_SHLC(dstv, srcv, bytes)  \
    ((dstv) = (bytes == 0) ? (srcv) :   \
     ((rc_vec_t)vshr_n_u64((uint64x1_t)(srcv), 8 * (bytes))))

#define RC_VEC_SHRC(dstv, srcv, bytes)  \
    ((dstv) = (bytes == 0) ? (srcv) :   \
     ((rc_vec_t)vshl_n_u64((uint64x1_t)(srcv), 8 * (bytes))))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)    \
do {                                                \
    rc_vec_t v1_, v2_;                              \
    RC_VEC_SHLC(v1_, srcv1, bytes);                 \
    RC_VEC_SHRC(v2_, srcv2, RC_VEC_SIZE - (bytes)); \
    RC_VEC_OR(dstv, v1_, v2_);                      \
} while (0)

#define RC_VEC_PACK(dstv, srcv1, srcv2)         \
do {                                            \
    rc_vec_t dummy_;                            \
    RC_NEON_UNZIP(dstv, dummy_, srcv1, srcv2);  \
} while (0)

#define RC_VEC_ZERO(vec) \
    ((vec) = vdup_n_u8(0))

#define RC_VEC_NOT(dstv, srcv) \
    ((dstv) = vmvn_u8(srcv))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = vand_u8(srcv1, srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = vorr_u8(srcv1, srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = veor_u8(srcv1, srcv2))

#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = vbic_u8(srcv1, srcv2))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = vorn_u8(srcv1, srcv2))

#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
do {                                      \
    rc_vec_t not_;                        \
    RC_VEC_NOT(not_, srcv2);              \
    RC_VEC_XOR(dstv, srcv1, not_);        \
} while (0)

#define RC_VEC_SPLAT(vec, scal) \
    ((vec) = vdup_n_u8(scal))

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = vqadd_u8(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2) \
    ((dstv) = vhadd_u8(srcv1, srcv2))

#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = vrhadd_u8(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2)     \
do {                                        \
    rc_vec_t sv1_ = (srcv1);                \
    rc_vec_t sv2_ = (srcv2);                \
    rc_vec_t adj_1_, adj_xor_, adj_, avg_;  \
    rc_vec_t cmp_, adj_cmpx_;               \
    RC_VEC_SPLAT(adj_1_, 1);                \
    RC_VEC_CMPGT(cmp_, sv2_, sv1_);         \
    RC_VEC_XOR(adj_xor_, sv1_, sv2_);       \
    RC_VEC_AVGR(avg_, sv1_, sv2_);          \
    RC_VEC_AND(adj_cmpx_, adj_xor_, cmp_);  \
    RC_VEC_AND(adj_, adj_cmpx_, adj_1_);    \
    (dstv) = vsub_u8(avg_, adj_);           \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = vqsub_u8(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = vabd_u8(srcv1, srcv2))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2)    \
do {                                        \
    rc_vec_t notv2_;                        \
    RC_VEC_NOT(notv2_, srcv2);              \
    RC_VEC_AVGT(dstv, srcv1, notv2_);       \
} while (0)

#define RC_VEC_SUBHR(dstv, srcv1, srcv2)    \
do {                                        \
    rc_vec_t notv2_;                        \
    RC_VEC_NOT(notv2_, srcv2);              \
    RC_VEC_AVGR(dstv, srcv1, notv2_);       \
} while (0)

#define RC_VEC_ABS(dstv, srcv)              \
do {                                        \
    rc_vec_t bias_, abs1_;                  \
    RC_VEC_SPLAT(bias_, 0x80);              \
    RC_VEC_SUBA(abs1_, srcv, bias_);        \
    RC_VEC_ADDS(dstv, abs1_, abs1_);        \
} while (0)

#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = vcgt_u8(srcv1, srcv2))

#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
    ((dstv) = vcge_u8(srcv1, srcv2))

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = vmin_u8(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = vmax_u8(srcv1, srcv2))

#define RC_VEC_LERP_(dstv, srcv1, srcv2, blendv, biaslo, biashi)    \
do {                                                                \
    int16x4_t bv_ = (int16x4_t)blendv;                              \
    rc_vec_t sv1_ = (srcv1);                                        \
    uint8x8_t rs_, dummy_;                                          \
    int16x4_t lo1_, hi1_, lo2_, hi2_;                               \
    /* Expand to 16 bits. */                                        \
    RC_NEON_ZIP(lo1_, hi1_, sv1_, RC_ZERO_);                        \
    RC_NEON_ZIP(lo2_, hi2_, srcv2, RC_ZERO_);                       \
    /* Do (srcv1 - srcv2) as 16 bits. */                            \
    int16x4_t lod_ = vsub_s16(lo2_, lo1_);                          \
    int16x4_t hid_ = vsub_s16(hi2_, hi1_);                          \
    /* ... * blendv ... */                                          \
    int16x4_t lobld_ = vmul_s16(lod_, bv_);                         \
    int16x4_t hibld_ = vmul_s16(hid_, bv_);                         \
    /* ... + bias = (srcv1 - srcv2) * blendv + bias */              \
    int16x4_t los_ = vadd_s16(lobld_, biaslo);                      \
    int16x4_t his_ = vadd_s16(hibld_, biashi);                      \
    /* Reduce to 8 bits. */                                         \
    los_ = vshr_n_s16(los_, 8);                                     \
    his_ = vshr_n_s16(his_, 8);                                     \
    RC_NEON_UNZIP(rs_, dummy_, los_, his_);                         \
    /* Finally, add srcv1. */                                       \
    (dstv) = vadd_u8(rs_, sv1_);                                    \
} while (0)

#define RC_VEC_BLEND(blendv, blend8) \
    ((blendv) = (rc_vec_t)vdup_n_s16(blend8))

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)     \
do {                                                        \
    int16x4_t bias_ = vdup_n_s16(0x80);                     \
    RC_VEC_LERP_(dstv, srcv1, srcv2, blendv, bias_, bias_); \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_BLEND(blendv, blend8)

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)    \
do {                                                        \
    int16x4_t blo_, bhi_;                                   \
    rc_vec_t srcv1_ = (srcv1);                              \
    rc_vec_t srcv2_ = (srcv2);                              \
    rc_vec_t bias_;                                         \
    RC_VEC_CMPGT(bias_, srcv1_, srcv2_);                    \
    RC_NEON_ZIP(blo_, bhi_, bias_, RC_ZERO_);               \
    RC_VEC_LERP_(dstv, srcv1_, srcv2_, blendv, blo_, bhi_); \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8)   \
    RC_VEC_BLEND(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)    \
do {                                                        \
    int16x4_t blo_, bhi_;                                   \
    rc_vec_t srcv1_ = (srcv1);                              \
    rc_vec_t srcv2_ = (srcv2);                              \
    rc_vec_t bias_;                                         \
    RC_VEC_CMPGT(bias_, srcv2_, srcv1_);                    \
    RC_NEON_ZIP(blo_, bhi_, bias_, RC_ZERO_);               \
    RC_VEC_LERP_(dstv, srcv1_, srcv2_, blendv, blo_, bhi_); \
} while (0)

/**
 *  We hide this one as an internal macro, because for reasons to be
 *  investigated, its use fails in the main test-suite while the backend
 *  tests succeed.
 */
#define RC_VEC_GETMASKV_(maskv, vec)                    \
do {                                                    \
    int8x8_t shv_ = {0, -1, -2, -3, -4, -5, -6, -7};    \
    rc_vec_t tmp_ = vrev64_u8(vec);                     \
    tmp_ = vand_u8(tmp_, vdup_n_u8(0x80));              \
    tmp_ = vshl_u8(tmp_, shv_);                         \
    (maskv) = (rc_vec_t)                                \
        vpaddl_u32(vpaddl_u16(vpaddl_u8(tmp_)));        \
} while (0)

#define RC_VEC_GETMASKW(maskw, vec)         \
do {                                        \
    rc_vec_t maskv_;                        \
    RC_VEC_GETMASKV_(maskv_, vec);          \
    (maskw) = RC_TVEC_(rc_vec_t, maskv_);   \
} while (0)

#define RC_VEC_SETMASKV(vec, maskv)                      \
do {                                                     \
    rc_vec_t v_;                                         \
    rc_vec_t mask_ = (rc_vec_t){1<<0, 1<<1, 1<<2, 1<<3,  \
                                1<<4, 1<<5, 1<<6, 1<<7}; \
    uint8_t indx0_ = vget_lane_u8(maskv, 0);             \
    RC_VEC_SPLAT(v_, indx0_);                            \
    (vec) = vtst_u8(v_, mask_);                          \
} while (0)

#define RC_VEC_SUMN 128 /* floor(UINT16_MAX/510) = 128 */

#define RC_VEC_SUMV(accv, srcv)             \
do {                                        \
    uint16x4_t accv_ = (uint16x4_t)(accv);  \
    /* Parallel add and accumulate */       \
    accv_ = vpadal_u8(accv_, srcv);         \
    (accv) = (rc_vec_t)accv_;               \
} while (0)

#define RC_VEC_SUMR(sum, accv)              \
do {                                        \
    uint16x4_t accv_ = (uint16x4_t)(accv);  \
    uint64x1_t sumr_;                       \
    /* Parallel add */                      \
    sumr_ = vpaddl_u32(vpaddl_u16(accv_));  \
    /* Store in scalar */                   \
    (sum) = RC_TVEC_(uint64x1_t, sumr_);    \
} while (0)

/* floor(UINT16_MAX/16) - (floor(UINT16_MAX/16) % 4) = 4092 */
#define RC_VEC_CNTN 4092

#define RC_VEC_CNTV(accv, srcv) \
    RC_VEC_SUMV(accv, vcnt_u8(srcv))

#define RC_VEC_CNTR(cnt, accv) \
    RC_VEC_SUMR(cnt, accv)

#define RC_VEC_MACN 1024

#define RC_VEC_MACV(accv, srcv1, srcv2)         \
do {                                            \
    uint32x2_t accv_ = (uint32x2_t)(accv);      \
    uint16x8_t prod_;                           \
    uint64x2_t sum_;                            \
    /* Multiply and widen */                    \
    prod_ = vmull_u8(srcv1, srcv2);             \
    /* Parallel add the 16 bit vectors */       \
    sum_ = vpaddlq_u32(vpaddlq_u16(prod_));     \
    /* Add with accumulator */                  \
    accv_ = vadd_u32(accv_, vmovn_u64(sum_));   \
    (accv) = (rc_vec_t)accv_;                   \
} while (0)

#define RC_VEC_MACR(mac, accv)              \
do {                                        \
    uint32x2_t accv_ = (uint32x2_t)(accv);  \
    /* Parallel add and widen */            \
    uint64x1_t macr_ = vpaddl_u32(accv_);   \
    (mac) = RC_TVEC_(uint64x1_t, macr_);    \
} while (0)

#endif /* RC_VEC_NEON_H */
