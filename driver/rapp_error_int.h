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
 *  @file   rapp_error_int.h
 *  @brief  RAPP internal error handling.
 */

#ifndef RAPP_ERROR_INT_H
#define RAPP_ERROR_INT_H

#include <stdint.h> /* uint8_t */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Get the error code for a binary image.
 */
int
rapp_error_bin(const uint8_t *buf, int dim, int width, int height);

/**
 *  Get the error code for a binary image without alignment.
 */
int
rapp_error_noalign_bin(const uint8_t *buf, int dim,
                       int off, int width, int height);

/**
 *  Get the error code for an 8-bit image.
 */
int
rapp_error_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  Get the error code for an 8-bit image without alignment.
 */
int
rapp_error_noalign_u8(const uint8_t *buf, int dim, int width, int height);

/**
 *  Get the error code for an 16-bit image.
 */
int
rapp_error_u16(const uint16_t *buf, int dim, int width, int height);

/**
 *  Get the error code for an 32-bit image.
 */
int
rapp_error_u32(const uint32_t *buf, int dim, int width, int height);

/**
 *  Get the error code for two binary images.
 */
int
rapp_error_bin_bin(const uint8_t *buf1, int dim1, int width1, int height1,
                   const uint8_t *buf2, int dim2, int width2, int height2);

/**
 *  Get the error code for two binary images without alignment.
 */
int
rapp_error_noalign_bin_bin(const uint8_t *buf1, int dim1, int off1,
                           const uint8_t *buf2, int dim2, int off2,
                           int width, int height);

/**
 *  Get the error code for a binary image and an 8-bit image.
 */
int
rapp_error_bin_u8(const uint8_t *bin, int bin_dim,
                  const uint8_t *u8,  int u8_dim,
                  int width, int height);

/**
 *  Get the error code for a binary image and a 16-bit image.
 */
int
rapp_error_bin_u16(const uint8_t *bin, int bin_dim,
                   const uint16_t *u16, int u16_dim,
                   int width, int height);

/**
 *  Get the error code for a binary image and a 32-bit image.
 */
int
rapp_error_bin_u32(const uint8_t *bin, int bin_dim,
                   const uint32_t *u32, int u32_dim,
                   int width, int height);

/**
 *  Get the error code for an 8-bit image and a binary image.
 */
int
rapp_error_u8_bin(const uint8_t *u8,  int u8_dim,
                  const uint8_t *bin, int bin_dim,
                  int width, int height);

/**
 *  Get the error code for two 8-bit images.
 */
int
rapp_error_u8_u8(const uint8_t *buf1, int dim1, int width1, int height1,
                 const uint8_t *buf2, int dim2, int width2, int height2);

/**
 *  Get the error code for 8-bit image and a 16-bit image.
 */
int
rapp_error_u8_u16(const uint8_t *buf1, int dim1, int width1, int height1,
                  const uint16_t *buf2, int dim2, int width2, int height2);

/**
 *  Get the error code for 8-bit image and a 32-bit image.
 */
int
rapp_error_u8_u32(const uint8_t *buf1, int dim1, int width1, int height1,
                  const uint32_t *buf2, int dim2, int width2, int height2);

/**
 *  Just call abort.
 */
void
rapp_abort(void);

#if RC_ASSERTED_RETURNS
#define RAPP_ABORT_FOR_ASSERTED_RETURNS() rapp_abort()
#else
#define RAPP_ABORT_FOR_ASSERTED_RETURNS()
#endif

#ifdef __cplusplus
};
#endif

#endif /* RAPP_ERROR_INT_H */
