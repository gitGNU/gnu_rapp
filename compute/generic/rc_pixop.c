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
 *  @file   rc_pixop.c
 *  @brief  RAPP Compute layer pixelwise operations, generic implementation.
 */

#include <string.h>       /* memset(), memcpy() */
#include <stdlib.h>       /* abs()              */
#include "rc_impl_cfg.h"  /* Implementation cfg */
#include "rc_util.h"      /* MIN(), MAX()       */
#include "rc_pixop.h"     /* Exported API       */


/*
 * -------------------------------------------------------------
 *  Pixel operation macros
 * -------------------------------------------------------------
 */

/**
 *  Negate value.
 */
#define RC_PIXOP_NOT(op, arg1, arg2) \
    ((op) = ~(op))

/**
 *  Flip sign bit.
 */
#define RC_PIXOP_FLIP(op, arg1, arg2) \
    ((op) ^= 0x80)

/**
 *  Lookup-table transformation.
 */
#define RC_PIXOP_LUT(op, lut, arg) \
    ((op) = (lut)[(op)])

/**
 *  Average value.
 */
#define RC_PIXOP_AVG(op1, op2, arg) \
    ((op1) = ((op1) + (op2) + 1) >> 1)

/**
 *  Saturated addition.
 */
#define RC_PIXOP_ADDS(op1, op2, arg) \
do {                                 \
    int sum = (op1) + (op2);         \
    (op1)   = MIN(sum, 0xff);        \
} while (0)

/**
 *  Saturated subtraction.
 */
#define RC_PIXOP_SUBS(op1, op2, arg) \
do {                                 \
    int diff = (op1) - (op2);        \
    (op1)    = MAX(diff, 0);         \
} while (0)

/**
 *  Halved subtraction.
 */
#define RC_PIXOP_SUBH(op1, op2, arg) \
    ((op1) = ((op1) - (op2) + 0x100) >> 1)

/**
 *  Absolute-value subtraction.
 */
#define RC_PIXOP_SUBA(op1, op2, arg) \
    ((op1) = abs((op1) - (op2)))

/**
 *  Linear interpolation.
 */
#define RC_PIXOP_LERP(op1, op2, blend8)               \
    ((op1) += ((blend8)*((op2) - (op1)) + 0x80) >> 8) \

/**
 *  Linear interpolation with update rounded away from zero.
 */
#define RC_PIXOP_LERPN(op1, op2, blend8)         \
do {                                             \
    int32_t upd = (blend8)*((op2) - (op1));      \
    (op1) += (upd + (~(upd >> 31) & 0xff)) >> 8; \
} while (0)

/**
 *  Linear interpolation with inverted second operand.
 */
#define RC_PIXOP_LERPI(op1, op2, blend8)                     \
    ((op1) += ((blend8)*(0xff - (op2) - (op1)) + 0x80) >> 8) \

/**
 *  L1 norm.
 */
#define RC_PIXOP_NORM(op1, op2, arg) \
    ((op1) = (rc_pixop_abs_lut[op1] + rc_pixop_abs_lut[op2] + 1) >> 1)


/*
 * -------------------------------------------------------------
 *  Single-operand template macros
 * -------------------------------------------------------------
 */

/**
 *  Single-operand pixel operation template.
 */
#define RC_PIXOP_TEMPLATE(buf, dim, width, height,                        \
                          pixop, arg1, arg2, unroll)                      \
do {                                                                      \
    if ((unroll) == 4) {                                                  \
        RC_PIXOP_TEMPLATE_X4(buf, dim, width, height, pixop, arg1, arg2); \
    }                                                                     \
    else if ((unroll) == 2) {                                             \
        RC_PIXOP_TEMPLATE_X2(buf, dim, width, height, pixop, arg1, arg2); \
    }                                                                     \
    else {                                                                \
        RC_PIXOP_TEMPLATE_X1(buf, dim, width, height, pixop, arg1, arg2); \
    }                                                                     \
} while (0)

/**
 *  Single-operand per element pixel operation template.
 *  No unrolling.
 */
#define RC_PIXOP_TEMPLATE_X1(buf, dim, width, height, pixop, arg1, arg2) \
do {                                                                     \
    int y;                                                               \
    /* Process all rows */                                               \
    for (y = 0; y < (height); y++) {                                     \
        int i = y*(dim);                                                 \
        int x;                                                           \
        /* Perform operation */                                          \
        for (x = 0; x < (width); x++, i++) {                             \
            pixop(buf[i], arg1, arg2);                                   \
        }                                                                \
    }                                                                    \
} while (0)

/**
 *  Single-operand pixel operation template.
 *  Unrolled two times.
 */
#define RC_PIXOP_TEMPLATE_X2(buf, dim, width, height, pixop, arg1, arg2) \
do {                                                                     \
    int y;                                                               \
    /* Process all rows */                                               \
    for (y = 0; y < (height); y++) {                                     \
        int i = y*(dim);                                                 \
        int x;                                                           \
        /* Perform unrolled operation */                                 \
        for (x = 0; x < (width); x += 2) {                               \
            pixop(buf[i], arg1, arg2); i++;                              \
            pixop(buf[i], arg1, arg2); i++;                              \
        }                                                                \
    }                                                                    \
} while (0)

/**
 *  Single-operand pixel operation template.
 *  Unrolled four times.
 */
#define RC_PIXOP_TEMPLATE_X4(buf, dim, width, height, pixop, arg1, arg2) \
do {                                                                     \
    int y;                                                               \
    /* Process all rows */                                               \
    for (y = 0; y < (height); y++) {                                     \
        int i = y*(dim);                                                 \
        int x;                                                           \
        /* Perform unrolled operation */                                 \
        for (x = 0; x < (width); x += 4) {                               \
            pixop(buf[i], arg1, arg2); i++;                              \
            pixop(buf[i], arg1, arg2); i++;                              \
            pixop(buf[i], arg1, arg2); i++;                              \
            pixop(buf[i], arg1, arg2); i++;                              \
        }                                                                \
    }                                                                    \
} while (0)


/*
 * -------------------------------------------------------------
 *  Double-operand template macros
 * -------------------------------------------------------------
 */

/**
 *  Double-operand pixel operation template.
 */
#define RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim,         \
                           width, height, pixop, arg, unroll)  \
do {                                                           \
    if ((unroll) == 4) {                                       \
        RC_PIXOP_TEMPLATE2_X4(dst, dst_dim, src, src_dim,      \
                                width, height, pixop, arg);    \
    }                                                          \
    else if ((unroll) == 2) {                                  \
        RC_PIXOP_TEMPLATE2_X2(dst, dst_dim, src, src_dim,      \
                                width, height, pixop, arg);    \
    }                                                          \
    else {                                                     \
        RC_PIXOP_TEMPLATE2_X1(dst, dst_dim, src, src_dim,      \
                                width, height, pixop, arg);    \
    }                                                          \
} while (0)

/**
 *  Double-operand per element pixel operation template.
 *  No unrolling.
 */
#define RC_PIXOP_TEMPLATE2_X1(dst, dst_dim, src, src_dim, \
                              width, height, pixop, arg)  \
do {                                                      \
    int y;                                                \
    /* Process all rows */                                \
    for (y = 0; y < (height); y++) {                      \
        int i = y*(src_dim);                              \
        int j = y*(dst_dim);                              \
        int x;                                            \
        /* Perform operation */                           \
        for (x = 0; x < (width); x++, i++, j++)  {        \
            pixop(dst[j], src[i], arg);                   \
        }                                                 \
    }                                                     \
} while (0)

/**
 *  Double-operand pixel operation template.
 *  Unrolled two times.
 */
#define RC_PIXOP_TEMPLATE2_X2(dst, dst_dim, src, src_dim, \
                              width, height, pixop, arg)  \
do {                                                      \
    int y;                                                \
    /* Process all rows */                                \
    for (y = 0; y < (height); y++) {                      \
        int i = y*(src_dim);                              \
        int j = y*(dst_dim);                              \
        int x;                                            \
        /* Perform unrolled operation */                  \
        for (x = 0; x < (width); x += 2) {                \
            pixop(dst[j], src[i], arg); i++, j++;         \
            pixop(dst[j], src[i], arg); i++, j++;         \
        }                                                 \
    }                                                     \
} while (0)

/**
 *  Double-operand pixel operation template.
 *  Unrolled four times.
 */
#define RC_PIXOP_TEMPLATE2_X4(dst, dst_dim, src, src_dim, \
                              width, height, pixop, arg)  \
do {                                                      \
    int y;                                                \
    /* Process all rows */                                \
    for (y = 0; y < (height); y++) {                      \
        int i = y*(src_dim);                              \
        int j = y*(dst_dim);                              \
        int x;                                            \
        /* Perform unrolled operation */                  \
        for (x = 0; x < (width); x += 4) {                \
            pixop(dst[j], src[i], arg); i++, j++;         \
            pixop(dst[j], src[i], arg); i++, j++;         \
            pixop(dst[j], src[i], arg); i++, j++;         \
            pixop(dst[j], src[i], arg); i++, j++;         \
        }                                                 \
    }                                                     \
} while (0)


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  Signed with bias to absolute value conversion table.
 */
#if RC_IMPL(rc_pixop_abs_u8,  0) || \
    RC_IMPL(rc_pixop_norm_u8, 1)
static const uint8_t rc_pixop_abs_lut[256] =
    {0xff, 0xfe, 0xfc, 0xfa, 0xf8, 0xf6, 0xf4, 0xf2,
     0xf0, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe2,
     0xe0, 0xde, 0xdc, 0xda, 0xd8, 0xd6, 0xd4, 0xd2,
     0xd0, 0xce, 0xcc, 0xca, 0xc8, 0xc6, 0xc4, 0xc2,
     0xc0, 0xbe, 0xbc, 0xba, 0xb8, 0xb6, 0xb4, 0xb2,
     0xb0, 0xae, 0xac, 0xaa, 0xa8, 0xa6, 0xa4, 0xa2,
     0xa0, 0x9e, 0x9c, 0x9a, 0x98, 0x96, 0x94, 0x92,
     0x90, 0x8e, 0x8c, 0x8a, 0x88, 0x86, 0x84, 0x82,
     0x80, 0x7e, 0x7c, 0x7a, 0x78, 0x76, 0x74, 0x72,
     0x70, 0x6e, 0x6c, 0x6a, 0x68, 0x66, 0x64, 0x62,
     0x60, 0x5e, 0x5c, 0x5a, 0x58, 0x56, 0x54, 0x52,
     0x50, 0x4e, 0x4c, 0x4a, 0x48, 0x46, 0x44, 0x42,
     0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32,
     0x30, 0x2e, 0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22,
     0x20, 0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x14, 0x12,
     0x10, 0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x02,
     0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e,
     0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,
     0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e,
     0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,
     0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e,
     0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
     0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e,
     0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,
     0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e,
     0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,
     0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae,
     0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
     0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce,
     0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
     0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee,
     0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe};
#endif


/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Set all pixels to a constant value.
 */
#if RC_IMPL(rc_pixop_set_u8, 0)
void
rc_pixop_set_u8(uint8_t *buf, int dim, int width, int height, unsigned value)
{
    int y, i;

    for (y = 0, i = 0; y < height; y++, i += dim) {
        memset(&buf[i], value, width);
    }
}
#endif


/**
 *  Negate all pixels.
 */
#if RC_IMPL(rc_pixop_not_u8, 1)
void
rc_pixop_not_u8(uint8_t *buf, int dim, int width, int height)
{
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_NOT, 0, 0,
                      RC_UNROLL(rc_pixop_not_u8));
}
#endif

/**
 *  Flip the sign bit.
 */
#if RC_IMPL(rc_pixop_flip_u8, 1)
void
rc_pixop_flip_u8(uint8_t *buf, int dim, int width, int height)
{
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_FLIP, 0, 0,
                      RC_UNROLL(rc_pixop_flip_u8));
}
#endif


/**
 *  Lookup-table transformation.
 */
#if RC_IMPL(rc_pixop_lut_u8, 1)
void
rc_pixop_lut_u8(uint8_t *restrict buf, int dim,
                int width, int height, const uint8_t *restrict lut)
{
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_LUT, lut, 0,
                      RC_UNROLL(rc_pixop_lut_u8));
}
#endif


/**
 *  Absolute value.
 */
#if RC_IMPL(rc_pixop_abs_u8, 0)
void
rc_pixop_abs_u8(uint8_t *buf, int dim, int width, int height)
{
    /* Use a LUT to compute the absolute value */
    rc_pixop_lut_u8(buf, dim, width, height, rc_pixop_abs_lut);
}
#endif


/**
 *  Add signed constant.
 */
#if RC_IMPL(rc_pixop_addc_u8, 1)
void
rc_pixop_addc_u8(uint8_t *buf, int dim, int width, int height, int value)
{
    if (value > 0) {
        /* Positive value - use ADDS */
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_ADDS, value, 0,
                          RC_UNROLL(rc_pixop_addc_u8));
    }
    else if (value < 0) {
        /* Negative value - use SUBS */
        value = -value;
        RC_PIXOP_TEMPLATE(buf, dim, width, height,
                          RC_PIXOP_SUBS, value, 0,
                          RC_UNROLL(rc_pixop_addc_u8));
    }
}
#endif


/**
 *  Linear interpolation with a constant.
 */
#if RC_IMPL(rc_pixop_lerpc_u8, 1)
void
rc_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                  int height, unsigned value, unsigned alpha8)
{
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_LERP, value, alpha8,
                      RC_UNROLL(rc_pixop_lerpc_u8));
}
#endif


/**
 *  Linear interpolation with a constant, rounded towards the constant value.
 */
#if RC_IMPL(rc_pixop_lerpnc_u8, 1)
void
rc_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                   int height, unsigned value, unsigned alpha8)
{
    RC_PIXOP_TEMPLATE(buf, dim, width, height,
                      RC_PIXOP_LERPN, value, alpha8,
                      RC_UNROLL(rc_pixop_lerpnc_u8));
}
#endif


/*
 * -------------------------------------------------------------
 *  Double-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Saturated addition.
 */
#if RC_IMPL(rc_pixop_add_u8, 1)
void
rc_pixop_add_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_ADDS, 0,
                       RC_UNROLL(rc_pixop_add_u8));
}
#endif


/**
 *  Average value.
 */
#if RC_IMPL(rc_pixop_avg_u8, 1)
void
rc_pixop_avg_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_AVG, 0,
                       RC_UNROLL(rc_pixop_avg_u8));
}
#endif


/**
 *  Saturated subtraction.
 */
#if RC_IMPL(rc_pixop_sub_u8, 1)
void
rc_pixop_sub_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_SUBS, 0,
                       RC_UNROLL(rc_pixop_sub_u8));
}
#endif


/**
 *  Halved subtraction.
 */
#if RC_IMPL(rc_pixop_subh_u8, 1)
void
rc_pixop_subh_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_SUBH, 0,
                       RC_UNROLL(rc_pixop_subh_u8));
}
#endif


/**
 *  Absolute-value subtraction.
 */
#if RC_IMPL(rc_pixop_suba_u8, 1)
void
rc_pixop_suba_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                         RC_PIXOP_SUBA, 0,
                         RC_UNROLL(rc_pixop_suba_u8));
}
#endif


/**
 *  Linear interpolation.
 */
#if RC_IMPL(rc_pixop_lerp_u8, 1)
void
rc_pixop_lerp_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height, unsigned alpha8)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_LERP, alpha8,
                       RC_UNROLL(rc_pixop_lerp_u8));
}
#endif


/**
 *  Linear interpolation with non-zero update, i.e. rounded towards src.
 */
#if RC_IMPL(rc_pixop_lerpn_u8, 1)
void
rc_pixop_lerpn_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, unsigned alpha8)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_LERPN, alpha8,
                       RC_UNROLL(rc_pixop_lerpn_u8));
}
#endif


/**
 *  Linear interpolation with inverted second operand.
 */
#if RC_IMPL(rc_pixop_lerpi_u8, 1)
void
rc_pixop_lerpi_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, unsigned alpha8)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_LERPI, alpha8,
                       RC_UNROLL(rc_pixop_lerpn_u8));
}
#endif


/**
 *  L1 norm.
 */
#if RC_IMPL(rc_pixop_norm_u8, 1)
void
rc_pixop_norm_u8(uint8_t *restrict dst, int dst_dim,
                 const uint8_t *restrict src, int src_dim,
                 int width, int height)
{
    RC_PIXOP_TEMPLATE2(dst, dst_dim, src, src_dim, width, height,
                       RC_PIXOP_NORM, 0,
                       RC_UNROLL(rc_pixop_norm_u8));
}
#endif
