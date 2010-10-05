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
 *  @file   rc_bitblt_rop.h
 *  @brief  Bitblit vector raster operations.
 */

#ifndef RC_BITBLT_ROP_H
#define RC_BITBLT_ROP_H

#include "rc_vector.h"

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#define RC_BITBLT_ROP_COPY(dstv, srcv1, srcv2) \
    ((dstv) = (srcv2))

#ifdef RC_VEC_NOT
#define RC_BITBLT_ROP_NOT(dstv, srcv1, srcv2) \
    RC_VEC_NOT(dstv, srcv2)
#endif

#ifdef RC_VEC_AND
#define RC_BITBLT_ROP_AND(dstv, srcv1, srcv2) \
    RC_VEC_AND(dstv, srcv1, srcv2)
#endif

#ifdef RC_VEC_OR
#define RC_BITBLT_ROP_OR(dstv, srcv1, srcv2) \
    RC_VEC_OR(dstv, srcv1, srcv2)
#endif

#ifdef RC_VEC_XOR
#define RC_BITBLT_ROP_XOR(dstv, srcv1, srcv2) \
    RC_VEC_XOR(dstv, srcv1, srcv2)
#endif

#if defined RC_VEC_AND && defined RC_VEC_NOT
#define RC_BITBLT_ROP_NAND(dstv, srcv1, srcv2) \
do {                                           \
    RC_VEC_AND(dstv,  srcv1, srcv2);           \
    RC_VEC_NOT(dstv, dstv);                    \
} while (0)
#endif

#if defined RC_VEC_OR && defined RC_VEC_NOT
#define RC_BITBLT_ROP_NOR(dstv, srcv1, srcv2) \
do {                                          \
    RC_VEC_OR(dstv,  srcv1, srcv2);           \
    RC_VEC_NOT(dstv, dstv);                   \
} while (0)
#endif

#ifdef RC_VEC_XORNOT
#define RC_BITBLT_ROP_XNOR(dstv, srcv1, srcv2) \
    RC_VEC_XORNOT(dstv, srcv1, srcv2)
#endif

#ifdef RC_VEC_ANDNOT
#define RC_BITBLT_ROP_ANDN(dstv, srcv1, srcv2) \
    RC_VEC_ANDNOT(dstv, srcv1, srcv2)

#define RC_BITBLT_ROP_NORN(dstv, srcv1, srcv2) \
    RC_VEC_ANDNOT(dstv, srcv2, srcv1)
#endif

#ifdef RC_VEC_ORNOT
#define RC_BITBLT_ROP_ORN(dstv, srcv1, srcv2) \
    RC_VEC_ORNOT(dstv, srcv1, srcv2)

#define RC_BITBLT_ROP_NANDN(dstv, srcv1, srcv2) \
    RC_VEC_ORNOT(dstv, srcv2, srcv1)
#endif


#endif /* RC_BITBLT_ROP_H */
