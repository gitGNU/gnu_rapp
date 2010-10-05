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
 *  @file   rc_pad.c
 *  @brief  RAPP Compute layer 8-bit image padding.
 */

#include "rc_impl_cfg.h" /* Implementation config */
#include "rc_pad.h"      /* Padding API           */


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */


/**
 *  Pad to the left with a constant value.
 */
void
rc_pad_const_left_u8(uint8_t *buf, int dim,
                     int width, int height, int size, int value)
{
    (void)width;

    if (size == 1) {
        int y, i;

        for (y = 0, i = -1; y < height; y++, i += dim) {
            buf[i] = value;
        }
    }
    else {
        int y;

        for (y = 0; y < height; y++) {
            int i = y*dim - size;
            int x;

            for (x = 0; x < size; x++, i++) {
                buf[i] = value;
            }
        }
    }
}


/**
 *  Pad to the right with a constant value.
 */
void
rc_pad_const_right_u8(uint8_t *buf, int dim,
                      int width, int height, int size, int value)
{
    if (size == 1) {
        int y, i;

        for (y = 0, i = width; y < height; y++, i += dim) {
            buf[i] = value;
        }
    }
    else {
        int y;

        for (y = 0; y < height; y++) {
            int i = y*dim + width;
            int x;

            for (x = 0; x < size; x++, i++) {
                buf[i] = value;
            }
        }
    }
}


/**
 *  Pad to the left by clamping (replication).
 */
void
rc_pad_clamp_left_u8(uint8_t *buf, int dim,
                     int width, int height, int size)
{
    (void)width;

    if (size == 1) {
        int y, i;

        for (y = 0, i = 0; y < height; y++, i += dim) {
            buf[i - 1] = buf[i];
        }
    }
    else {
        int y;

        for (y = 0; y < height; y++) {
            int i = y*dim - 1;
            int v = buf[i + 1];
            int x;

            for (x = 0; x < size; x++, i--) {
                buf[i] = v;
            }
        }
    }
}


/**
 *  Pad to the right by clamping (replication).
 */
void
rc_pad_clamp_right_u8(uint8_t *buf, int dim,
                      int width, int height, int size)
{
    if (size == 1) {
        int y, i;

        for (y = 0, i = width - 1; y < height; y++, i += dim) {
            buf[i + 1] = buf[i];
        }
    }
    else {
        int y;

        for (y = 0; y < height; y++) {
            int i = y*dim + width;
            int v = buf[i - 1];
            int x;

            for (x = 0; x < size; x++, i++) {
                buf[i] = v;
            }
        }
    }
}
