/*  Copyright (C) 2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_loongson2ef.h
 *  @brief  RAPP Compute layer vector operations using 64-bit Loongson-2E
 *          and -2F instructions.
 */

#ifndef RC_VEC_LOONGSON_2EF_H
#define RC_VEC_LOONGSON_2EF_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

#include <loongson.h> /* Loongson-2E and -2F intrinsics */

/* For simplicity and brevity, a few local support macros. */

/**
 *  We don't use the "(rc_vec_t) (other_vector_type) {N}" syntax, as that
 *  yields worse code; constructing vectors on the stack, at least with
 *  the gcc-4.3 with backported Loongson-2E / -2F support where the port
 *  was tested.
 *
 *  We bit-copy a scalar into a vector; a simple one for the common
 *  rc_vec_t...
 */
#define RC_SCAL_(x) \
    (((union { rc_vec_t v; uint64_t i; })(uint64_t)(x)).v)

/* ...and one for other vector types needed for the implementation. */
#define RC_TSCAL_(t, x) \
    (((union { t v; uint64_t i; })(uint64_t)(x)).v)

/**
 *  See the porting documentation for generic comments.
 *
 *  For the machine specifics,
 *  <http://dev.lemote.com/files/resource/documents/
 *   Loongson/ls2f/Loongson2FUserGuide.pdf> was used.
 */

#define RC_VEC_HINT_CMPGE 1

#define RC_VEC_HINT_AVGR 1

typedef uint8x8_t rc_vec_t;

#define RC_VEC_SIZE 8

#define RC_ZERO_ RC_SCAL_(0)

#define RC_VEC_DECLARE()

#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = *(const rc_vec_t*)(ptr))

#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))

/* No port-specific support for unaligned load. */
#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                 \
do {                                                               \
    const uint8_t *sptr_, *uptr_;                                  \
    unsigned off_;                                                 \
    sptr_ = (ptr);                      /* Cache source pointer */ \
    uptr_ = (const uint8_t*)            /* Align down */           \
            ((uintptr_t)sptr_ & ~(RC_VEC_SIZE - 1));               \
    off_ = (uintptr_t)                  /* Alignment offset */     \
           sptr_ & (RC_VEC_SIZE - 1);                              \
    (vec1) = RC_SCAL_(8 * off_);        /* Alignment byte shift */ \
    (vec2) = RC_SCAL_(8 * (RC_VEC_SIZE - off_)); /* Complement */  \
    RC_VEC_LOAD(vec3, uptr_);           /* Load first vector */    \
    (uptr) = uptr_ + RC_VEC_SIZE;       /* Advance data pointer */ \
} while (0)

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)      \
do {                                                    \
    rc_vec_t vec_, sh1_, sh2_;                          \
    RC_VEC_LOAD(vec_, uptr);                            \
    RC_VEC_SHL(sh1_, vec3, vec1);                       \
    RC_VEC_SHR(sh2_, vec_, vec2);                       \
    RC_VEC_OR(dstv, sh1_, sh2_);                        \
    (vec3) = vec_;                                      \
} while (0)

#define RC_VEC_SHINIT(shv, bytes) \
    ((shv) = RC_SCAL_(8 * (bytes)))

/**
 *  Unfortunately, the builtin support for loongson SIMD in gcc is
 *  lacking (at least including gcc-4.6.0). For example, trivial logical
 *  operators don't exist, neither as builtins nor operators on the
 *  appropriate vector-type: you'll get code using general registers and
 *  regular insns.
 */
#define RC_VEC_SHL(dstv, srcv, shv)             \
    __asm__("dsrl %[dst],%[src],%[cnt]" :       \
            [dst] "=f" (dstv) :                 \
            [src] "f" (srcv), [cnt] "f" (shv))

/* Beware that "the logical right" is the left here. as in most ISA's. */
#define RC_VEC_SHR(dstv, srcv, shv)             \
    __asm__("dsll %[dst],%[src],%[cnt]" :       \
            [dst] "=f" (dstv) :                 \
            [src] "f" (srcv), [cnt] "f" (shv))

#define RC_VEC_SHLC(dstv, srcv, bytes) \
    RC_VEC_SHL(dstv, srcv, RC_SCAL_(8 * (bytes)))

#define RC_VEC_SHRC(dstv, srcv, bytes) \
    RC_VEC_SHR(dstv, srcv, RC_SCAL_(8 * (bytes)))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)        \
do {                                                    \
    rc_vec_t sv1_, sv2_;                                \
    RC_VEC_SHLC(sv1_, srcv1, bytes);                    \
    RC_VEC_SHRC(sv2_, srcv2, RC_VEC_SIZE - (bytes));    \
    RC_VEC_OR(dstv, sv1_, sv2_);                        \
} while (0)

#define RC_VEC_PACK(dstv, srcv1, srcv2)         \
do {                                            \
    rc_vec_t sv1p_, sv2p_, mask_;               \
    RC_SPLAT16B_(mask_, 0xff);                  \
    RC_VEC_AND(sv1p_, srcv1, mask_);            \
    RC_VEC_AND(sv2p_, srcv2, mask_);            \
    (dstv) = packushb((uint16x4_t)sv1p_,        \
                      (uint16x4_t)sv2p_);       \
} while (0)

#define RC_VEC_ZERO(vec) \
    ((vec) = RC_ZERO_)

#define RC_VEC_NOT(dstv, srcv)                          \
    __asm__("nor %[dst],%[op1],%[op1]" :                \
            [dst] "=f" (dstv) : [op1] "f" (srcv))

#define RC_VEC_AND(dstv, srcv1, srcv2)                  \
    __asm__("and %[dst],%[op1],%[op2]" :                \
            [dst] "=f" (dstv) :                         \
            [op1] "f" (srcv1), [op2] "f" (srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2)                   \
    __asm__("or %[dst],%[op1],%[op2]" :                 \
            [dst] "=f" (dstv) :                         \
            [op1] "f" (srcv1), [op2] "f" (srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2)                  \
    __asm__("xor %[dst],%[op1],%[op2]" :                \
            [dst] "=f" (dstv) :                         \
            [op1] "f" (srcv1), [op2] "f" (srcv2))

#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = pandn_ub(srcv2, srcv1))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
do {                                     \
    rc_vec_t inv_;                       \
    RC_VEC_NOT(inv_, srcv2);             \
    RC_VEC_OR(dstv, srcv1, inv_);        \
} while (0)

#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
do {                                      \
    rc_vec_t inv_;                        \
    RC_VEC_NOT(inv_ , srcv2);             \
    RC_VEC_XOR(dstv, srcv1, inv_);        \
} while (0)

#define RC_SPLAT16B_(vec, scal)                                         \
    ((vec) = (rc_vec_t)pshufh_u(RC_TSCAL_(uint16x4_t, 0),               \
                                RC_TSCAL_(uint16x4_t, scal), 0))

#define RC_VEC_SPLAT(vec, scal) \
    RC_SPLAT16B_(vec, (scal) * 256 + (scal))

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = paddusb(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2)        \
do {                                           \
    rc_vec_t sv1_ = (srcv1);                   \
    rc_vec_t sv2_ = (srcv2);                   \
    rc_vec_t adj_1_, adj_xor_, adj_, avg_;     \
    RC_VEC_SPLAT(adj_1_, 1);                   \
    RC_VEC_XOR(adj_xor_, sv1_, sv2_);          \
    RC_VEC_AVGR(avg_, sv1_, sv2_);             \
    RC_VEC_AND(adj_, adj_xor_, adj_1_);        \
    (dstv) = psubb_u(avg_, adj_);              \
} while (0)

#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = pavgb(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2)        \
do {                                           \
    rc_vec_t sv1_ = (srcv1);                   \
    rc_vec_t sv2_ = (srcv2);                   \
    rc_vec_t adj_1_, adj_xor_, adj_, avg_;     \
    rc_vec_t le_, adj_cmpx_;                   \
    RC_VEC_SPLAT(adj_1_, 1);                   \
    RC_VEC_CMPLE_(le_, sv2_, sv1_);            \
    RC_VEC_XOR(adj_xor_, sv1_, sv2_);          \
    RC_VEC_AVGR(avg_, sv1_, sv2_);             \
    RC_VEC_ANDNOT(adj_cmpx_, adj_xor_, le_);   \
    RC_VEC_AND(adj_, adj_cmpx_, adj_1_);       \
    (dstv) = psubb_u(avg_, adj_);              \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = psubusb(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = pasubub(srcv1, srcv2))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2)        \
do {                                            \
    rc_vec_t notv2_;                            \
    RC_VEC_NOT(notv2_, srcv2);                  \
    RC_VEC_AVGT(dstv, srcv1, notv2_);           \
} while (0)

#define RC_VEC_SUBHR(dstv, srcv1, srcv2)        \
do {                                            \
    rc_vec_t notv2_;                            \
    RC_VEC_NOT(notv2_, srcv2);                  \
    RC_VEC_AVGR(dstv, srcv1, notv2_);           \
} while (0)

#define RC_VEC_ABS(dstv, srcv)                  \
do {                                            \
    rc_vec_t bias_, abs1_;                      \
    RC_VEC_SPLAT(bias_, 0x80);                  \
    RC_VEC_SUBA(abs1_, srcv, bias_);            \
    RC_VEC_ADDS(dstv, abs1_, abs1_);            \
} while (0)

/**
 *  We can only use the byte-sized equality operator (not e.g. pcmpgtb),
 *  as the byte-sized compares works on *signed* bytes.
 *  See also <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=48348>.
 */

#define RC_VEC_CMPGT(dstv, srcv1, srcv2)        \
do {                                            \
    rc_vec_t le_;                               \
    RC_VEC_CMPLE_(le_, srcv1, srcv2);           \
    RC_VEC_NOT(dstv, le_);                      \
} while (0)

#define RC_VEC_CMPLE_(dstv, srcv1, srcv2)       \
do {                                            \
    rc_vec_t cmpsv1_ = (srcv1);                 \
    rc_vec_t minsv_;                            \
    RC_VEC_MIN(minsv_, cmpsv1_, srcv2);         \
    RC_VEC_CMPEQ_(dstv, cmpsv1_, minsv_);       \
} while (0)

#define RC_VEC_CMPGE(dstv, srcv1, srcv2)        \
do {                                            \
    rc_vec_t sv1_ = (srcv1);                    \
    rc_vec_t maxsv_;                            \
    RC_VEC_MAX(maxsv_, sv1_, srcv2);            \
    RC_VEC_CMPEQ_(dstv, sv1_, maxsv_);          \
} while (0)

#define RC_VEC_CMPEQ_(dstv, srcv1, srcv2)       \
    ((dstv) = pcmpeqb_u(srcv1, srcv2))

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = pminub(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = pmaxub(srcv1, srcv2))

#define RC_VEC_BLEND(blendv, blend8) \
    RC_SPLAT16B_(blendv, (blend8))

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                    \
    rc_vec_t bias_;                                     \
    RC_SPLAT16B_(bias_, 0x80);                          \
    RC_VEC_LERP_(dstv, srcv1, srcv2, blendv,            \
                 (int16x4_t)bias_, (int16x4_t)bias_);   \
} while (0)

#define RC_VEC_LERP_(dstv, srcv1, srcv2, blendv, biaslo, biashi)        \
do {                                                                    \
    int16x4_t bv__ = (int16x4_t)(blendv);                               \
    /* Expand to 16 bits. */                                            \
    rc_vec_t sv1__ = (srcv1);                                           \
    int16x4_t lo1__ = (int16x4_t)punpcklbh_u(sv1__, RC_ZERO_);          \
    int16x4_t hi1__ = (int16x4_t)punpckhbh_u(sv1__, RC_ZERO_);          \
    int16x4_t lo2__ = (int16x4_t)punpcklbh_u(srcv2, RC_ZERO_);          \
    int16x4_t hi2__ = (int16x4_t)punpckhbh_u(srcv2, RC_ZERO_);          \
    /* Do (srcv1 - srcv2) as 16 bits. */                                \
    int16x4_t lod__ = psubh_s(lo2__, lo1__);                            \
    int16x4_t hid__ = psubh_s(hi2__, hi1__);                            \
    /* ... * blendv ... */                                              \
    int16x4_t lobld__ = pmullh(lod__, bv__);                            \
    int16x4_t hibld__ = pmullh(hid__, bv__);                            \
    /* ... + bias = (srcv1 - srcv2) * blendv + bias */                  \
    int16x4_t los__ = paddh_s(lobld__, biaslo);                         \
    int16x4_t his__ = paddh_s(hibld__, biashi);                         \
    /* Reduce to 8 bits. */                                             \
    uint16x4_t slos__ = psrlh_u((uint16x4_t)los__, 8);                  \
    uint16x4_t shis__ = psrlh_u((uint16x4_t)his__, 8);                  \
    rc_vec_t rs__ = packushb(slos__, shis__);                           \
    /* Finally, add srcv1. */                                           \
    (dstv) = paddb_u(rs__, sv1__);                                      \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
     RC_VEC_BLEND(blendv, blend8)

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)        \
do {                                                            \
    rc_vec_t srcv1_ = (srcv1);                                  \
    rc_vec_t srcv2_ = (srcv2);                                  \
    rc_vec_t max_, bias_;                                       \
    RC_VEC_MAX(max_, srcv1_, srcv2_);                           \
    RC_VEC_CMPEQ_(bias_, srcv1_, max_);                         \
    int16x4_t blo_ = (int16x4_t)punpcklbh_u(bias_, RC_ZERO_);   \
    int16x4_t bhi_ = (int16x4_t)punpckhbh_u(bias_, RC_ZERO_);   \
    RC_VEC_LERP_(dstv, srcv1_, srcv2_, blendv, blo_, bhi_);     \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_BLENDZ(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)        \
do {                                                            \
    rc_vec_t srcv1_ = (srcv1);                                  \
    rc_vec_t srcv2_ = (srcv2);                                  \
    rc_vec_t max_, bias_;                                       \
    RC_VEC_MAX(max_, srcv1_, srcv2_);                           \
    RC_VEC_CMPEQ_(bias_, srcv2_, max_);                         \
    int16x4_t blo_ = (int16x4_t)punpcklbh_u(bias_, RC_ZERO_);   \
    int16x4_t bhi_ = (int16x4_t)punpckhbh_u(bias_, RC_ZERO_);   \
    RC_VEC_LERP_(dstv, srcv1_, srcv2_, blendv, blo_, bhi_);     \
} while (0)

#define RC_VEC_GETMASKV(maskv, vec) \
  ((maskv) = pmovmskb_u(vec))

/**
 *  The bit-count accumulator is just the rc_vec_t, so the max iteration
 *  count is floor(255/8) - (floor(255/8) % 4) == 28.
 */
#define RC_VEC_CNTN 28 /* floor(255/8) - (floor(255/8) % 4) = 28 */

#define RC_VEC_CNTV(accv, srcv)                                         \
do {                                                                    \
    rc_vec_t c1_;                                                       \
    rc_vec_t c2_;                                                       \
    rc_vec_t c4_;                                                       \
    rc_vec_t acc_ = (srcv);                                             \
    rc_vec_t tmp_;                                                      \
    RC_VEC_SPLAT(c1_, 0x55);                                            \
    RC_VEC_SPLAT(c2_, 0x33);                                            \
    RC_VEC_SPLAT(c4_, 0x0f);                                            \
    RC_VEC_SHL(tmp_, acc_, RC_SCAL_(1));   /* tmp  = acc >> 1  */       \
    RC_VEC_AND(tmp_, tmp_, c1_);           /* tmp &= 0x55...   */       \
    RC_VEC_AND(acc_, acc_, c1_);           /* acc &= 0x55...   */       \
    acc_ = paddb_u(acc_, tmp_);            /* acc += tmp       */       \
    RC_VEC_SHL(tmp_, acc_, RC_SCAL_(2));   /* tmp  = acc >> 2  */       \
    RC_VEC_AND(tmp_, tmp_, c2_);           /* tmp &= 0x33...   */       \
    RC_VEC_AND(acc_, acc_, c2_);           /* acc &= 0x33...   */       \
    acc_ = paddb_u(acc_, tmp_);            /* acc += tmp       */       \
    RC_VEC_SHL(tmp_, acc_, RC_SCAL_(4));   /* tmp  = acc >> 4  */       \
    acc_ = paddb_u(acc_, tmp_);            /* acc += tmp       */       \
    RC_VEC_AND(acc_, acc_, c4_);           /* acc &= 0x0f...   */       \
    (accv) = paddusb(accv, acc_);                                       \
} while (0)

#define RC_VEC_CNTR(cnt, accv) \
    RC_VEC_SUMR(cnt, accv)

/* There's no actual accumulator step, we just add together the bytes. */
#define RC_VEC_SUMN 1

/* With RC_VEC_SUMN == 1, there is no accumulation, just an assignment. */
#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = (srcv))

/**
 *  The biadd insn adds together the unsigned bytes forming a 16-bit
 *  result. It sets the whole 64 bits (clearing the high 48 bits)
 *  but getting the low 32 bits requires fewer insns (than 64 or 16
 *  bits).
 */
#define RC_VEC_SUMR(cnt, accv) \
 ((cnt) = ((union { uint32_t i[2]; uint16x4_t v; })(biadd(accv))).i[0])

/**
 *  The "MAC" accumulator is the 2*int32_t vector type given by the
 *  pmaddhw insn, so the max value is actually 16512, but that's
 *  untestable.
 */
#define RC_VEC_MACN 1024

#define RC_VEC_MACV(accv, srcv1, srcv2)                         \
do {                                                            \
    rc_vec_t sv1_ = (srcv1);                                    \
    rc_vec_t sv2_ = (srcv2);                                    \
    int16x4_t lo1_ = (int16x4_t)punpcklbh_u(sv1_, RC_ZERO_);    \
    int16x4_t hi1_ = (int16x4_t)punpckhbh_u(sv1_, RC_ZERO_);    \
    int16x4_t lo2_ = (int16x4_t)punpcklbh_u(sv2_, RC_ZERO_);    \
    int16x4_t hi2_ = (int16x4_t)punpckhbh_u(sv2_, RC_ZERO_);    \
    int32x2_t lom_ = pmaddhw(lo1_, lo2_);                       \
    int32x2_t him_ = pmaddhw(hi1_, hi2_);                       \
    int32x2_t m_ = paddw_s(him_, lom_);                         \
    (accv) = (rc_vec_t)paddw_s((int32x2_t)(accv), m_);          \
} while (0)

#define RC_VEC_MACR(mac, accv)                                  \
do {                                                            \
    int32x2_t res_;                                             \
    int32x2_t accv_ = (int32x2_t)(accv);                        \
    rc_vec_t accv_sh_;                                          \
    RC_VEC_SHLC(accv_sh_, (rc_vec_t)accv_, RC_VEC_SIZE / 2);    \
    res_ = paddw_s(accv_, (int32x2_t)accv_sh_);                 \
    (mac) = ((union { uint32_t i[2]; int32x2_t v; })res_).i[0]; \
} while (0)

#endif  /* RC_VEC_LOONGSON_2EF_H */
