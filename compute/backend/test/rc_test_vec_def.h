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
 *  @file   rc_test_vec_def.h
 *  @brief  Test function definitions for vector operations.
 */

#include <assert.h> /* assert() */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Define the function name macro.
 */
#undef RC_TEST_VEC_FUNC
#ifdef RC_TEST_VEC_REF
#define RC_TEST_VEC_FUNC(name) \
    rc_test_vec_ ## name ## _ref
#else
#define RC_TEST_VEC_FUNC(name) \
    rc_test_vec_ ## name
#endif

/* Using macros to avoid copy-paste source as much as possible. */

#define RC_TEST_BINOP_FUNCTION(opup, oplo)                      \
static int                                                      \
RC_TEST_VEC_FUNC(oplo)(uint8_t *dst, const uint8_t *src1,       \
                       const uint8_t *src2, int val)            \
{                                                               \
    rc_vec_t dstv, srcv1, srcv2;                                \
    RC_VEC_DECLARE();                                           \
    (void)val;                                                  \
    RC_VEC_LOAD(srcv1, src1);                                   \
    RC_VEC_LOAD(srcv2, src2);                                   \
    RC_VEC_ ## opup(dstv, srcv1, srcv2);                        \
    RC_VEC_STORE(dst, dstv);                                    \
    RC_VEC_CLEANUP();                                           \
    return 0;                                                   \
}

#define RC_TEST_UNOP_FUNCTION(opup, oplo)                       \
static int                                                      \
RC_TEST_VEC_FUNC(oplo)(uint8_t *dst, const uint8_t *src1,       \
                       const uint8_t *src2, int val)            \
{                                                               \
    rc_vec_t dstv, srcv1;                                       \
    RC_VEC_DECLARE();                                           \
    (void)val;                                                  \
    (void)src2;                                                 \
    RC_VEC_LOAD(srcv1, src1);                                   \
    RC_VEC_ ## opup(dstv, srcv1);                               \
    RC_VEC_STORE(dst, dstv);                                    \
    RC_VEC_CLEANUP();                                           \
    return 0;                                                   \
}

/**
 *  These functions have a two-vector result. We split up testing into
 *  left and right destination part rather than adding test-framework
 *  bits for testing two destination vectors.
 *  Using macros to avoid copy-paste source as much as possible.
 */

#define RC_TEST_CONV_FUNCTION(lr, ext)                          \
static int                                                      \
RC_TEST_VEC_FUNC(lr ## ext)(uint8_t *dst, const uint8_t *src1,  \
                            const uint8_t *src2, int val)       \
{                                                               \
    rc_vec_t srcv, ldstv, rdstv;                                \
    RC_VEC_DECLARE();                                           \
    (void)src2;                                                 \
    (void)val;                                                  \
    (void)ldstv; /* One of ldstv, rdstv is "unused-but-set". */ \
    (void)rdstv;                                                \
    RC_VEC_LOAD(srcv, src1);                                    \
    RC_VEC ## ext(ldstv, rdstv, srcv);                          \
    RC_VEC_STORE(dst, lr ## dstv);                              \
    RC_VEC_CLEANUP();                                           \
    return 0;                                                   \
}

/*
 * -------------------------------------------------------------
 *  Misaligned memory access
 * -------------------------------------------------------------
 */

#if defined RC_VEC_LDINIT && defined RC_VEC_LOADU
static int
RC_TEST_VEC_FUNC(loadu)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int val)
{
    uint8_t        buf[RC_VEC_SIZE];
    const uint8_t *ptr;
    rc_vec_t       u1, u2, u3;
    rc_vec_t       vec1, vec2;
    int            k;

    RC_VEC_DECLARE();
    (void)src1;
    (void)src2;
    assert(src1 == dst  + RC_VEC_SIZE);
    assert(src2 == src1 + RC_VEC_SIZE);

    /* First load */
    RC_VEC_LDINIT(u1, u2, u3, ptr, &src1[val]);
    RC_VEC_LOADU(vec1, u1, u2, u3, ptr);
    RC_VEC_STORE(dst, vec1);
    memcpy(buf, dst, RC_VEC_SIZE);

    /* Second load should be the same */
    RC_VEC_LDINIT(u1, u2, u3, ptr, &dst[val]);
    RC_VEC_LOADU(vec2, u1, u2, u3, ptr);
    ptr += RC_VEC_SIZE;
    RC_VEC_LOADU(vec2, u1, u2, u3, ptr);
    RC_VEC_STORE(dst, vec2);

    /* Combine */
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dst[k] |= buf[k];
    }

    RC_VEC_CLEANUP();

    return 0;
}
#else
#define rc_test_vec_loadu NULL
#endif


/*
 * -------------------------------------------------------------
 *  Field-independent operations
 * -------------------------------------------------------------
 */

#ifdef RC_VEC_ZERO
static int
RC_TEST_VEC_FUNC(zero)(uint8_t *dst, const uint8_t *src1,
                       const uint8_t *src2, int val)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();
    (void)src1;
    (void)src2;
    (void)val;
    RC_VEC_ZERO(vec);
    RC_VEC_STORE(dst, vec);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_zero NULL
#endif

#ifdef RC_VEC_NOT
RC_TEST_UNOP_FUNCTION(NOT, not)
#else
#define rc_test_vec_not NULL
#endif

#ifdef RC_VEC_AND
RC_TEST_BINOP_FUNCTION(AND, and)
#else
#define rc_test_vec_and NULL
#endif

#ifdef RC_VEC_OR
RC_TEST_BINOP_FUNCTION(OR, or)
#else
#define rc_test_vec_or NULL
#endif

#ifdef RC_VEC_XOR
RC_TEST_BINOP_FUNCTION(XOR, xor)
#else
#define rc_test_vec_xor NULL
#endif

#ifdef RC_VEC_ANDNOT
RC_TEST_BINOP_FUNCTION(ANDNOT, andnot)
#else
#define rc_test_vec_andnot NULL
#endif

#ifdef RC_VEC_ORNOT
RC_TEST_BINOP_FUNCTION(ORNOT, ornot)
#else
#define rc_test_vec_ornot NULL
#endif

#ifdef RC_VEC_XORNOT
RC_TEST_BINOP_FUNCTION(XORNOT, xornot)
#else
#define rc_test_vec_xornot NULL
#endif


/*
 * -------------------------------------------------------------
 *  Field relocation
 * -------------------------------------------------------------
 */

#if defined RC_VEC_SHINIT && defined RC_VEC_SHL
static int
RC_TEST_VEC_FUNC(shl)(uint8_t *dst, const uint8_t *src1,
                      const uint8_t *src2, int bytes)
{
    rc_vec_t dstv, srcv, shv;
    RC_VEC_DECLARE();
    (void)src2;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_SHINIT(shv, bytes);
    RC_VEC_SHL(dstv, srcv, shv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_shl NULL
#endif

#if defined RC_VEC_SHINIT && defined RC_VEC_SHR
static int
RC_TEST_VEC_FUNC(shr)(uint8_t *dst, const uint8_t *src1,
                      const uint8_t *src2, int bytes)
{
    rc_vec_t dstv, srcv, shv;
    RC_VEC_DECLARE();
    (void)src2;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_SHINIT(shv, bytes);
    RC_VEC_SHR(dstv, srcv, shv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_shr NULL
#endif

#ifdef RC_VEC_SHLC
static int
RC_TEST_VEC_FUNC(shlc)(uint8_t *dst, const uint8_t *src1,
                       const uint8_t *src2, int bytes)
{
    rc_vec_t dstv, srcv;
    RC_VEC_DECLARE();
    (void)src2;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_ZERO(dstv);
    switch (bytes) {
        case  0: RC_VEC_SHLC(dstv, srcv,  0); break;
#if RC_VEC_SIZE > 1
        case  1: RC_VEC_SHLC(dstv, srcv,  1); break;
#endif
#if RC_VEC_SIZE > 2
        case  2: RC_VEC_SHLC(dstv, srcv,  2); break;
        case  3: RC_VEC_SHLC(dstv, srcv,  3); break;
#endif
#if RC_VEC_SIZE > 4
        case  4: RC_VEC_SHLC(dstv, srcv,  4); break;
        case  5: RC_VEC_SHLC(dstv, srcv,  5); break;
        case  6: RC_VEC_SHLC(dstv, srcv,  6); break;
        case  7: RC_VEC_SHLC(dstv, srcv,  7); break;
#endif
#if RC_VEC_SIZE > 8
        case  8: RC_VEC_SHLC(dstv, srcv,  8); break;
        case  9: RC_VEC_SHLC(dstv, srcv,  9); break;
        case 10: RC_VEC_SHLC(dstv, srcv, 10); break;
        case 11: RC_VEC_SHLC(dstv, srcv, 11); break;
        case 12: RC_VEC_SHLC(dstv, srcv, 12); break;
        case 13: RC_VEC_SHLC(dstv, srcv, 13); break;
        case 14: RC_VEC_SHLC(dstv, srcv, 14); break;
        case 15: RC_VEC_SHLC(dstv, srcv, 15); break;
#endif
    }
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_shlc NULL
#endif

#ifdef RC_VEC_SHRC
static int
RC_TEST_VEC_FUNC(shrc)(uint8_t *dst, const uint8_t *src1,
                       const uint8_t *src2, int bytes)
{
    rc_vec_t dstv, srcv;
    RC_VEC_DECLARE();
    (void)src2;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_ZERO(dstv);
    switch (bytes) {
        case  0: RC_VEC_SHRC(dstv, srcv,  0); break;
#if RC_VEC_SIZE > 1
        case  1: RC_VEC_SHRC(dstv, srcv,  1); break;
#endif
#if RC_VEC_SIZE > 2
        case  2: RC_VEC_SHRC(dstv, srcv,  2); break;
        case  3: RC_VEC_SHRC(dstv, srcv,  3); break;
#endif
#if RC_VEC_SIZE > 4
        case  4: RC_VEC_SHRC(dstv, srcv,  4); break;
        case  5: RC_VEC_SHRC(dstv, srcv,  5); break;
        case  6: RC_VEC_SHRC(dstv, srcv,  6); break;
        case  7: RC_VEC_SHRC(dstv, srcv,  7); break;
#endif
#if RC_VEC_SIZE > 8
        case  8: RC_VEC_SHRC(dstv, srcv,  8); break;
        case  9: RC_VEC_SHRC(dstv, srcv,  9); break;
        case 10: RC_VEC_SHRC(dstv, srcv, 10); break;
        case 11: RC_VEC_SHRC(dstv, srcv, 11); break;
        case 12: RC_VEC_SHRC(dstv, srcv, 12); break;
        case 13: RC_VEC_SHRC(dstv, srcv, 13); break;
        case 14: RC_VEC_SHRC(dstv, srcv, 14); break;
        case 15: RC_VEC_SHRC(dstv, srcv, 15); break;
#endif
    }
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_shrc NULL
#endif

#ifdef RC_VEC_ALIGNC
static int
RC_TEST_VEC_FUNC(alignc)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int bytes)
{
    rc_vec_t dstv, srcv1, srcv2;
    RC_VEC_DECLARE();
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_ZERO(dstv);
    switch (bytes) {
#if RC_VEC_SIZE > 1
        case  1: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  1); break;
#endif
#if RC_VEC_SIZE > 2
        case  2: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  2); break;
        case  3: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  3); break;
#endif
#if RC_VEC_SIZE > 4
        case  4: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  4); break;
        case  5: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  5); break;
        case  6: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  6); break;
        case  7: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  7); break;
#endif
#if RC_VEC_SIZE > 8
        case  8: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  8); break;
        case  9: RC_VEC_ALIGNC(dstv, srcv1, srcv2,  9); break;
        case 10: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 10); break;
        case 11: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 11); break;
        case 12: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 12); break;
        case 13: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 13); break;
        case 14: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 14); break;
        case 15: RC_VEC_ALIGNC(dstv, srcv1, srcv2, 15); break;
#endif
    }
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_alignc NULL
#endif

#ifdef RC_VEC_PACK
RC_TEST_BINOP_FUNCTION(PACK, pack)
#else
#define rc_test_vec_pack NULL
#endif


/*
 * -------------------------------------------------------------
 *  Arithmetic operations
 * -------------------------------------------------------------
 */

#ifdef RC_VEC_SPLAT
static int
RC_TEST_VEC_FUNC(splat)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int val)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();
    (void)src1;
    (void)src2;
    RC_VEC_SPLAT(vec, val);
    RC_VEC_STORE(dst, vec);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_splat NULL
#endif

#ifdef RC_VEC_ABS
RC_TEST_UNOP_FUNCTION(ABS, abs)
#else
#define rc_test_vec_abs NULL
#endif

#ifdef RC_VEC_ADDS
RC_TEST_BINOP_FUNCTION(ADDS, adds)
#else
#define rc_test_vec_adds NULL
#endif

#ifdef RC_VEC_ADD16
RC_TEST_BINOP_FUNCTION(ADD16, add16)
#else
#define rc_test_vec_add16 NULL
#endif

#ifdef RC_VEC_SUB16
RC_TEST_BINOP_FUNCTION(SUB16, sub16)
#else
#define rc_test_vec_sub16 NULL
#endif

#ifdef RC_VEC_ADD32
RC_TEST_BINOP_FUNCTION(ADD32, add32)
#else
#define rc_test_vec_add32 NULL
#endif

#ifdef RC_VEC_SUB32
RC_TEST_BINOP_FUNCTION(SUB32, sub32)
#else
#define rc_test_vec_sub32 NULL
#endif

#ifdef RC_VEC_AVGT
RC_TEST_BINOP_FUNCTION(AVGT, avgt)
#else
#define rc_test_vec_avgt NULL
#endif

#ifdef RC_VEC_AVGR
RC_TEST_BINOP_FUNCTION(AVGR, avgr)
#else
#define rc_test_vec_avgr NULL
#endif

#ifdef RC_VEC_AVGZ
RC_TEST_BINOP_FUNCTION(AVGZ, avgz)
#else
#define rc_test_vec_avgz NULL
#endif


#ifdef RC_VEC_SUBS
RC_TEST_BINOP_FUNCTION(SUBS, subs)
#else
#define rc_test_vec_subs NULL
#endif

#ifdef RC_VEC_SUBA
RC_TEST_BINOP_FUNCTION(SUBA, suba)
#else
#define rc_test_vec_suba NULL
#endif

#ifdef RC_VEC_SUBHT
RC_TEST_BINOP_FUNCTION(SUBHT, subht)
#else
#define rc_test_vec_subht NULL
#endif

#ifdef RC_VEC_SUBHR
RC_TEST_BINOP_FUNCTION(SUBHR, subhr)
#else
#define rc_test_vec_subhr NULL
#endif

#ifdef RC_VEC_CMPGT
static int
RC_TEST_VEC_FUNC(cmpgt)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int val)
{
    rc_vec_t dstv, srcv1, srcv2;
    RC_VEC_DECLARE();
    (void)val;
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_CMPGT(dstv, srcv1, srcv2);
    RC_VEC_SPLAT(srcv1, 0x80);
    RC_VEC_AND(dstv, dstv, srcv1);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_cmpgt NULL
#endif

#ifdef RC_VEC_CMPGE
static int
RC_TEST_VEC_FUNC(cmpge)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int val)
{
    rc_vec_t dstv, srcv1, srcv2;
    RC_VEC_DECLARE();
    (void)val;
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_CMPGE(dstv, srcv1, srcv2);
    RC_VEC_SPLAT(srcv1, 0x80);
    RC_VEC_AND(dstv, dstv, srcv1);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_cmpge NULL
#endif

#ifdef RC_VEC_MIN
RC_TEST_BINOP_FUNCTION(MIN, min)
#else
#define rc_test_vec_min NULL
#endif

#ifdef RC_VEC_MAX
RC_TEST_BINOP_FUNCTION(MAX, max)
#else
#define rc_test_vec_max NULL
#endif

#ifdef RC_VEC_LERP
static int
RC_TEST_VEC_FUNC(lerp)(uint8_t *dst, const uint8_t *src1,
                       const uint8_t *src2, int alpha)
{
    rc_vec_t dstv, srcv1, srcv2;
    rc_vec_t blendv;
    RC_VEC_DECLARE();
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_BLEND(blendv, alpha);
    RC_VEC_LERP(dstv, srcv1, srcv2, alpha, blendv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_lerp NULL
#endif

#ifdef RC_VEC_LERPZ
static int
RC_TEST_VEC_FUNC(lerpz)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int alpha)
{
    rc_vec_t dstv, srcv1, srcv2;
    rc_vec_t blendv;
    RC_VEC_DECLARE();
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_BLENDZ(blendv, alpha);
    RC_VEC_LERPZ(dstv, srcv1, srcv2, alpha, blendv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_lerpz NULL
#endif

#ifdef RC_VEC_LERPN
static int
RC_TEST_VEC_FUNC(lerpn)(uint8_t *dst, const uint8_t *src1,
                        const uint8_t *src2, int alpha)
{
    rc_vec_t dstv, srcv1, srcv2;
    rc_vec_t blendv;
    RC_VEC_DECLARE();
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_BLENDN(blendv, alpha);
    RC_VEC_LERPN(dstv, srcv1, srcv2, alpha, blendv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_lerpn NULL
#endif


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

#ifdef RC_VEC_GETMASKW
static int
RC_TEST_VEC_FUNC(getmaskw)(uint8_t *dst, const uint8_t *src1,
                           const uint8_t *src2, int val)
{
    rc_vec_t srcv;
    int      mask;
    RC_VEC_DECLARE();
    (void)dst;
    (void)src2;
    (void)val;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_GETMASKW(mask, srcv);
    RC_VEC_CLEANUP();
    return mask;
}
#else
#define rc_test_vec_getmaskw NULL
#endif

#ifdef RC_VEC_GETMASKV
static int
RC_TEST_VEC_FUNC(getmaskv)(uint8_t *dst, const uint8_t *src1,
                           const uint8_t *src2, int val)
{
    rc_vec_t srcv, dstv, maskv;
    RC_VEC_DECLARE();
    (void)src2;
    (void)val;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_GETMASKV(dstv, srcv);
    RC_VEC_SPLAT(maskv, 0xff);
    RC_VEC_SHLC(maskv, maskv, RC_VEC_SIZE - (RC_VEC_SIZE + 7) / 8);
    RC_VEC_AND(dstv, dstv, maskv);
    RC_VEC_STORE(dst, dstv);
    RC_VEC_CLEANUP();
    return 0;
}
#else
#define rc_test_vec_getmaskv NULL
#endif

#ifdef RC_VEC_SETMASKV
RC_TEST_UNOP_FUNCTION(SETMASKV, setmaskv)
#else
#define rc_test_vec_setmaskv NULL
#endif


/*
 * -------------------------------------------------------------
 *  Type conversions
 * -------------------------------------------------------------
 */

#ifdef RC_VEC_8S16
RC_TEST_CONV_FUNCTION(l, _8S16)
RC_TEST_CONV_FUNCTION(r, _8S16)
#else
#define rc_test_vec_l_8S16 NULL
#define rc_test_vec_r_8S16 NULL
#endif

#ifdef RC_VEC_8U16
RC_TEST_CONV_FUNCTION(l, _8U16)
RC_TEST_CONV_FUNCTION(r, _8U16)
#else
#define rc_test_vec_l_8U16 NULL
#define rc_test_vec_r_8U16 NULL
#endif

#ifdef RC_VEC_16S32
RC_TEST_CONV_FUNCTION(l, _16S32)
RC_TEST_CONV_FUNCTION(r, _16S32)
#else
#define rc_test_vec_l_16S32 NULL
#define rc_test_vec_r_16S32 NULL
#endif

#ifdef RC_VEC_16U32
RC_TEST_CONV_FUNCTION(l, _16U32)
RC_TEST_CONV_FUNCTION(r, _16U32)
#else
#define rc_test_vec_l_16U32 NULL
#define rc_test_vec_r_16U32 NULL
#endif


/*
 * -------------------------------------------------------------
 *  Reductions
 * -------------------------------------------------------------
 */

#if defined RC_VEC_CNTN && defined RC_VEC_CNTV && defined RC_VEC_CNTR
static int
RC_TEST_VEC_FUNC(cnt)(uint8_t *dst, const uint8_t *src1,
                      const uint8_t *src2, int val)
{
    rc_vec_t srcv, accv;
    int      sum, k;
    RC_VEC_DECLARE();
    (void)dst;
    (void)src2;
    (void)val;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_ZERO(accv);
    for (k = 0; k < RC_VEC_CNTN; k++) {
        RC_VEC_CNTV(accv, srcv);
    }
    RC_VEC_CNTR(sum, accv);
    RC_VEC_CLEANUP();
    return sum;
}
#else
#define rc_test_vec_cnt NULL
#endif

#if defined RC_VEC_SUMN && defined RC_VEC_SUMV && defined RC_VEC_SUMR
static int
RC_TEST_VEC_FUNC(sum)(uint8_t *dst, const uint8_t *src1,
                      const uint8_t *src2, int val)
{
    rc_vec_t srcv, accv;
    int      sum, k;
    RC_VEC_DECLARE();
    (void)dst;
    (void)src2;
    (void)val;
    RC_VEC_LOAD(srcv, src1);
    RC_VEC_ZERO(accv);
    for (k = 0; k < RC_VEC_SUMN; k++) {
        RC_VEC_SUMV(accv, srcv);
    }
    RC_VEC_SUMR(sum, accv);
    RC_VEC_CLEANUP();
    return sum;
}
#else
#define rc_test_vec_sum NULL
#endif

#if defined RC_VEC_MACN && defined RC_VEC_MACV && defined RC_VEC_MACR
static int
RC_TEST_VEC_FUNC(mac)(uint8_t *dst, const uint8_t *src1,
                      const uint8_t *src2, int val)
{
    rc_vec_t srcv1, srcv2, accv;
    int      sum, k;
    RC_VEC_DECLARE();
    (void)dst;
    (void)val;
    RC_VEC_LOAD(srcv1, src1);
    RC_VEC_LOAD(srcv2, src2);
    RC_VEC_ZERO(accv);
    for (k = 0; k < RC_VEC_MACN; k++) {
        RC_VEC_MACV(accv, srcv1, srcv2);
    }
    RC_VEC_MACR(sum, accv);
    RC_VEC_CLEANUP();
    return sum;
}
#else
#define rc_test_vec_mac NULL
#endif
