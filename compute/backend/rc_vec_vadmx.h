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
 *  @file   rc_vec_vadmx.h
 *  @brief  RAPP Compute layer vector operations
 *          using 64-bit VADMX instructions.
 */

#ifndef RC_VEC_VADMX_H
#define RC_VEC_VADMX_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

/* See the porting documentation for generic comments. */

typedef uint8_t rc_vec_t __attribute__ ((__vector_size__(8)));

#define RC_VEC_SIZE 8

/**
 *  No global declarations is needed for VADMX registers.
 */
#define RC_VEC_DECLARE()

/**
 *  No global clean up is needed for VADMX registers.
 */
#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = *(const rc_vec_t*)(ptr))

#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))

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

#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)            \
do {                                                          \
    rc_vec_t vec__;                                           \
    RC_VEC_LOAD(vec__, uptr);                                 \
    RC_VEC_OR(dstv, __builtin_ax_vsr(vec3,  vec1),            \
                    __builtin_ax_vsl(vec__, vec2));           \
    (vec3) = vec__;                                           \
} while (0)

#define RC_VEC_SHINIT(shv, bytes) \
    RC_VEC_SPLAT(shv, bytes)

#define RC_VEC_SHL(dstv, srcv, shv) \
    ((dstv) = __builtin_ax_vsr(srcv, shv))

#define RC_VEC_SHR(dstv, srcv, shv) \
    ((dstv) = __builtin_ax_vsl(srcv, shv))

#define RC_VEC_SHLC(dstv, srcv, bytes) \
    ((dstv) = __builtin_ax_vsri(srcv, bytes))

#define RC_VEC_SHRC(dstv, srcv, bytes) \
    ((dstv) = __builtin_ax_vsli(srcv, bytes))

#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes) \
    ((dstv) = __builtin_ax_valign(srcv1, srcv2, bytes))

#define RC_VEC_PACK(dstv, srcv1, srcv2)                   \
do {                                                      \
    rc_vec_t perm__ = (rc_vec_t){0, 2, 4, 6, 0, 0, 0, 0}; \
    rc_vec_t sv1__  = __builtin_ax_vshfl(srcv1, perm__); \
    rc_vec_t sv2__  = __builtin_ax_vshfl(srcv2, perm__); \
    (dstv) = __builtin_ax_vcomb(sv1__, sv2__, 4);         \
} while (0)

#define RC_VEC_ZERO(vec) \
    RC_VEC_SPLAT(vec, 0)

/**
 *  We could write these trivial operators as open code expressions
 *  (e.g. ((dstv) = ~(srcv)), but keeping them as builtins apparently
 *  has positive effects on scheduling and code size (at least for RAPP,
 *  possibly by chance), so let's be consistent, at least until insn
 *  combination works for vector types and we see an actual improvement.
 */
#define RC_VEC_NOT(dstv, srcv) \
   ((dstv) = __builtin_ax_vnot(srcv))

#define RC_VEC_AND(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vand(srcv1, srcv2))

#define RC_VEC_OR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vor(srcv1, srcv2))

#define RC_VEC_XOR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vxor(srcv1, srcv2))

/**
 *  In gcc-4.3.x (at least), combine doesn't happen for vector
 *  operations, so don't express these as open coded expressions, or
 *  we'll get two instructions.
 */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vandn(srcv1, srcv2))

#define RC_VEC_ORNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vorn(srcv1, srcv2))

#define RC_VEC_XORNOT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vxorn(srcv1, srcv2))

/**
 *  Don't express this as a constant vector until GCC generates the
 *  equivalent code for such vector constants.
 */
#define RC_VEC_SPLAT(vec, scal) \
    ((vec) = __builtin_ax_splat(scal))

#define RC_VEC_ABS(dstv, srcv) \
    ((dstv) = __builtin_ax_vabs(srcv))

#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vadds(srcv1, srcv2))

#define RC_VEC_AVGT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vavgt(srcv1, srcv2))

#define RC_VEC_AVGR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vavgr(srcv1, srcv2))

#define RC_VEC_AVGZ(dstv, srcv1, srcv2)            \
do {                                               \
    rc_vec_t blendv__;                             \
    RC_VEC_SPLAT(blendv__, 0x80);                  \
    RC_VEC_LERPZ(dstv, srcv1, srcv2, 0, blendv__); \
} while (0)

#define RC_VEC_SUBS(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubs(srcv1, srcv2))

#define RC_VEC_SUBA(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsuba(srcv1, srcv2))

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubht(srcv1, srcv2))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vsubhr(srcv1, srcv2))

#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vcmpgt(srcv1, srcv2))

#define RC_VEC_CMPGE(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vcmpge(srcv1, srcv2))

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vmin(srcv1, srcv2))

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    ((dstv) = __builtin_ax_vmax(srcv1, srcv2))

#define RC_VEC_BLEND(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                    \
    rc_vec_t lo__ = __builtin_ax_vzipl(srcv1, srcv2);   \
    rc_vec_t hi__ = __builtin_ax_vziph(srcv1, srcv2);   \
    lo__   = __builtin_ax_vlerp(lo__, blendv);          \
    hi__   = __builtin_ax_vlerp(hi__, blendv);          \
    (dstv) = __builtin_ax_vcomb(lo__, hi__, 4);         \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
    RC_VEC_SPLAT(blendv, 0x100 - (blend8))

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv) \
    RC_VEC_LERPN(dstv, srcv2, srcv1, 0, blendv)

#define RC_VEC_BLENDN(blendv, blend8) \
    RC_VEC_SPLAT(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv) \
do {                                                     \
    rc_vec_t lo__ = __builtin_ax_vzipl(srcv1, srcv2);    \
    rc_vec_t hi__ = __builtin_ax_vziph(srcv1, srcv2);    \
    lo__   = __builtin_ax_vlerpnz(lo__, blendv);         \
    hi__   = __builtin_ax_vlerpnz(hi__, blendv);         \
    (dstv) = __builtin_ax_vcomb(lo__, hi__, 4);          \
} while (0)

#define RC_VEC_GETMASKV(maskv, vec) \
    ((maskv) = __builtin_ax_rpack(vec))

#define RC_VEC_SETMASKV(vec, maskv)                             \
do {                                                            \
    rc_vec_t zero_ = (rc_vec_t){0, 0, 0, 0, 0, 0, 0, 0};        \
    rc_vec_t v_ = __builtin_ax_vaddx(zero_, maskv);             \
    rc_vec_t mask_ = (rc_vec_t){1<<0, 1<<1, 1<<2, 1<<3,         \
                                1<<4, 1<<5, 1<<6, 1<<7};        \
    rc_vec_t andv_ = __builtin_ax_vand(v_, mask_);              \
    (vec) = __builtin_ax_vcmpeq(andv_, mask_);                  \
} while (0)

#define RC_VEC_CNTN  28

/**
 *  In gcc-4.3.x (at least), __builtin_popcount doesn't take vector
 *  type arguments and return value.
 */
#define RC_VEC_CNTV(accv, srcv) \
    ((accv) = __builtin_ax_vadd(accv, __builtin_ax_vpcnt(srcv)))

#define RC_VEC_CNTR(cnt, accv)                 \
do {                                           \
    rc_vec_t sumv__ = __builtin_ax_rsum(accv); \
    RC_VEC_SUMR(cnt, sumv__);                  \
} while (0)

#define RC_VEC_SUMN 1

#define RC_VEC_SUMV(accv, srcv) \
    ((accv) = __builtin_ax_rsum(srcv))

#define RC_VEC_SUMR(sum, accv) \
    ((sum) = ((union { int i; rc_vec_t v; })(accv)).i)

#define RC_VEC_MACN 1

#define RC_VEC_MACV(accv, srcv1, srcv2) \
    ((accv) = __builtin_ax_rdot(srcv1, srcv2))

#define RC_VEC_MACR(mac, accv) \
    RC_VEC_SUMR(mac, accv)

#endif /* RC_VEC_VADMX_H */
