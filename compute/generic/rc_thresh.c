/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_thresh.c
 *  @brief  RAPP Compute layer thresholding to binary, generic implementation.
 */

#include "rc_impl_cfg.h" /* Implementation cfg     */
#include "rc_thresh_tpl.h" /* Thresholding templates */
#include "rc_thresh.h"   /* Thresholding API       */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Compute op1 > op2 ? 1 : 0 without branches.
 */
#define RC_THRESH_CMPGT(op1, op2, arg) \
    ((unsigned)((int)(op2) - (int)(op1)) >> (8*sizeof(int) - 1))

/**
 *  Compute op1 < op2 ? 1 : 0 without branches.
 */
#define RC_THRESH_CMPLT(op1, op2, arg) \
    RC_THRESH_CMPGT(op2, op1, 0)

/**
 *  Compute op1 > op2 && op1 < op3 ? 1 : 0 without branches.
 */
#define RC_THRESH_CMPGTLT(op1, op2, op3) \
    (RC_THRESH_CMPGT(op1, op2, 0) &      \
     RC_THRESH_CMPLT(op1, op3, 0))

/**
 *  Compute op1 > op2 || op1 < op3 ? 1 : 0 without branches.
 */
#define RC_THRESH_CMPLTGT(op1, op2, op3) \
    (RC_THRESH_CMPLT(op1, op2, 0) |      \
     RC_THRESH_CMPGT(op1, op3, 0))


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Single thresholding greater-than.
 */
#if RC_IMPL(rc_thresh_gt_u8, 1)
void
rc_thresh_gt_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height, int thresh)
{
    RC_TEMPLATE_THRESH(dst, dst_dim, src, src_dim, width, height,
                       thresh, 0, RC_THRESH_CMPGT,
                       RC_UNROLL(rc_thresh_gt_u8));
}
#endif


/**
 *  Single thresholding less-than.
 */
#if RC_IMPL(rc_thresh_lt_u8, 1)
void
rc_thresh_lt_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height, int thresh)
{
    RC_TEMPLATE_THRESH(dst, dst_dim, src, src_dim, width, height,
                       thresh, 0, RC_THRESH_CMPLT,
                       RC_UNROLL(rc_thresh_lt_u8));
}
#endif


/**
 *  Double thresholding greater-than AND less-than.
 */
#if RC_IMPL(rc_thresh_gtlt_u8, 1)
void
rc_thresh_gtlt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int low, int high)
{
    RC_TEMPLATE_THRESH(dst, dst_dim, src, src_dim, width, height,
                       low, high, RC_THRESH_CMPGTLT,
                       RC_UNROLL(rc_thresh_gtlt_u8));
}
#endif


/**
 *  Double thresholding less-than OR greater-than.
 */
#if RC_IMPL(rc_thresh_ltgt_u8, 1)
void
rc_thresh_ltgt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int low, int high)
{
    RC_TEMPLATE_THRESH(dst, dst_dim, src, src_dim, width, height,
                       low, high, RC_THRESH_CMPLTGT,
                       RC_UNROLL(rc_thresh_ltgt_u8));
}
#endif
