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
 *  @file   rapp_ref_pad_bin.h
 *  @brief  RAPP binary image padding, reference implementation.
 */

#ifndef RAPP_REF_PAD_BIN_H
#define RAPP_REF_PAD_BIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

void
rapp_ref_pad_align_bin(uint8_t *buf, int dim, int off,
                       int width, int height, int value);

void
rapp_ref_pad_const_bin(uint8_t *buf, int dim, int off,
                       int width, int height, int size, int set);

void
rapp_ref_pad_clamp_bin(uint8_t *buf, int dim, int off,
                       int width, int height, int size);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_REF_PAD_BIN_H */
