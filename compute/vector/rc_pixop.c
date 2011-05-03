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
 *  @file   rc_pixop.c
 *  @brief  RAPP Compute layer pixelwise operations, vector implementation.
 */

#include "rc_impl_cfg.h" /* Implementation cfg  */
#include "rc_vector.h"   /* Vector operations   */
#include "rc_util.h"     /* RC_DIV_CEIL()       */
#include "rc_pixop.h"    /* Pixel operation API */


/*
 * -------------------------------------------------------------
 *  Pixel operation macros
 * -------------------------------------------------------------
 */

#define RC_PIXOP_COPY(vec1, vec2, arg1, arg2) \
    ((vec1) = (vec2))

#ifdef RC_VEC_NOT
#define RC_PIXOP_NOT(vec1, arg1, arg2, arg3) \
    RC_VEC_NOT(vec1, vec1)
#endif

#ifdef RC_VEC_XOR
#define RC_PIXOP_FLIP(vec1, msbv, arg2, arg3) \
    RC_VEC_XOR(vec1, vec1, msbv)
#endif

#ifdef RC_VEC_ABS
#define RC_PIXOP_ABS(vec, arg1, arg2, arg3) \
    RC_VEC_ABS(vec, vec)
#endif

#ifdef RC_VEC_AVGR
#define RC_PIXOP_AVG(vec1, vec2, arg1, arg2) \
    RC_VEC_AVGR(vec1, vec1, vec2)
#endif

#ifdef RC_VEC_ADDS
#define RC_PIXOP_ADDS(vec1, vec2, arg1, arg2) \
    RC_VEC_ADDS(vec1, vec1, vec2)
#endif

#ifdef RC_VEC_SUBS
#define RC_PIXOP_SUBS(vec1, vec2, arg1, arg2) \
    RC_VEC_SUBS(vec1, vec1, vec2)
#endif

#ifdef RC_VEC_SUBHR
#define RC_PIXOP_SUBH(vec1, vec2, arg1, arg2) \
    RC_VEC_SUBHR(vec1, vec1, vec2)
#endif

#ifdef RC_VEC_SUBA
#define RC_PIXOP_SUBA(vec1, vec2, arg1, arg2) \
    RC_VEC_SUBA(vec1, vec1, vec2)
#endif

#if defined RC_VEC_LERP && defined RC_VEC_BLEND
#define RC_PIXOP_LERP_NOR(vec1, vec2, blend8, blendv) \
    RC_VEC_LERP(vec1, vec1, vec2, blend8, blendv)
#define RC_PIXOP_BLEND_NOR RC_VEC_BLEND

#define RC_PIXOP_LERP_REV(vec1, vec2, blend8, blendv) \
    RC_VEC_LERP(vec1, vec2, vec1, blend8, blendv)
#define RC_PIXOP_BLEND_REV RC_VEC_BLEND
#endif

#if defined RC_VEC_LERPN && defined RC_VEC_BLENDN
#define RC_PIXOP_LERPN_NOR(vec1, vec2, blend8, blendv) \
    RC_VEC_LERPN(vec1, vec1, vec2, blend8, blendv)
#define RC_PIXOP_BLENDN_NOR RC_VEC_BLENDN
#endif

#if defined RC_VEC_LERPZ && defined RC_VEC_BLENDZ
#define RC_PIXOP_LERPN_REV(vec1, vec2, blend8, blendv) \
    RC_VEC_LERPZ(vec1, vec2, vec1, blend8, blendv)
#define RC_PIXOP_BLENDN_REV RC_VEC_BLENDZ
#endif

#if defined RC_VEC_NOT && defined RC_VEC_LERP && defined RC_VEC_BLEND
#define RC_PIXOP_LERPI_NOR(vec1, vec2, blend8, blendv) \
do {                                                   \
    RC_VEC_NOT(vec2, vec2);                            \
    RC_VEC_LERP(vec1, vec1, vec2, blend8, blendv);     \
} while (0)
#define RC_PIXOP_BLENDI_NOR RC_VEC_BLEND

#define RC_PIXOP_LERPI_REV(vec1, vec2, blend8, blendv) \
do {                                                   \
    RC_VEC_NOT(vec2, vec2);                            \
    RC_VEC_LERP(vec1, vec2, vec1, blend8, blendv);     \
} while (0)
#define RC_PIXOP_BLENDI_REV RC_VEC_BLEND
#endif

#ifdef RC_VEC_AVGZ
#define RC_PIXOP_AVGN(vec1, vec2, arg1, arg2) \
    RC_VEC_AVGZ(vec1, vec2, vec1)
#endif

#if defined RC_VEC_ABS && defined RC_VEC_AVGR
#define RC_PIXOP_NORM(vec1, vec2, arg1, arg2) \
do {                                          \
    RC_VEC_ABS(vec1, vec1);                   \
    RC_VEC_ABS(vec2, vec2);                   \
    RC_VEC_AVGR(vec1, vec1, vec2);            \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  Template macros
 * -------------------------------------------------------------
 */

/**
 *  Single-operand template.
 */
#define RC_PIXOP_TEMPLATE(buf, dim, width, height,              \
                          pixop, arg1, arg2, arg3, unroll)      \
do {                                                            \
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);                  \
    int len = tot / (unroll);                                   \
    int rem = tot % (unroll);                                   \
    int y;                                                      \
                                                                \
    /* Process all rows */                                      \
    for (y = 0; y < (height); y++) {                            \
        int i = y*(dim);                                        \
        int x;                                                  \
                                                                \
        /* Perform unrolled operation */                        \
        for (x = 0; x < len; x++) {                             \
            RC_PIXOP_ITER(buf, i, pixop, arg1, arg2, arg3);     \
            if ((unroll) >= 2) {                                \
                RC_PIXOP_ITER(buf, i, pixop, arg1, arg2, arg3); \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_PIXOP_ITER(buf, i, pixop, arg1, arg2, arg3); \
                RC_PIXOP_ITER(buf, i, pixop, arg1, arg2, arg3); \
            }                                                   \
        }                                                       \
        /* Handle the remaining vectors */                      \
        for (x = 0; x < rem; x++) {                             \
            RC_PIXOP_ITER(buf, i, pixop, arg1, arg2, arg3);     \
        }                                                       \
    }                                                           \
} while (0)

/**
 *  Double-operand template.
 */
#define RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim,               \
                           width, height, pixop, arg1, arg2, unroll) \
do {                                                                 \
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);                       \
    int len = tot / (unroll);                                        \
    int rem = tot % (unroll);                                        \
    int y;                                                           \
                                                                     \
    /* Process all rows */                                           \
    for (y = 0; y < (height); y++) {                                 \
        int i = y*(src_dim);                                         \
        int j = y*(dst_dim);                                         \
        int x;                                                       \
                                                                     \
        /* Perform unrolled operation */                             \
        for (x = 0; x < len; x++) {                                  \
            RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2);       \
            if ((unroll) >= 2) {                                     \
                RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2);   \
            }                                                        \
            if ((unroll) == 4) {                                     \
                RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2);   \
                RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2);   \
            }                                                        \
        }                                                            \
        /* Handle the remaining vectors */                           \
        for (x = 0; x < rem; x++) {                                  \
            RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2);       \
        }                                                            \
    }                                                                \
} while (0)

/**
 *  Single-operand operation iteration.
 */
#define RC_PIXOP_ITER(buf, pos, pixop, arg1, arg2, arg3) \
do {                                                     \
    rc_vec_t vec_;                                       \
    RC_VEC_LOAD(vec_, &(buf)[pos]);                      \
    pixop(vec_, arg1, arg2, arg3);                       \
    RC_VEC_STORE(&(buf)[pos], vec_);                     \
    (pos) += RC_VEC_SIZE;                                \
} while (0)

/**
 *  Double-operand iteration.
 */
#define RC_PIXOP_ITER2(dst, src, j, i, pixop, arg1, arg2) \
do {                                                      \
    rc_vec_t dv_, sv_;                                    \
    RC_VEC_LOAD(sv_, &(src)[i]);                          \
    RC_VEC_LOAD(dv_, &(dst)[j]);                          \
    pixop(dv_, sv_, arg1, arg2);                          \
    RC_VEC_STORE(&(dst)[j], dv_);                         \
    (i) += RC_VEC_SIZE;                                   \
    (j) += RC_VEC_SIZE;                                   \
} while (0)


/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Set all pixels to a constant value.
 */
#if    RC_IMPL(rc_pixop_set_u8, 1)
#ifdef RC_VEC_SPLAT
void
rc_pixop_set_u8(uint8_t *buf, int dim, int width, int height, unsigned value)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    RC_VEC_SPLAT(vec, value);
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_COPY, vec, 0, 0,
                      RC_UNROLL(rc_pixop_set_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Negate all pixels.
 */
#if    RC_IMPL(rc_pixop_not_u8, 1)
#ifdef RC_PIXOP_NOT
void
rc_pixop_not_u8(uint8_t *buf, int dim, int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_NOT, 0, 0, 0,
                      RC_UNROLL(rc_pixop_not_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Flip the sign bit.
 */
#if RC_IMPL(rc_pixop_flip_u8, 1)
#if defined RC_VEC_SPLAT && defined RC_PIXOP_FLIP
void
rc_pixop_flip_u8(uint8_t *buf, int dim, int width, int height)
{
    rc_vec_t msbv;
    RC_VEC_DECLARE();

    RC_VEC_SPLAT(msbv, 0x80);
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_FLIP, msbv, 0, 0,
                      RC_UNROLL(rc_pixop_flip_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Absolute value.
 */
#if    RC_IMPL(rc_pixop_abs_u8, 1)
#ifdef RC_PIXOP_ABS
void
rc_pixop_abs_u8(uint8_t *buf, int dim, int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_ABS, 0, 0, 0,
                      RC_UNROLL(rc_pixop_abs_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Add signed constant.
 */
#if RC_IMPL(rc_pixop_addc_u8, 1)
#if defined RC_VEC_SPLAT && defined RC_PIXOP_ADDS && defined RC_PIXOP_SUBS
void
rc_pixop_addc_u8(uint8_t *buf, int dim, int width, int height, int value)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    if (value > 0) {
        /* Positive value - use ADDS() */
        RC_VEC_SPLAT(vec, value);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_ADDS, vec, 0, 0,
                          RC_UNROLL(rc_pixop_addc_u8));
    }
    else if (value < 0) {
        /* Negative value - use SUBS() */
        RC_VEC_SPLAT(vec, -value);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_SUBS, vec, 0, 0,
                          RC_UNROLL(rc_pixop_addc_u8));
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Linear interpolation with a constant.
 */
#if RC_IMPL(rc_pixop_lerpc_u8, 1)
#if defined RC_VEC_SPLAT && defined RC_PIXOP_AVG && \
    defined RC_PIXOP_LERP_NOR && defined RC_PIXOP_LERP_REV
void
rc_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                  int height, unsigned value, unsigned alpha8)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    /* Vectorize argument */
    RC_VEC_SPLAT(vec, value);

    /* Perform operation */
    if (alpha8 == 0x80) {
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_AVG, vec, 0, 0,
                          RC_UNROLL(rc_pixop_lerpc_u8));
    }
    else if (alpha8 < 0x80) {
        rc_vec_t blendv;

        RC_PIXOP_BLEND_NOR(blendv, alpha8);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_LERP_NOR, vec, alpha8, blendv,
                          RC_UNROLL(rc_pixop_lerpc_u8));
    }
    else { /* alpha8 > 0x80 */
        rc_vec_t blendv;

        alpha8 = 0x100 - alpha8;
        RC_PIXOP_BLEND_REV(blendv, alpha8);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_LERP_REV, vec, alpha8, blendv,
                          RC_UNROLL(rc_pixop_lerpc_u8));
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Linear interpolation with a constant, rounded towards the constant value.
 */
#if RC_IMPL(rc_pixop_lerpnc_u8, 1)
#if defined RC_VEC_SPLAT && defined RC_PIXOP_AVGN && \
    defined RC_PIXOP_LERPN_NOR && defined RC_PIXOP_LERPN_REV
void
rc_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                   int height, unsigned value, unsigned alpha8)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    /* Vectorize argument */
    RC_VEC_SPLAT(vec, value);

    /* Perform operation */
    if (alpha8 == 0x80) {
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_AVGN, vec, 0, 0,
                          RC_UNROLL(rc_pixop_lerpnc_u8));
    }
    else if (alpha8 < 0x80) {
        rc_vec_t blendv;

        RC_PIXOP_BLENDN_NOR(blendv, alpha8);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_LERPN_NOR, vec, alpha8, blendv,
                          RC_UNROLL(rc_pixop_lerpnc_u8));
    }
    else { /* alpha8 > 0x80 */
        rc_vec_t blendv;

        alpha8 = 0x100 - alpha8;
        RC_PIXOP_BLENDN_REV(blendv, alpha8);
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_LERPN_REV, vec, alpha8, blendv,
                          RC_UNROLL(rc_pixop_lerpnc_u8));
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/*
 * -------------------------------------------------------------
 *  Double-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Saturated addition.
 */
#if    RC_IMPL(rc_pixop_add_u8, 1)
#ifdef RC_PIXOP_ADDS
void
rc_pixop_add_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_ADDS, 0, 0,
                       RC_UNROLL(rc_pixop_add_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Average value.
 */
#if    RC_IMPL(rc_pixop_avg_u8, 1)
#ifdef RC_PIXOP_AVG
void
rc_pixop_avg_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_AVG, 0, 0,
                       RC_UNROLL(rc_pixop_avg_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Saturated subtraction.
 */
#if    RC_IMPL(rc_pixop_sub_u8, 1)
#ifdef RC_PIXOP_SUBS
void
rc_pixop_sub_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_SUBS, 0, 0,
                       RC_UNROLL(rc_pixop_sub_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Halved subtraction.
 */
#if    RC_IMPL(rc_pixop_subh_u8, 1)
#ifdef RC_PIXOP_SUBH
void
rc_pixop_subh_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_SUBH, 0, 0,
                       RC_UNROLL(rc_pixop_subh_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Absolute-value subtraction.
 */
#if    RC_IMPL(rc_pixop_suba_u8, 1)
#ifdef RC_PIXOP_SUBA
void
rc_pixop_suba_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_SUBA, 0, 0,
                       RC_UNROLL(rc_pixop_suba_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Linear interpolation.
 */
#if RC_IMPL(rc_pixop_lerp_u8, 1)
#if defined RC_PIXOP_LERP_NOR && defined RC_PIXOP_LERP_REV
void
rc_pixop_lerp_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height, unsigned alpha8)
{
    if (alpha8 == 0x80) {
        rc_pixop_avg_u8(dst, dst_dim, src, src_dim, width, height);
    }
    else if (alpha8 < 0x80) {
        rc_vec_t blendv;
        RC_VEC_DECLARE();

        RC_PIXOP_BLEND_NOR(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERP_NOR, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerp_u8));

        RC_VEC_CLEANUP();
    }
    else { /* alpha8 > 0x80 */
        rc_vec_t blendv;
        RC_VEC_DECLARE();

        alpha8 = 0x100 - alpha8;
        RC_PIXOP_BLEND_REV(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERP_REV, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerp_u8));

        RC_VEC_CLEANUP();
    }
}
#endif
#endif


/**
 *  Linear interpolation with non-zero update, i.e. rounded towards src.
 */
#if RC_IMPL(rc_pixop_lerpn_u8, 1)
#if defined RC_PIXOP_AVGN && \
    defined RC_PIXOP_LERPN_NOR && defined RC_PIXOP_LERPN_REV
void
rc_pixop_lerpn_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, unsigned alpha8)
{
    RC_VEC_DECLARE();

    if (alpha8 == 0x80) {
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_AVGN, 0, 0,
                           RC_UNROLL(rc_pixop_lerpn_u8));
    }
    else if (alpha8 < 0x80) {
        rc_vec_t blendv;

        RC_VEC_BLENDN(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERPN_NOR, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerpn_u8));
    }
    else { /* alpha8 > 0x80 */
        rc_vec_t blendv;

        alpha8 = 0x100 - alpha8;
        RC_VEC_BLENDZ(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERPN_REV, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerpn_u8));
    }

    RC_VEC_CLEANUP();
}
#endif
#endif


/**
 *  Linear interpolation with inverted second operand.
 */
#if RC_IMPL(rc_pixop_lerpi_u8, 1)
#if defined RC_PIXOP_LERPI_NOR && defined RC_PIXOP_LERPI_REV
void
rc_pixop_lerpi_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height, unsigned alpha8)
{
    if (alpha8 == 0x80) {
        rc_pixop_subh_u8(dst, dst_dim, src, src_dim, width, height);
    }
    else if (alpha8 < 0x80) {
        rc_vec_t blendv;
        RC_VEC_DECLARE();

        RC_PIXOP_BLENDI_NOR(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERPI_NOR, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerpi_u8));

        RC_VEC_CLEANUP();
    }
    else { /* alpha8 > 0x80 */
        rc_vec_t blendv;
        RC_VEC_DECLARE();

        alpha8 = 0x100 - alpha8;
        RC_PIXOP_BLENDI_REV(blendv, alpha8);
        RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                           RC_PIXOP_LERPI_REV, alpha8, blendv,
                           RC_UNROLL(rc_pixop_lerpi_u8));

        RC_VEC_CLEANUP();
    }
}
#endif
#endif


/**
 *  L1 norm.
 */
#if RC_IMPL(rc_pixop_norm_u8, 1)
#if defined RC_PIXOP_NORM
void
rc_pixop_norm_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_VEC_DECLARE();

    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_NORM, 0, 0,
                       RC_UNROLL(rc_pixop_norm_u8));

    RC_VEC_CLEANUP();
}
#endif
#endif
