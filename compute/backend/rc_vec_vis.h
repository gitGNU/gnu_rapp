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
 *  @file   rc_vec_vis.h
 *  @brief  RAPP Compute layer operations using 64-bit VIS instructions
 *          through GCC builtins.
 */

#ifndef RC_VEC_VIS_H
#define RC_VEC_VIS_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif

/**
 *  The code could probably be adjusted to handle both endiannesses, but
 *  I can't test that, and I can't perceive any actual interest.
 */
#ifndef WORDS_BIGENDIAN
#error "This file supports only big-endian SPARC"
#endif

/**
 *  This file was written using documentation gleaned from the GCC SPARC
 *  back-end and from web searches on each individual useful-looking INSN
 *  as "INSN vis". The most information was found in
 *  <http://www.fujitsu.com/downloads/PRMPWR/JPS1-R1.0.4-Common-pub.pdf> and
 *  <http://www.weblearn.hs-bremen.de/risse/RST/WS06/x86_SUN/Dokumentation/
 *   SPARC/UltraSPARC%20IIi%20User%27s%20Manual%20.pdf>. For the record,
 *  the supposedly official SPARC V9 instruction set manual was consulted,
 *  at time of this writing found at
 *  <http://download.oracle.com/docs/cd/E19963-01/pdf/821-1607.pdf> (VIS
 *  instructions are listed in chapter E.6) but unfortunately, it isn't
 *  sufficient, for example semantics are not described in appropriate
 *  detail.
 */

/* For simplicity and brevity, a few local support macros. */

/**
 *  Bit-copying a scalar into a vector; a simple macro for the common
 *  rc_vec_t. This is not as profitable for VIS as for many other
 *  architectures, because moving from/to regular registers and the VIS
 *  registers (the floating point registers) can't be done directly; it
 *  requires intermediate memory. Thus forming scalars from regular
 *  operations is usually not a win, at least not for scalars which can
 *  be loaded from a constant pool.
 */
#define RC_SCAL_VEC_(x) \
    (((union { rc_vec_t v; uint64_t i; })(uint64_t)(x)).v)
#define RC_VEC_SCAL_(x) \
    (((union { rc_vec_t v; uint64_t i; })(uint64_t)(x)).i)
#define RC_SCAL_TVEC_(t, x) \
    (((union { t v; uint64_t i; })(uint64_t)(x)).v)
#define RC_SCAL32_TVEC_(t, x) \
    (((union { t v; uint32_t i; })(uint32_t)(x)).v)
#define RC_TVEC_SCAL32_(t, x) \
    (((union { t v; uint32_t i; })(uint32_t)(x)).i)

/* Bit-copying to/from vectors of different lengths than rc_vec_t. */
#define RC_VEC_TVEC_(t, x) \
    (((union { t v; rc_vec_t vt; })(x)).v)
#define RC_TVEC_VEC_(t, x) \
    (((union { rc_vec_t v; t tv; })(x)).v)

/**
 *  This one could be done as a cast, as we're forced to keep it the
 *  same size as rc_vec_t. Keeping it like this in case we will ever go
 *  back to the proper size definition.
 */
#define RC_VEC_SVEC_(x) \
    (((union { rc_svec_type_ v; rc_vec_t vt; })(x)).v)

/* Best way to get (0, 0, ..., 0) for rc_vec_type. */
#define RC_ZERO_ ((rc_vec_t) {0})

/**
 *  We need to set GSR.scale_factor at times. This is a hidden data
 *  dependency from __builtin_vis_faligndata and __builtin_vis_fpack16
 *  on the GSR register (for the former also set by a previous
 *  __builtin_vis_alignaddr). We must coerce GCC to keep this asm, which
 *  in its trivial form (assuming we combine rc_gsr_align_ and
 *  rc_gsr_scale_ in one) would appear to only read and modify an unused
 *  variable.
 *  We coerce GCC by faking the use of the value on one operand for the
 *  dependent builtin and the use of a dependent output together with
 *  the set GSR. For vector operands, apply RC_GSR_ALIGN_ or
 *  RC_GSR_SCALE_ on a temporary variable used as input to the builtin,
 *  and apply RC_GSR_DEP_RESULT_ on the result. The former will force
 *  GSR to be set to the right value before the builtin is executed, and
 *  the latter will stop subsequent (different) settings from being
 *  scheduled between the GSR-setting and the builtin.
 *  We use separate variables instead of the more obvious struct, so
 *  either field data (or both), if unused, can be eliminated.
 *  Do not apply the volatile specifier to this asm, as that would
 *  affect scheduling of other code across the asm. We rely heavily on
 *  GCC to recognize all settings as constant, so you'll not actually
 *  see any variables, just GSR set to constant values.
 *  Using "brm" as a constraint for the fake-depended variable so it
 *  can live (temporarily) anywhere.
 *  This macro is only applied on identifiers and scalars, so we don't
 *  need to protect the arguments from multiple evaluations.
 */
#define RC_GSR_ALIGN_SCALE_(al, sc, dep)                                \
do {                                                                    \
    /* Make sure we're mutually exclusive with RC_VEC_LDINIT. */        \
    if (rc_gsr_ldinit_) {                                               \
        extern void rc_mixing_GSR_setting_with_RC_VEC_LDINIT_(void);    \
        rc_mixing_GSR_setting_with_RC_VEC_LDINIT_();                    \
    }                                                                   \
                                                                        \
    /**                                                                 \
     *  If any of the arguments is non-constant, don't bother           \
     *  checking if they have changed from the last writing,            \
     *  just do the GSR write.                                          \
     *  Don't check anything at the first write.                        \
     */                                                                 \
    if (!__builtin_constant_p(al) ||                                    \
        !__builtin_constant_p(sc) ||                                    \
        !rc_gsr_set_ ||                                                 \
        (unsigned) (al) != rc_gsr_align_ ||                             \
        (unsigned) (sc) != rc_gsr_scale_)                               \
    {                                                                   \
        rc_gsr_set_ = 1;                                                \
        rc_gsr_align_ = (al);                                           \
        rc_gsr_scale_ = (sc);                                           \
        unsigned int val_ = (rc_gsr_scale_ << 3) | rc_gsr_align_;       \
                                                                        \
        if (__builtin_constant_p (val_)) {                              \
            __asm__("wr %%g0,%[gsrval],%%gsr\n"                         \
                    ";# dep %[depvec] on fake GSR %[fakegsr]" :         \
                    [depvec] "=brm" (dep),                              \
                    [fakegsr] "=rm" (rc_gsr_fakedep_) :                 \
                    "0" (dep),                                          \
                    [gsrval] "i" (val_),                                \
                    "1" (rc_gsr_fakedep_));                             \
        }                                                               \
        else {                                                          \
            __asm__("wr %[gsrval],0,%%gsr"                              \
                    "\n;# dep %[depvec] on fake GSR %[fakegsr]" :       \
                    [depvec] "=brm" (dep),                              \
                    [fakegsr] "=rm" (rc_gsr_fakedep_) :                 \
                    "0" (dep),                                          \
                    [gsrval] "r" (val_),                                \
                    "1" (rc_gsr_fakedep_));                             \
        }                                                               \
    }                                                                   \
    else {                                                              \
        /* If nothing changes, we still need to mark the dependency. */ \
        __asm__("\n;# dep %[depvec] on fake GSR %[fakegsr]" :           \
                [depvec] "=brm" (dep) :                                 \
                "0" (dep),                                              \
                [fakegsr] "g" (rc_gsr_fakedep_));                       \
    }                                                                   \
} while (0)

/**
 *  We need to make the faked GSR dependent on the result of an
 *  operation (not just the operands), or else if may be changed between
 *  the marked dependence of the incoming operand on the faked GSR and
 *  the operation. FIXME: this is very brittle. What if GCC would copy
 *  (duplicate) the faked GSR? Nothing magic about it as GCC sees it,
 *  not like it's a hardware control register or anything.
 */
#define RC_GSR_DEP_RESULT_(x)                                   \
    __asm__("\n;# dep fake GSR %[fakegsr] on %[xdep]" :         \
            [fakegsr] "=brm" (rc_gsr_fakedep_) :                \
            [xdep] "brm" (x), "0" (rc_gsr_fakedep_))

#define RC_GSR_ALIGN_(x, dep) \
    RC_GSR_ALIGN_SCALE_(x, rc_gsr_scale_, dep)

#define RC_GSR_SCALE_(x, dep) \
    RC_GSR_ALIGN_SCALE_(rc_gsr_align_, x, dep)

/* RC_VEC_CMPGE is defined in terms of RC_VEC_CMPGT. */
#define RC_VEC_HINT_CMPGT

/**
 *  RC_VEC_AVGR is slightly more complicated than RC_VEC_AVGT, the
 *  former having a corrective term.
 */
#define RC_VEC_HINT_AVGT

typedef unsigned char rc_vec_t __attribute__((__vector_size__(8)));

/* We need a few helper types. */

/* One with 16-bit signed integers, "shorts", instead of unsigned bytes. */
typedef short rc_svec_type_ __attribute__((__vector_size__(8)));

/**
 *  We give the smaller types the size of 8 bytes instead of the correct 4,
 *  because of GCC-related problems involving conversion between
 *  different-sized types (incorrect assembly from some versions,
 *  internal compiler errors, unnecessary intermediates on stack when
 *  splitting or joining the larger type into/from the smaller types).
 *  By pretending that all types have the "full" size (but limiting to a
 *  subset of valid registers for the smaller types), we work around
 *  that effectively, at least for gcc-4.3.2 and spot-checked with
 *  gcc-4.6.0-prerelease. As a consequence, we can't use the builtins
 *  that require the smaller types. FIXME: On the plus side, we could
 *  mix-in the GSR setting in the asms. Not done until it's known whether
 *  that'd introduce stall cycles.
 */
typedef unsigned char rc_vec4_type_ __attribute__((__vector_size__(4)));
typedef short rc_svec2_type_ __attribute__((__vector_size__(4)));

/**
 *  We need to concatenate two smaller vectors to one larger, forming
 *  ABCDEFGH from ABCD and EFGH.  This also does the right concatenation
 *  of vector (floating-point) registers.
 */
#define RC_CONCAT_(abcdefgh, abcd, efgh)                \
do {                                                    \
   typedef union {                                      \
       struct { rc_vec4_type_ hi_, lo_; } hilo_;        \
       rc_vec_t v_;                                     \
   } RC_hl_type_;                                       \
   RC_hl_type_ hl_ = (RC_hl_type_) {{(abcd), (efgh)}};  \
   (abcdefgh) = hl_.v_;                                 \
} while (0)

#define RC_CONCATLOW_ RC_CONCAT_

/* The inverse of RC_CONCAT_. */
#define RC_SPLIT_(abcdefgh, abcd, efgh)                 \
do {                                                    \
   typedef union {                                      \
       rc_vec_t v_;                                     \
       struct { rc_vec4_type_ hi_, lo_; } hilo_;        \
   } RC_hl_type_;                                       \
   RC_hl_type_ hl_ = (RC_hl_type_) (abcdefgh);          \
   (abcd) = hl_.hilo_.hi_;                              \
   (efgh) = hl_.hilo_.lo_;                              \
} while (0)

#define RC_SPLITLOW_ RC_SPLIT_

#define RC_VEC_SIZE 8

/**
 *  These are used to keep state and fake-in the otherwise implicit
 *  dependencies on the GSR.scale_factor and GSR.alignaddr_offset
 *  fields for the fpack16 instruction and aligndata builtin.
 */
#define RC_VEC_DECLARE()                                                \
    /* Current GSR.scale_factor. */                                     \
    unsigned int rc_gsr_scale_ __attribute__ ((__unused__)) = 7;        \
    /* Current GSR.alignaddr_offset. */                                 \
    unsigned int rc_gsr_align_ __attribute__ ((__unused__)) = 4;        \
    /* Set to 1 after first actual GSR setting is emitted. */           \
    unsigned int rc_gsr_set_ __attribute__ ((__unused__)) = 0;          \
    /* The variable we use to track dependencies for GSR. */            \
    register unsigned int rc_gsr_fakedep_                               \
        __attribute__ ((__unused__)) = 0;                               \
    /* Whether __builtin_vis_alignaddr has been used. */                \
    unsigned int rc_gsr_ldinit_ __attribute__ ((__unused__)) = 0

#define RC_VEC_CLEANUP()

#define RC_VEC_LOAD(vec, ptr) \
    ((vec) = *(const rc_vec_t*)(ptr))

#define RC_VEC_STORE(ptr, vec) \
    (*(rc_vec_t*)(ptr) = (vec))

/**
 *  Note that we don't set or fake-set GSR here. This will work only
 *  as long as all vectors (all with operations where GSR is involved)
 *  are loaded through the resulting pointer. However, we don't really
 *  handle that, instead we make sure GSR-related operations and
 *  RC_VEC_LDINIT are not used together, guarded by mutual exlusion of
 *  nonzero values for rc_gsr_ldinit_ and rc_gsr_set_.
 */
#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)                      \
do {                                                                    \
    const uint8_t *uptr_;                                               \
    rc_vec_t vec3_;                                                     \
    (void)(vec1);                                                       \
    (void)(vec2);                                                       \
    /* Guard against use of other GSR-setting operations. */            \
    if (rc_gsr_set_ != 0) {                                             \
        extern void rc_mixing_RC_VEC_LDINIT_with_GSR_setting_(void);    \
        rc_mixing_RC_VEC_LDINIT_with_GSR_setting_();                    \
    }                                                                   \
    /* Mark that this operation has been used. */                       \
    rc_gsr_ldinit_ = 1;                                                 \
    /* Align down the pointer. */                                       \
    uptr_ = __builtin_vis_alignaddr((void*)(ptr), 0);                   \
    /* Load first vector. */                                            \
    RC_VEC_LOAD(vec3_, uptr_);                                          \
    /* Advance the aligned data pointer */                              \
    (uptr) = uptr_ + RC_VEC_SIZE;                                       \
    (vec3) = vec3_;                                                     \
} while (0)

/**
 *  Beware that there's a hidden secondary dependency from aligndata to
 *  the previous alignaddr; the GSR.alignaddr_offset setting is not
 *  explicit. As long as we use the primary result of falignaddr as a
 *  dependency for this operation (that we use the load from that
 *  address) and all subsequent operations that involve GSR, we should
 *  be safe. We need to make GSR (at this time, probably containing the
 *  default value) dependent on the result, so no GSR-setting operations
 *  are scheduled before the faligndata.
 */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)                      \
do {                                                                    \
    rc_vec_t vec_;                                                      \
    RC_VEC_LOAD(vec_, uptr);                                            \
    (dstv) = __builtin_vis_faligndatav8qi(vec3, vec_);                  \
    (vec3) = vec_;                                                      \
} while (0)

/**
 *  There are no specific shift insns for the vector registers, but we
 *  can use the alignaddr/faligndata pair, or set GSR and then use
 *  faligndata. We do the latter, which works better when we need to set
 *  GSR.scale_factor in the same function. There will be problems if we
 *  have to support shifts of misaligned data, but that's presently being
 *  guarded against.
 *
 *  We construct a vector used as the shifted-in part, so we use a
 *  zero-vector for the "usual" shifts. We also set GSR.alignaddr_offset
 *  to the shift count, and make the shifted-in vector depend on that.
 *  This shift count only works for "left" shifts, so we don't support
 *  right shifts at all. (Though introducing separate shift
 *  initializers, leaving RC_VEC_SHINIT as the alternative default, might
 *  be a good idea anyway.)
 */
#define RC_SHINITV_(shv, vec, bytes)            \
do {                                            \
    rc_vec_t v_ = (vec);                        \
    RC_GSR_ALIGN_(bytes, v_);                   \
    (shv) = v_;                                 \
} while (0)

#define RC_VEC_SHINIT(shv, bytes) \
    RC_SHINITV_(shv, RC_ZERO_, bytes)

#define RC_VEC_SHL(dstv, srcv, shv)                             \
do {                                                            \
    rc_vec_t dstv_ = __builtin_vis_faligndatav8qi(srcv, shv);   \
    RC_GSR_DEP_RESULT_(dstv_);                                  \
    (dstv) = dstv_;                                             \
} while (0)

/**
 *  To support RC_VEC_SHR, we'd have to have a different RC_VEC_SHINIT
 *  for it, but as RC_VEC_SHR isn't used at time of this writing,
 *  there's no need to implement it. Thus we lie about implementing the
 *  whole interface, though that's still true for all practical use.
 */
#undef RC_VEC_SHR

#define RC_VEC_SHLC(dstv, srcv, bytes)          \
do {                                            \
    rc_vec_t shv_;                              \
    RC_VEC_SHINIT(shv_, bytes);                 \
    RC_VEC_SHL(dstv, srcv, shv_);               \
} while (0)

/**
 *  RC_VEC_SHRC is as unused as RC_VEC_SHR but at least we can trivially
 *  implement this, if only just for show value.
 */
#define RC_VEC_SHRC(dstv, srcv, bytes)                  \
do {                                                    \
    rc_vec_t shv_;                                      \
    unsigned int bytes_ = (unsigned int)(bytes);        \
    if (bytes_ == 0) {                                  \
        (dstv) = (srcv);                                \
    }                                                   \
    else {                                              \
        RC_VEC_SHINIT(shv_, RC_VEC_SIZE - bytes_);      \
        RC_VEC_SHL(dstv, shv_, srcv);                   \
    }                                                   \
} while (0)

/**
 *  This is just like RC_VEC_SHLC, but shifting in a general
 *  vector instead of the zero-vector.
 */
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)        \
do {                                                    \
    rc_vec_t shv_;                                      \
    RC_SHINITV_(shv_, srcv2, bytes);                    \
    RC_VEC_SHL(dstv, srcv1, shv_);                      \
} while (0)

/**
 *  Regarding fpack16, it uses the GSR.scale_factor, shifting results
 *  from the 16-bit element form before truncating with saturation.
 *  For values entered as zero-extended bytes, this value needs to be 7
 *  to get the expected bytes back.
 */
#define RC_VEC_PACK(dstv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_vec_t zero_ = RC_ZERO_;                                          \
    rc_vec_t mask_ = (rc_vec_t){0, 255, 0, 255, 0, 255, 0, 255};        \
    rc_vec_t sm1_, sm2_, ss1_, ss2_, dstv_;                             \
    rc_vec4_type_ hi_, lo_;                                             \
    /**                                                                 \
     *  We use the zero-vector input as a common operand that we        \
     *  can make GSR depend on, for operations on both subvectors.      \
     */                                                                 \
    RC_GSR_ALIGN_SCALE_(RC_VEC_SIZE - 1, 7, zero_);                     \
    /* Shift sources to odd index, thus forming 16-bit elements. */     \
    RC_VEC_SHL(ss1_, zero_, srcv1);                                     \
    RC_VEC_SHL(ss2_, zero_, srcv2);                                     \
    /* Mask off high part so saturation has no effect. */               \
    RC_VEC_AND(sm1_, ss1_, mask_);                                      \
    RC_VEC_AND(sm2_, ss2_, mask_);                                      \
    hi_ = __builtin_vis_fpack16((rc_svec_type_)sm1_);                   \
    lo_ = __builtin_vis_fpack16((rc_svec_type_)sm2_);                   \
    RC_CONCAT_(dstv_, hi_, lo_);                                        \
    RC_GSR_DEP_RESULT_(dstv_);                                          \
    (dstv) = dstv_;                                                     \
} while (0)

#define RC_VEC_ZERO(vec) \
    ((vec) = RC_ZERO_)

/**
 *  No builtins for trivial logical operations, but using actual
 *  operators yields the expected code, as should be.
 */
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

/**
 *  The intuitive (rc_vec_t) {scal, scal, ..., scal} causes the vector to
 *  be constructed by byte stores on the stack with gcc at least as
 *  recent as gcc-4.3.2. We still get at least one store+load on stack,
 *  but that's necessary to copy from general to VIS (floating point)
 *  registers.
 */
#define RC_VEC_SPLAT(vec, scal)                 \
do {                                            \
    uint64_t val_ = (scal);                     \
    val_ |= val_ << 8;                          \
    val_ |= val_ << 16;                         \
    val_ |= val_ << 32;                         \
    (vec) = RC_SCAL_VEC_(val_);                 \
} while (0)

#define RC_TVEC_BLOB4_(t, s) \
    (t){(s), (s), (s), (s)}

#define RC_TVEC_BLOB8_(t, s) \
    (t){(s), (s), (s), (s), (s), (s), (s), (s)}

#define RC_VEC_BLOB_(s) \
    RC_TVEC_BLOB8_(rc_vec_t, s)

/**
 *  To get back values packed to bytes, entered by fexpand, the magic
 *  value for GSR.scale_factor is 3.
 */
#define RC_VEC_ADDS(dstv, srcv1, srcv2) \
    RC_ADDSAVGT_(dstv, srcv1, srcv2, 3)

/**
 *  We divide by two by just tweaking the magic number to
 *  GSR.scale_factor and the whole operation takes the same form as
 *  RC_VEC_ADDS.
 */
#define RC_VEC_AVGT(dstv, srcv1, srcv2) \
    RC_ADDSAVGT_(dstv, srcv1, srcv2, 2)

#define RC_ADDSAVGT_(dstv, srcv1, srcv2, sf) \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, sf, RC_ADD2_)

/**
 *  Similarly, RC_VEC_AVGR differs from RC_VEC_AVGT by a rounding term,
 *  corresponding to 1 (before the division), added to the 16-bit
 *  representation (fixed-point, not a regular 2's  complement
 *  representation).
 */
#define RC_VEC_AVGR(dstv, srcv1, srcv2)                         \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, 2, RC_FIXPADDROUND_)

#define RC_FIXPADDROUND_(a, b) \
  (a) + (b) + RC_TVEC_BLOB4_(rc_svec_type_, 16)

#define RC_ADDSAVGRT_(dstv, srcv1, srcv2, sf)    \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, sf, RC_ADD2_)

#define RC_ADD2_(a, b) (a) + (b)

#define RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, sf, op)            \
do {                                                            \
    rc_vec_t s1_ = (srcv1);                                     \
    rc_vec_t dstv_;                                             \
    rc_vec4_type_ hi_, lo_;                                     \
    rc_vec4_type_ s1hi_, s1lo_;                                 \
    rc_vec4_type_ s2hi_, s2lo_;                                 \
    rc_svec_type_ hi1_, hi2_, lo1_, lo2_, his_, los_;           \
    /* Fake GSR to be dependent on and input to srcv1. */       \
    RC_GSR_SCALE_(sf, s1_);                                     \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                               \
    RC_SPLIT_(srcv2, s2hi_, s2lo_);                             \
    hi1_ = __builtin_vis_fexpand(s1hi_);                        \
    lo1_ = __builtin_vis_fexpand(s1lo_);                        \
    hi2_ = __builtin_vis_fexpand(s2hi_);                        \
    lo2_ = __builtin_vis_fexpand(s2lo_);                        \
    his_ = op(hi1_, hi2_);                                      \
    los_ = op(lo1_, lo2_);                                      \
    hi_ = __builtin_vis_fpack16(his_);                          \
    lo_ = __builtin_vis_fpack16(los_);                          \
    RC_CONCAT_(dstv_, hi_, lo_);                                \
    RC_GSR_DEP_RESULT_(dstv_);                                  \
    (dstv) = dstv_;                                             \
} while (0)

#define RC_ARRAY16_(t, x, n)                            \
    t x[16] =                                           \
     {                                                  \
       (t){0,     0,   0, 0}, (t){0,     0,   0, (n)},  \
       (t){0,     0, (n), 0}, (t){0,     0, (n), (n)},  \
       (t){0,   (n),   0, 0}, (t){0,   (n),   0, (n)},  \
       (t){0,   (n), (n), 0}, (t){0,   (n), (n), (n)},  \
       (t){(n),   0,   0, 0}, (t){(n),   0,   0, (n)},  \
       (t){(n),   0, (n), 0}, (t){(n),   0, (n), (n)},  \
       (t){(n), (n),   0, 0}, (t){(n), (n),   0, (n)},  \
       (t){(n), (n), (n), 0}, (t){(n), (n), (n), (n)}   \
     }

/* The fcmp<X>16 insns are missing as builtins. */
#define RC_CMP_(op, dst, src1, src2)                    \
    __asm__("fcmp" #op "16 %[op1],%[op2],%[dest]" :     \
            [dest] "=r" (dst) :                         \
            [op1] "e" (src1), [op2] "e" (src2))

/**
 *  Generating the rounding vector numbers numbers as scalars, e.g.
 *  as "((cmphi_ & 1) << 4) | ((cmphi_ & 2) << 19) |
 *  ((uint64_t)(cmphi_ & 4) << 34) | ((uint64_t)(cmphi_ & 8) << 49)",
 *  is not a good idea, as you get a load and a store on stack together
 *  with a bunch of scalar shifts and masks compared to the
 *  forming of address (a single shift) and load from rounds_[].
 *  FIXME: make sure rounds_[] is shared.
 */
#define RC_VEC_AVGZ(dstv, srcv1, srcv2)                         \
do {                                                            \
    rc_vec_t s1_ = (srcv1);                                     \
    rc_vec_t dstv_;                                             \
    rc_vec4_type_ hi_, lo_;                                     \
    rc_svec_type_ hi1_, hi2_, lo1_, lo2_, his_, los_;           \
    rc_svec_type_ roundhi_, roundlo_;                           \
    rc_vec4_type_ s1hi_, s1lo_;                                 \
    rc_vec4_type_ s2hi_, s2lo_;                                 \
    unsigned int cmphi_, cmplo_;                                \
    static const RC_ARRAY16_(rc_svec_type_, rounds_, 16);       \
    RC_GSR_SCALE_(2, s1_);                                      \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                               \
    RC_SPLIT_(srcv2, s2hi_, s2lo_);                             \
    hi1_ = __builtin_vis_fexpand(s1hi_);                        \
    lo1_ = __builtin_vis_fexpand(s1lo_);                        \
    hi2_ = __builtin_vis_fexpand(s2hi_);                        \
    lo2_ = __builtin_vis_fexpand(s2lo_);                        \
    RC_CMP_(gt, cmphi_, hi1_, hi2_);                            \
    RC_CMP_(gt, cmplo_, lo1_, lo2_);                            \
    roundhi_ = rounds_[cmphi_];                                 \
    roundlo_ = rounds_[cmplo_];                                 \
    his_ = hi1_ + hi2_ + roundhi_;                              \
    los_ = lo1_ + lo2_ + roundlo_;                              \
    hi_ = __builtin_vis_fpack16(his_);                          \
    lo_ = __builtin_vis_fpack16(los_);                          \
    RC_CONCAT_(dstv_, hi_, lo_);                                \
    RC_GSR_DEP_RESULT_(dstv_);                                  \
    (dstv) = dstv_;                                             \
} while (0)

/* Subtraction with saturation is like addition with saturation. */
#define RC_VEC_SUBS(dstv, srcv1, srcv2)                         \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, 3, RC_MINUS12_)

#define RC_MINUS12_(a, b) ((a) - (b))

/**
 *  We don't have an specific operation for absolute value, but with
 *  subtraction with saturation we can just "or" together results after
 *  subtractions both ways.
 */
#define RC_VEC_SUBA(dstv, srcv1, srcv2)                 \
do {                                                    \
    rc_vec_t s1_ = (srcv1);                             \
    rc_vec_t dstv_;                                     \
    rc_vec4_type_ s1hi_, s1lo_, s2hi_, s2lo_;           \
    rc_vec4_type_ hip_, lop_, hin_, lon_, hi_, lo_;     \
    rc_svec_type_ hi1_, hi2_, lo1_, lo2_;               \
    rc_svec_type_ hisp_, losp_, hisn_, losn_;           \
    RC_GSR_SCALE_(3, s1_);                              \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                       \
    RC_SPLIT_(srcv2, s2hi_, s2lo_);                     \
    hi1_ = __builtin_vis_fexpand(s1hi_);                \
    lo1_ = __builtin_vis_fexpand(s1lo_);                \
    hi2_ = __builtin_vis_fexpand(s2hi_);                \
    lo2_ = __builtin_vis_fexpand(s2lo_);                \
    hisp_ = hi1_ - hi2_;                                \
    hisn_ = hi2_ - hi1_;                                \
    losp_ = lo1_ - lo2_;                                \
    losn_ = lo2_ - lo1_;                                \
    hip_ = __builtin_vis_fpack16(hisp_);                \
    hin_ = __builtin_vis_fpack16(hisn_);                \
    lop_ = __builtin_vis_fpack16(losp_);                \
    lon_ = __builtin_vis_fpack16(losn_);                \
    hi_ = hip_ | hin_;                                  \
    lo_ = lop_ | lon_;                                  \
    RC_CONCAT_(dstv_, hi_, lo_);                        \
    RC_GSR_DEP_RESULT_(dstv_);                          \
    (dstv) = dstv_;                                     \
} while (0)

#define RC_VEC_SUBHT(dstv, srcv1, srcv2) \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, 2, RC_FIXPSUBHT_)

#define RC_FIXPSUBHT_(a, b) \
    ((a) - (b) + RC_TVEC_BLOB4_(rc_svec_type_, 255*16))

#define RC_VEC_SUBHR(dstv, srcv1, srcv2) \
    RC_ADDSUBSAVGRT_(dstv, srcv1, srcv2, 2, RC_FIXPSUBHR_)

#define RC_FIXPSUBHR_(a, b) \
    ((a) - (b) + RC_TVEC_BLOB4_(rc_svec_type_, 256*16))

#define RC_VEC_ABS(dstv, srcv)                                  \
do {                                                            \
    rc_vec_t s1_ = (srcv);                                      \
    rc_vec_t dstv_;                                             \
    rc_vec4_type_ s1hi_, s1lo_;                                 \
    rc_vec4_type_ hip_, lop_, hin_, lon_, hi_, lo_;             \
    rc_svec_type_ hi1_, lo1_, hisp_, losp_, hisn_, losn_;       \
    rc_svec_type_ fixp128_ =                                    \
        RC_TVEC_BLOB4_(rc_svec_type_, 128*16);                  \
    RC_GSR_SCALE_(4, s1_);                                      \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                               \
    hi1_ = __builtin_vis_fexpand(s1hi_);                        \
    lo1_ = __builtin_vis_fexpand(s1lo_);                        \
    hisp_ = hi1_ - fixp128_;                                    \
    hisn_ = fixp128_ - hi1_;                                    \
    losp_ = lo1_ - fixp128_;                                    \
    losn_ = fixp128_ - lo1_;                                    \
    hip_ = __builtin_vis_fpack16(hisp_);                        \
    hin_ = __builtin_vis_fpack16(hisn_);                        \
    lop_ = __builtin_vis_fpack16(losp_);                        \
    lon_ = __builtin_vis_fpack16(losn_);                        \
    hi_ = hip_ | hin_;                                          \
    lo_ = lop_ | lon_;                                          \
    RC_CONCAT_(dstv_, hi_, lo_);                                \
    RC_GSR_DEP_RESULT_(dstv_);                                  \
    (dstv) = dstv_;                                             \
} while (0)

/**
 *  We use the same trick as in the SWAR backend, saving two compares
 *  and subsequent loads at the expense of a NOT and two additions.
 */
#define RC_VEC_CMPGT(dstv, srcv1, srcv2) \
    RC_VEC_AVGT(dstv, srcv1, ~(srcv2))

#define RC_VEC_CMPGE(dstv, srcv1, srcv2)        \
do {                                            \
    rc_vec_t gt_;                               \
    RC_VEC_CMPGT(gt_, srcv2, srcv1);            \
    RC_VEC_NOT(dstv, gt_);                      \
} while (0)

#define RC_VEC_MIN(dstv, srcv1, srcv2) \
    RC_VEC_MINMAX_(dstv, srcv1, srcv2, RC_VEC_NOT_, RC_VEC_ID_)

#define RC_VEC_NOT_(x) (~(x))
#define RC_VEC_ID_(x) (x)

#define RC_VEC_MINMAX_(dstv, srcv1, srcv2, s1op, s2op)          \
do {                                                            \
    rc_vec_t s1_ = (srcv1);                                     \
    rc_vec_t s2_ = (srcv2);                                     \
    rc_svec_type_ hi1_, hi2_, lo1_, lo2_;                       \
    rc_vec4_type_ s1hi_, s1lo_;                                 \
    rc_vec4_type_ s2hi_, s2lo_;                                 \
    static const RC_ARRAY16_(rc_vec4_type_, masks_, 255);       \
    rc_vec4_type_ masklo_, maskhi_;                             \
    unsigned int cmphi_, cmplo_;                                \
    rc_vec_t mask_;                                             \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                               \
    RC_SPLIT_(s2_, s2hi_, s2lo_);                               \
    hi1_ = __builtin_vis_fexpand(s1hi_);                        \
    lo1_ = __builtin_vis_fexpand(s1lo_);                        \
    hi2_ = __builtin_vis_fexpand(s2hi_);                        \
    lo2_ = __builtin_vis_fexpand(s2lo_);                        \
    RC_CMP_(gt, cmphi_, hi1_, hi2_);                            \
    RC_CMP_(gt, cmplo_, lo1_, lo2_);                            \
    maskhi_ = masks_[cmphi_];                                   \
    masklo_ = masks_[cmplo_];                                   \
    RC_CONCAT_(mask_, maskhi_, masklo_);                        \
    (dstv) = (s1_ & s1op(mask_)) | (s2_ & s2op(mask_));         \
} while (0)

#define RC_VEC_MAX(dstv, srcv1, srcv2) \
    RC_VEC_MINMAX_(dstv, srcv1, srcv2, RC_VEC_ID_, RC_VEC_NOT_)

/**
 *  The blend vector is a single (blend8 << 8) as a 16-bit number, or
 *  shorter expressed as "((blendv) = (rc_vec_t) {blend8})", but if
 *  we do that, we run into <http://gcc.gnu.org/bugzilla/PR48610>.
 */
#define RC_VEC_BLEND(blendv, blend8) \
  ((blendv) = RC_SCAL_VEC_((uint64_t) (blend8) << (8 * (RC_VEC_SIZE - 1))))

#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)         \
do {                                                            \
    rc_svec_type_ bias_ = RC_TVEC_BLOB4_(rc_svec_type_, 0x80);  \
    RC_VEC_LERP_(dstv, srcv1, srcv2, blendv, bias_, bias_);     \
} while (0)

/**
 *  The only useful multiplications (fmul8x16 and fmul8x16au) are
 *  ((int8_t << 8) * uint8_t) >> 8, but luckily, that's all we need
 *  (well, two of them).
 */
#define RC_VEC_LERP_(dstv, srcv1, srcv2, blendv, biashi, biaslo)        \
do {                                                                    \
    rc_vec_t s1__ = (srcv1);                                            \
    rc_vec_t s2__ = (srcv2);                                            \
    rc_vec_t dbhix__, dblox__, dstv__;                                  \
    rc_svec2_type_ bv__ = RC_VEC_TVEC_(rc_svec2_type_, blendv);         \
    rc_vec4_type_ zero__ = RC_SCAL_TVEC_(rc_vec4_type_, 0);             \
    rc_svec_type_ s1hi__, s1lo__, s1bhi__, s1blo__;                     \
    rc_svec_type_ s2bhi__, s2blo__, dbhihi__, dblohi__;                 \
    rc_svec_type_ dbhi__, dblo__;                                       \
    rc_vec4_type_ s1hi4__, s2hi4__, s1lo4__, s2lo4__;                   \
    rc_vec4_type_ hi__, lo__;                                           \
    rc_svec_type_ masklow__ = RC_TVEC_BLOB4_(rc_svec_type_, 0xff);      \
    /* Move srcv1 and srcv2 to the high bits of two 16-bit vectors. */  \
    RC_SPLIT_(s1__, s1hi4__, s1lo4__);                                  \
    RC_SPLIT_(s2__, s2hi4__, s2lo4__);                                  \
    s1hi__ = (rc_svec_type_)__builtin_vis_fpmerge(s1hi4__, zero__);     \
    s1lo__ = (rc_svec_type_)__builtin_vis_fpmerge(s1lo4__, zero__);     \
    RC_GSR_ALIGN_SCALE_(RC_VEC_SIZE - 1, 7, bv__);                      \
    /**                                                                 \
     *  We need to do the blending before subtraction, due to the       \
     *  multiplication operand only working on a four-element uint8_t   \
     *  vector, the other operand being four-element int16_t.           \
     */                                                                 \
    s2bhi__ = __builtin_vis_fmul8x16au(s2hi4__, bv__);                  \
    s2blo__ = __builtin_vis_fmul8x16au(s2lo4__, bv__);                  \
    s1bhi__ = __builtin_vis_fmul8x16au(s1hi4__, bv__);                  \
    s1blo__ = __builtin_vis_fmul8x16au(s1lo4__, bv__);                  \
    /* Now, srcv2*blendv - srcv1*blendv + bias + srcv1. */              \
    dbhihi__ = s2bhi__ - s1bhi__ + biashi + s1hi__;                     \
    dblohi__ = s2blo__ - s1blo__ + biaslo + s1lo__;                     \
    /**                                                                 \
     *  We have to move the data from the high to the low bits before   \
     *  "packing" with fpack16, as it can't shift that far.             \
     */                                                                 \
    RC_VEC_SHL(dbhix__, (rc_vec_t)dbhihi__, (rc_vec_t)dbhihi__);        \
    RC_VEC_SHL(dblox__, (rc_vec_t)dblohi__, (rc_vec_t)dblohi__);        \
    dbhi__ = (rc_svec_type_)dbhix__ & masklow__;                        \
    dblo__ = (rc_svec_type_)dblox__ & masklow__;                        \
    hi__ = __builtin_vis_fpack16(dbhi__);                               \
    lo__ = __builtin_vis_fpack16(dblo__);                               \
    RC_CONCAT_(dstv__, hi__, lo__);                                     \
    RC_GSR_DEP_RESULT_(dstv__);                                         \
    (dstv) = dstv__;                                                    \
} while (0)

#define RC_VEC_BLENDZ(blendv, blend8) \
     RC_VEC_BLEND(blendv, blend8)

#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv) \
    RC_LERPNZ_(dstv, srcv1, srcv2, blendv, RC_1st_, RC_2nd_)

#define RC_1st_(a, b) (a)
#define RC_2nd_(a, b) (b)

#define RC_LERPNZ_(dstv, srcv1, srcv2, blendv, cmp1st, cmp2nd)          \
do {                                                                    \
    rc_vec_t s1_ = (srcv1);                                             \
    rc_vec_t s2_ = (srcv2);                                             \
    unsigned int cmphi_, cmplo_;                                        \
    rc_svec_type_ biashi_, biaslo_;                                     \
    rc_vec4_type_ s1hi_, s1lo_, s2hi_, s2lo_;                           \
    rc_svec_type_ hi1_, hi2_, lo1_, lo2_;                               \
    static const RC_ARRAY16_(rc_svec_type_, masks_, 255);               \
    RC_SPLIT_(s1_, s1hi_, s1lo_);                                       \
    RC_SPLIT_(s2_, s2hi_, s2lo_);                                       \
    hi1_ = __builtin_vis_fexpand(s1hi_);                                \
    lo1_ = __builtin_vis_fexpand(s1lo_);                                \
    hi2_ = __builtin_vis_fexpand(s2hi_);                                \
    lo2_ = __builtin_vis_fexpand(s2lo_);                                \
    RC_CMP_(gt, cmphi_, cmp1st(hi1_, hi2_), cmp2nd(hi1_, hi2_));        \
    RC_CMP_(gt, cmplo_, cmp1st(lo1_, lo2_), cmp2nd(lo1_, lo2_));        \
    biashi_ = masks_[cmphi_];                                           \
    biaslo_ = masks_[cmplo_];                                           \
    RC_VEC_LERP_(dstv, s1_, s2_, blendv, biashi_, biaslo_);             \
} while (0)

#define RC_VEC_BLENDN(blendv, blend8) \
     RC_VEC_BLEND(blendv, blend8)

#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv) \
    RC_LERPNZ_(dstv, srcv1, srcv2, blendv, RC_2nd_, RC_1st_)

/**
 *  Finally a use for the fcmp<X>16 operations leaving their result in a
 *  general register!
 */
#define RC_VEC_GETMASKW(maskw, vec)                             \
do {                                                            \
    unsigned int maskwhi_, maskwlo_;                            \
    rc_vec4_type_ vechi4_, veclo4_;                             \
    rc_svec_type_ vechi_, veclo_;                               \
    rc_svec_type_ maskv_ = RC_TVEC_BLOB4_(rc_svec_type_, 0x80); \
    rc_vec4_type_ zero_ = RC_SCAL_TVEC_(rc_vec4_type_, 0);      \
    RC_SPLIT_(vec, vechi4_, veclo4_);                           \
    vechi_ = (rc_svec_type_)__builtin_vis_fpmerge(zero_,        \
                                                  vechi4_);     \
    veclo_ = (rc_svec_type_)__builtin_vis_fpmerge(zero_,        \
                                                  veclo4_);     \
    RC_CMP_(le, maskwhi_, maskv_, vechi_);                      \
    RC_CMP_(le, maskwlo_, maskv_, veclo_);                      \
    (maskw) = (maskwhi_ << 4) | maskwlo_;                       \
} while (0)

/**
 *  There's no apparent benefit for defining RC_VEC_CNTV (CNTN, CNTS):
 *  no operations that can be used for bit-counting on floating-point
 *  registers. The only theoretical advantage compared to SWAR64 would be
 *  to possibly use the PDIST instruction in the CNTR reduction step, but
 *  that is offset by lack of a right-shift operator (and any other
 *  reasonable way) in the CNTV step. Anyway, for completeness:
 */

/* See RC_VEC_SUMN. */
#define RC_VEC_CNTN 1024

#define RC_VEC_CNTV(accv, srcv)                 \
do {                                            \
    uint64_t acc_ = RC_VEC_SCAL_(srcv);         \
    rc_vec_t accv_;                             \
    uint64_t tmp_;                              \
    uint64_t c1_ = 0x5555555555555555ULL;       \
    uint64_t c2_ = 0x3333333333333333ULL;       \
    uint64_t c4_ = 0x0f0f0f0f0f0f0f0fULL;       \
    tmp_ = acc_ >> 1;  /* tmp  = acc >> 1  */   \
    tmp_ &= c1_;       /* tmp &= 0x55...   */   \
    acc_ &= c1_;       /* acc &= 0x55...   */   \
    acc_ += tmp_;      /* acc += tmp       */   \
    tmp_ = acc_ >> 2;  /* tmp  = acc >> 2  */   \
    tmp_ &= c2_;       /* tmp &= 0x33...   */   \
    acc_ &= c2_;       /* acc &= 0x33...   */   \
    acc_ += tmp_;      /* acc += tmp       */   \
    tmp_ = acc_ >> 4;  /* tmp  = acc >> 4  */   \
    acc_ += tmp_;      /* acc += tmp       */   \
    acc_ &= c4_;       /* acc &= 0x0f...   */   \
    accv_ = RC_SCAL_VEC_(acc_);                 \
    RC_VEC_SUMV(accv, accv_);                   \
} while (0)

#define RC_VEC_CNTR(cnt, accv)                  \
    RC_VEC_SUMR(cnt, accv)

/**
 *  We reduce in the CNTV step too, as pdist is accumulating, so the
 *  upper bound is actually the limits of the uint64_t accumulator.
 */
#define RC_VEC_SUMN 1024

/* The sum of elements equals the sum of each "absolute distance" to zero. */
#define RC_VEC_SUMV(accv, srcv)                                         \
    ((accv) = RC_SCAL_VEC_(__builtin_vis_pdist (srcv, RC_ZERO_,         \
                                                RC_VEC_SCAL_(accv))))

#define RC_VEC_SUMR(cnt, accv) \
    ((cnt) = RC_VEC_SCAL_(accv))

/**
 *  Like RC_VEC_CNTx, there is not much leverage to the SWAR64
 *  alternative, as we don't have a proper uint8_t-by-uint8_t element
 *  multiplicaton, but still defined for reference. We fpmerge one of
 *  the arrays with zero, then fmul8x16 on each pair. Except, because we
 *  can only multiply a int8_t by uint8_t, we first have to mask off the
 *  MSB for one of the operands, divide it by two (getting 0 or 64) and
 *  multiply it separately, then add that result in twice to the
 *  7bit-by-8bit result. (No, we can't treat the result as unsigned and
 *  mask off the sign bits, that doesn't work for a 16-bit result.)
 *
 *  After the multiplication, if we were doing separate accumulation,
 *  we'd then add the elements together. For reduction, we'd (mask off
 *  the other part and) add bytes in the high part and the low part of
 *  the 16-bit element separately using pdist. The reduction step would
 *  then be "mac = highbyte * 256 + lowbyte". As 255*255 only fits in 16
 *  bits, we could only "accumulate" once that way, but we do most of
 *  the reduction here too, calling pdist, but keep the high and low
 *  parts separate. For the reduction step we save the *256
 *  multiplication, which necessarily involves moving operands to scalar
 *  (general) registers, a move which has to go through intermediate
 *  memory. We can do the accumulation as many times as the result of
 *  each 4-element subvector fits in a uint32_t.
 */
#define RC_VEC_MACN 1024 /* Really, 4*floor(1<<32)/255/255/(8/2)/4) = 16512. */

#define RC_VEC_MACV(accv, srcv1, srcv2)                                 \
do {                                                                    \
    rc_vec_t s1_ = (srcv1);                                             \
    rc_vec_t s2_ = (srcv2);                                             \
    rc_vec_t accvin_ = (accv);                                          \
    rc_vec_t s1lo7_, s1msb_, accvout_;                                  \
    uint32_t maclo_, machi_;                                            \
    rc_svec_type_ masklow_ = RC_TVEC_BLOB4_(rc_svec_type_, 255);        \
    rc_svec_type_ s1msbhi_, s1msblo_, s1lo7hi_, s1lo7lo_;               \
    rc_svec_type_ s1msbdiv2hi_, s1msbdiv2lo_;                           \
    rc_vec4_type_ s1lo7hi4_, s1lo7lo4_, s1msbhi4_, s1msblo4_;           \
    rc_vec4_type_ s1msbdiv2hi4_, s1msbdiv2lo4_, s2hi4_, s2lo4_;         \
    rc_vec4_type_ accvhi4_, accvlo4_;                                   \
    rc_svec_type_ mulhilo7_, mullolo7_, mulhimsbdiv2_, mullomsbdiv2_;   \
    rc_svec_type_ mulhi_, mullo_, mulhihi_, mullohi_;                   \
    rc_svec_type_ mulhilo_, mullolo_;                                   \
    rc_vec4_type_ zero4_ = RC_SCAL_TVEC_(rc_vec4_type_, 0);             \
    rc_vec_t msb_ = RC_VEC_BLOB_(0x80);                                 \
    RC_VEC_AND(s1msb_, s1_, msb_);                                      \
    RC_VEC_AND(s1lo7_, s1_, ~msb_);                                     \
    RC_GSR_SCALE_(2, s1msb_);                                           \
    RC_SPLIT_(s1msb_, s1msbhi4_, s1msblo4_);                            \
    s1msbhi_ = __builtin_vis_fexpand(s1msbhi4_);                        \
    s1msblo_ = __builtin_vis_fexpand(s1msblo4_);                        \
    /* Divide the MSB by 2 through fpack16. */                          \
    s1msbdiv2hi4_ = __builtin_vis_fpack16(s1msbhi_);                    \
    s1msbdiv2lo4_ = __builtin_vis_fpack16(s1msblo_);                    \
    RC_SPLIT_(s2_, s2hi4_, s2lo4_);                                     \
    RC_SPLIT_(s1lo7_, s1lo7hi4_, s1lo7lo4_);                            \
    /**                                                                 \
     *  Merge the multiplication operands from the split parts by       \
     *  zipping in 0 in the *low* bits.                                 \
     */                                                                 \
    s1msbdiv2hi_ = (rc_svec_type_)__builtin_vis_fpmerge(s1msbdiv2hi4_,  \
                                                        zero4_);        \
    s1msbdiv2lo_ = (rc_svec_type_)__builtin_vis_fpmerge(s1msbdiv2lo4_,  \
                                                        zero4_);        \
    s1lo7hi_ = (rc_svec_type_)__builtin_vis_fpmerge(s1lo7hi4_, zero4_); \
    s1lo7lo_ = (rc_svec_type_)__builtin_vis_fpmerge(s1lo7lo4_, zero4_); \
    /* Do the multiplications, leaving proper 16-bit results. */        \
    mulhilo7_ = __builtin_vis_fmul8x16(s2hi4_, s1lo7hi_);               \
    mullolo7_ = __builtin_vis_fmul8x16(s2lo4_, s1lo7lo_);               \
    mulhimsbdiv2_ = __builtin_vis_fmul8x16(s2hi4_, s1msbdiv2hi_);       \
    mullomsbdiv2_ = __builtin_vis_fmul8x16(s2lo4_, s1msbdiv2lo_);       \
    /* Add the parts to get the final products. */                      \
    mulhi_ = mulhilo7_ + mulhimsbdiv2_ + mulhimsbdiv2_;                 \
    mullo_ = mullolo7_ + mullomsbdiv2_ + mullomsbdiv2_;                 \
    /* Separate the high and low parts so we can apply pdist. */        \
    mulhihi_ = mulhi_ & ~masklow_;                                      \
    mulhilo_ = mulhi_ & masklow_;                                       \
    mullohi_ = mullo_ & ~masklow_;                                      \
    mullolo_ = mullo_ & masklow_;                                       \
    RC_SPLITLOW_(accvin_, accvhi4_, accvlo4_);                          \
    maclo_ =                                                            \
        __builtin_vis_pdist ((rc_vec_t)mullolo_, RC_ZERO_,              \
                             RC_TVEC_SCAL32_(rc_vec4_type_, accvlo4_)); \
    maclo_ =                                                            \
        __builtin_vis_pdist ((rc_vec_t)mulhilo_, RC_ZERO_, maclo_);     \
    machi_ =                                                            \
        __builtin_vis_pdist ((rc_vec_t)mullohi_, RC_ZERO_,              \
                             RC_TVEC_SCAL32_(rc_vec4_type_, accvhi4_)); \
    machi_ =                                                            \
        __builtin_vis_pdist ((rc_vec_t)mulhihi_, RC_ZERO_, machi_);     \
    RC_CONCATLOW_(accvout_,                                             \
                  RC_SCAL32_TVEC_(rc_vec4_type_, (uint32_t)machi_),     \
                  RC_SCAL32_TVEC_(rc_vec4_type_, (uint32_t)maclo_));    \
    RC_GSR_DEP_RESULT_(accvout_);                                       \
    (accv) = accvout_;                                                  \
} while (0)

/* Extract the two uint32_t's and form the high + low sum. */
#define RC_VEC_MACR(mac, accv)                          \
do {                                                    \
    rc_vec4_type_ accvhi4_, accvlo4_;                   \
    uint64_t machi_, maclo_;                            \
    RC_SPLITLOW_(accv, accvhi4_, accvlo4_);             \
    machi_ = RC_TVEC_SCAL32_(rc_vec4_type_, accvhi4_);  \
    maclo_ = RC_TVEC_SCAL32_(rc_vec4_type_, accvlo4_);  \
    (mac) = maclo_ + machi_ * 256;                      \
} while (0)

#endif /* RC_VECTOR_VIS_H */
