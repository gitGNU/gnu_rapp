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
 *  @brief  Bitblit word raster operations.
 */

#ifndef RC_BITBLT_ROP_H
#define RC_BITBLT_ROP_H


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#define RC_BITBLT_ROP_COPY(srcw1, srcw2) \
    (srcw2)

#define RC_BITBLT_ROP_NOT(srcw1, srcw2) \
    (~(srcw2))

#define RC_BITBLT_ROP_AND(srcw1, srcw2) \
    ((srcw1) & (srcw2))

#define RC_BITBLT_ROP_OR(srcw1, srcw2) \
    ((srcw1) | (srcw2))

#define RC_BITBLT_ROP_XOR(srcw1, srcw2) \
    ((srcw1) ^ (srcw2))

#define RC_BITBLT_ROP_NAND(srcw1, srcw2) \
    (~((srcw1) & (srcw2)))

#define RC_BITBLT_ROP_NOR(srcw1, srcw2) \
    (~((srcw1) | (srcw2)))

#define RC_BITBLT_ROP_XNOR(srcw1, srcw2) \
    (~((srcw1) ^ (srcw2)))

#define RC_BITBLT_ROP_ANDN(srcw1, srcw2) \
    ((srcw1) & ~(srcw2))

#define RC_BITBLT_ROP_ORN(srcw1, srcw2) \
    ((srcw1) | ~(srcw2))

#define RC_BITBLT_ROP_NANDN(srcw1, srcw2) \
    (~(srcw1) | (srcw2))

#define RC_BITBLT_ROP_NORN(srcw1, srcw2) \
    (~(srcw1) & (srcw2))

#endif /* RC_BITBLT_ROP_H */
