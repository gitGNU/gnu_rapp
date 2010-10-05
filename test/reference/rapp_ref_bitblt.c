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
 *  @file   rapp_ref_bitblt.c
 *  @brief  RAPP bitblt operations, reference implementation.
 */

#include "rapp.h"            /* RAPP API      */
#include "rapp_ref_bitblt.h" /* Reference API */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_ref_bitblt_copy(int op1, int op2);

static int
rapp_ref_bitblt_not(int op1, int op2);

static int
rapp_ref_bitblt_and(int op1, int op2);

static int
rapp_ref_bitblt_or(int op1, int op2);

static int
rapp_ref_bitblt_xor(int op1, int op2);

static int
rapp_ref_bitblt_nand(int op1, int op2);

static int
rapp_ref_bitblt_nor(int op1, int op2);

static int
rapp_ref_bitblt_xnor(int op1, int op2);

static int
rapp_ref_bitblt_andn(int op1, int op2);

static int
rapp_ref_bitblt_orn(int op1, int op2);

static int
rapp_ref_bitblt_nandn(int op1, int op2);

static int
rapp_ref_bitblt_norn(int op1, int op2);

static void
rapp_ref_bitblt_driver(uint8_t *dst, int dst_dim, int dst_off,
                       const uint8_t *src, int src_dim, int src_off,
                       int width, int height, int (*rop)(int, int));

/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

void
rapp_ref_bitblt_copy_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_copy);
}

void
rapp_ref_bitblt_not_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_not);
}

void
rapp_ref_bitblt_and_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_and);
}

void
rapp_ref_bitblt_or_bin(uint8_t *dst, int dst_dim, int dst_off,
                       const uint8_t *src, int src_dim, int src_off,
                       int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_or);
}

void
rapp_ref_bitblt_xor_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_xor);
}

void
rapp_ref_bitblt_nand_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_nand);
}

void
rapp_ref_bitblt_nor_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_nor);
}

void
rapp_ref_bitblt_xnor_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_xnor);
}

void
rapp_ref_bitblt_andn_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_andn);
}

void
rapp_ref_bitblt_orn_bin(uint8_t *dst, int dst_dim, int dst_off,
                        const uint8_t *src, int src_dim, int src_off,
                        int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_orn);
}

void
rapp_ref_bitblt_nandn_bin(uint8_t *dst, int dst_dim, int dst_off,
                          const uint8_t *src, int src_dim, int src_off,
                          int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_nandn);
}

void
rapp_ref_bitblt_norn_bin(uint8_t *dst, int dst_dim, int dst_off,
                         const uint8_t *src, int src_dim, int src_off,
                         int width, int height)
{
    rapp_ref_bitblt_driver(dst, dst_dim, dst_off, src, src_dim, src_off,
                           width, height, &rapp_ref_bitblt_norn);
}

/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rapp_ref_bitblt_copy(int op1, int op2)
{
    (void)op1;
    return op2;
}

static int
rapp_ref_bitblt_not(int op1, int op2)
{
    (void)op1;
    return !op2;
}

static int
rapp_ref_bitblt_and(int op1, int op2)
{
    return op1 && op2;
}

static int
rapp_ref_bitblt_or(int op1, int op2)
{
    return op1 || op2;
}

static int
rapp_ref_bitblt_xor(int op1, int op2)
{
    return (op1 || op2) && !(op1 && op2);
}

static int
rapp_ref_bitblt_nand(int op1, int op2)
{
    return !(op1 && op2);
}

static int
rapp_ref_bitblt_nor(int op1, int op2)
{
    return !(op1 || op2);
}

static int
rapp_ref_bitblt_xnor(int op1, int op2)
{
    return !rapp_ref_bitblt_xor(op1, op2);
}

static int
rapp_ref_bitblt_andn(int op1, int op2)
{
    return op1 && !op2;
}

static int
rapp_ref_bitblt_orn(int op1, int op2)
{
    return op1 || !op2;
}

static int
rapp_ref_bitblt_nandn(int op1, int op2)
{
    return !(op1 && !op2);
}

static int
rapp_ref_bitblt_norn(int op1, int op2)
{
    return !(op1 || !op2);
}

static void
rapp_ref_bitblt_driver(uint8_t *dst, int dst_dim, int dst_off,
                       const uint8_t *src, int src_dim, int src_off,
                       int width, int height, int (*rop)(int, int))
{
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int dpix = rapp_pixel_get_bin(dst, dst_dim, dst_off, x, y);
            int spix = rapp_pixel_get_bin(src, src_dim, src_off, x, y);
            dpix = (*rop)(dpix, spix);
            rapp_pixel_set_bin(dst, dst_dim, dst_off, x, y, dpix);
        }
    }
}
