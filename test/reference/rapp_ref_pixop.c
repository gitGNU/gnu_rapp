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
 *  @file   rapp_ref_pixop.c
 *  @brief  RAPP pixelwise operations, reference implementation.
 */

#include <stdlib.h>         /* abs()         */
#include "rapp_ref_pixop.h" /* Reference API */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#undef  MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef  CLAMP
#define CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_ref_pixop_not(int val);

static int
rapp_ref_pixop_flip(int val);

static int
rapp_ref_pixop_lut(int val, const uint8_t *lut);

static int
rapp_ref_pixop_abs(int val);

static int
rapp_ref_pixop_copy(int v1, int v2);

static int
rapp_ref_pixop_add(int v1, int v2);

static int
rapp_ref_pixop_avg(int v1, int v2);

static int
rapp_ref_pixop_sub(int v1, int v2);

static int
rapp_ref_pixop_subh(int v1, int v2);

static int
rapp_ref_pixop_suba(int v1, int v2);

static int
rapp_ref_pixop_lerp(int v1, int v2, int alpha8);

static int
rapp_ref_pixop_lerpn(int v1, int v2, int alpha8);

static int
rapp_ref_pixop_lerpi(int v1, int v2, int alpha8);

static int
rapp_ref_pixop_norm(int v1, int v2);

static void
rapp_ref_pixop_driver(uint8_t *buf, int dim, int width, int height,
                      int (*op)(), intptr_t arg1, int arg2);
static void
rapp_ref_pixop_driver2(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, int (*op)(), int arg);

/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

void
rapp_ref_pixop_set_u8(uint8_t *buf, int dim, int width,
                      int height, unsigned value)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_copy, value, 0);
}

void
rapp_ref_pixop_not_u8(uint8_t *buf, int dim, int width, int height)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_not, 0, 0);
}

void
rapp_ref_pixop_flip_u8(uint8_t *buf, int dim, int width, int height)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_flip, 0, 0);
}

void
rapp_ref_pixop_lut_u8(uint8_t *buf, int dim,
                      int width, int height, const uint8_t *lut)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_lut, (intptr_t)lut, 0);
}

void
rapp_ref_pixop_abs_u8(uint8_t *buf, int dim, int width, int height)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_abs, 0, 0);
}

void
rapp_ref_pixop_addc_u8(uint8_t *buf, int dim, int width, int height, int value)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_add, value, 0);
}

void
rapp_ref_pixop_lerpc_u8(uint8_t *buf, int dim, int width,
                        int height, unsigned value, unsigned alpha8)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_lerp, value, alpha8);
}

void
rapp_ref_pixop_lerpnc_u8(uint8_t *buf, int dim, int width,
                         int height, unsigned value, unsigned alpha8)
{
    rapp_ref_pixop_driver(buf, dim, width, height,
                          &rapp_ref_pixop_lerpn, value, alpha8);
}


/*
 * -------------------------------------------------------------
 *  Double-operand functions
 * -------------------------------------------------------------
 */

void
rapp_ref_pixop_copy_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_copy, 0);
}

void
rapp_ref_pixop_add_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_add, 0);
}

void
rapp_ref_pixop_avg_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_avg, 0);
}

void
rapp_ref_pixop_sub_u8(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_sub, 0);
}

void
rapp_ref_pixop_subh_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_subh, 0);
}

void
rapp_ref_pixop_suba_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_suba, 0);
}

void
rapp_ref_pixop_lerp_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, unsigned alpha8)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_lerp, alpha8);
}

void
rapp_ref_pixop_lerpn_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, unsigned alpha8)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_lerpn, alpha8);
}

void
rapp_ref_pixop_lerpi_u8(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height, unsigned alpha8)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_lerpi, alpha8);
}

void
rapp_ref_pixop_norm_u8(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height)
{
    rapp_ref_pixop_driver2(dst, dst_dim, src, src_dim, width, height,
                           &rapp_ref_pixop_norm, 0);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rapp_ref_pixop_not(int val)
{
    return 0xff - val;
}

static int
rapp_ref_pixop_flip(int val)
{
    return val ^ 0x80;
}

static int
rapp_ref_pixop_lut(int val, const uint8_t *lut)
{
    return lut[val];
}

static int
rapp_ref_pixop_abs(int val)
{
    return MIN(2*abs(val - 0x80), 0xff);
}

static int
rapp_ref_pixop_copy(int v1, int v2)
{
    (void)v1;
    return v2;
}

static int
rapp_ref_pixop_add(int v1, int v2)
{
    return CLAMP(v1 + v2, 0, 0xff);
}

static int
rapp_ref_pixop_avg(int v1, int v2)
{
    return (v1 + v2 + 1) / 2;
}

static int
rapp_ref_pixop_sub(int v1, int v2)
{
    return CLAMP(v1 - v2, 0, 0xff);
}

static int
rapp_ref_pixop_subh(int v1, int v2)
{
    return (v1 - v2 + 0x100) / 2;
}

static int
rapp_ref_pixop_suba(int v1, int v2)
{
    return abs(v1 - v2);
}

static int
rapp_ref_pixop_lerp(int v1, int v2, int alpha8)
{
    return v1 + (((long)alpha8*(v2 - v1) + 0x80) >> 8);
}

static int
rapp_ref_pixop_lerpn(int v1, int v2, int alpha8)
{
    long upd = (long)alpha8*(v2 - v1);
    return v1 + ((upd + (upd > 0 ? 0xff : 0)) >> 8);
}

static int
rapp_ref_pixop_lerpi(int v1, int v2, int alpha8)
{
    return v1 + (((long)alpha8*(0xff - v2 - v1) + 0x80) >> 8);
}

static int
rapp_ref_pixop_norm(int v1, int v2)
{
    return MIN(abs(v1 - 0x80) + abs(v2 - 0x80), 0xff);
}

static void
rapp_ref_pixop_driver(uint8_t *buf, int dim, int width, int height,
                      int (*op)(), intptr_t arg1, int arg2)
{
    int y;

    for (y = 0; y < height; y++) {
        int i = y*dim;
        int x;

        for (x = 0; x < width; x++, i++) {
            buf[i] = (*op)(buf[i], arg1, arg2);
        }
    }
}

static void
rapp_ref_pixop_driver2(uint8_t *dst, int dst_dim,
                       const uint8_t *src, int src_dim,
                       int width, int height, int (*op)(), int arg)
{
    int y;

    for (y = 0; y < height; y++) {
        int i = y*src_dim;
        int j = y*dst_dim;
        int x;

        for (x = 0; x < width; x++, i++, j++) {
            dst[j] = (*op)(dst[j], src[i], arg);
        }
    }
}
