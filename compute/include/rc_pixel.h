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
 *  @file   rc_pixel.h
 *  @brief  RAPP Compute layer pixel access macros.
 */

#ifndef RC_PIXEL_H
#define RC_PIXEL_H

#include <stdint.h>      /* uint8_t    */
#include "rc_platform.h" /* Endianness */

/**
 *  Get the word buffer index for a binary pixel.
 */
#define RC_PIXEL_GET_IDX(dim, off, x, y) \
    ((int)(y)*(int)(dim) + (((int)(x) + (int)(off)) >> 3))

/**
 *  Get the word bit position for a binary pixel.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_PIXEL_GET_BIT(dim, off, x) \
    (7 - (((int)(x) + (int)(off)) & 7))
#else
#define RC_PIXEL_GET_BIT(dim, off, x) \
    (((int)(x) + (int)(off)) & 7)
#endif

/**
 *  Get a binary pixel value.
 */
#define RC_PIXEL_GET_BIN(buf, dim, off, x, y)                      \
    ((((const uint8_t*)(buf))[RC_PIXEL_GET_IDX(dim, off, x, y)] >> \
                              RC_PIXEL_GET_BIT(dim, off, x)) & 1)

/**
 *  Set a binary pixel value.
 */
#define RC_PIXEL_SET_BIN(buf, dim, off, x, y, pix)		\
do {								\
      if (pix)							\
	(((uint8_t*)(buf))[RC_PIXEL_GET_IDX(dim, off, x, y)] |=	\
	 1 << RC_PIXEL_GET_BIT(dim, off, x));			\
      else							\
	(((uint8_t*)(buf))[RC_PIXEL_GET_IDX(dim, off, x, y)] &=	\
	 ~(1 << RC_PIXEL_GET_BIT(dim, off, x)));		\
} while (0)

/**
 *  Get a u8 pixel value.
 */
#define RC_PIXEL_GET_U8(buf, dim, x, y) \
    ((const uint8_t*)(buf))[(int)(y)*(int)(dim) + (int)(x)]

/**
 *  Set a u8 pixel value
 */
#define RC_PIXEL_SET_U8(buf, dim, x, y, pix) \
    (((uint8_t*)(buf))[(int)(y)*(int)(dim) + (int)(x)] = (uint8_t)(pix))


#endif /* RC_PIXEL_H */
