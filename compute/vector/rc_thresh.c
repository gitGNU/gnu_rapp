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
 *  @file   rc_thresh.c
 *  @brief  RAPP Compute layer thresholding to binary, vector implementation.
 */

#include "rc_impl_cfg.h"   /* Implementation config  */
#include "rc_vector.h"     /* Vector operations      */
#include "rc_thresh.h"     /* Thresholding API       */
#include "rc_thresh_tpl.h" /* Thresholding templates */

#ifdef RC_THRESH_TEMPLATE

/*
 * -------------------------------------------------------------
 *  Comparison macros
 * -------------------------------------------------------------
 */

/**
 *  Compare less-than.
 */
#ifdef RC_VEC_CMPGT
#define RC_THRESH_CMPGT(dstv, srcv, thrv, arg) \
    RC_VEC_CMPGT(dstv, srcv, thrv)

/**
 *  Compare less-than.
 */
#define RC_THRESH_CMPLT(dstv, srcv, thrv, arg) \
    RC_VEC_CMPGT(dstv, thrv, srcv)
#endif

/**
 *  Compare greater-than-or-equal.
 */
#ifdef RC_VEC_CMPGE
#define RC_THRESH_CMPGE(dstv, srcv, thrv, arg) \
    RC_VEC_CMPGE(dstv, srcv, thrv)

/**
 *  Compare less-than-or-equal.
 */
#define RC_THRESH_CMPLE(dstv, srcv, thrv, arg) \
    RC_VEC_CMPGE(dstv, thrv, srcv)
#endif

/**
 *  Compare greater-than AND less-than.
 */
#if defined RC_VEC_CMPGT && defined RC_VEC_AND
#define RC_THRESH_CMPGTLT(dstv, srcv, thrv1, thrv2) \
do {                                                \
    rc_vec_t tmpv;                                  \
    RC_VEC_CMPGT(tmpv, srcv, thrv1);                \
    RC_VEC_CMPGT(dstv, thrv2, srcv);                \
    RC_VEC_AND(dstv, dstv, tmpv);                   \
} while (0)
#endif

/**
 *  Compare greater-than-or-equal AND less-than-or-equal.
 */
#if defined RC_VEC_CMPGE && defined RC_VEC_AND
#define RC_THRESH_CMPGELE(dstv, srcv, thrv1, thrv2) \
do {                                                \
    rc_vec_t tmpv;                                  \
    RC_VEC_CMPGE(tmpv, srcv, thrv1);                \
    RC_VEC_CMPGE(dstv, thrv2, srcv);                \
    RC_VEC_AND(dstv, dstv, tmpv);                   \
} while (0)
#endif

/**
 *  Compare less-than AND greater-than.
 */
#if defined RC_VEC_CMPGT && defined RC_VEC_OR
#define RC_THRESH_CMPLTGT(dstv, srcv, thrv1, thrv2) \
do {                                                \
    rc_vec_t tmpv;                                  \
    RC_VEC_CMPGT(tmpv, thrv1, srcv);                \
    RC_VEC_CMPGT(dstv, srcv, thrv2);                \
    RC_VEC_OR(dstv, dstv, tmpv);                    \
} while (0)
#endif

/**
 *  Compare less-than-or-equal AND greater-than-or-equal.
 */
#if defined RC_VEC_CMPGE && defined RC_VEC_OR
#define RC_THRESH_CMPLEGE(dstv, srcv, thrv1, thrv2) \
do {                                                \
    rc_vec_t tmpv;                                  \
    RC_VEC_CMPGE(tmpv, thrv1, srcv);                \
    RC_VEC_CMPGE(dstv, srcv, thrv2);                \
    RC_VEC_OR(dstv, dstv, tmpv);                    \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Single thresholding greater-than.
 */
#if RC_IMPL(rc_thresh_gt_u8, 1)
#if defined RC_VEC_HINT_CMPGT || defined RC_THRESH_CMPGE
void
rc_thresh_gt_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height, int thresh)
{
#ifdef RC_VEC_HINT_CMPGT
    /* Use the faster CMPGT directly */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, thresh, 0, RC_THRESH_CMPGT,
                       RC_UNROLL(rc_thresh_gt_u8));
#else
    /* Use the faster CMPGE with an adjusted threshold */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, thresh + 1, 0, RC_THRESH_CMPGE,
                       RC_UNROLL(rc_thresh_gt_u8));
#endif
}
#endif
#endif


/**
 *  Single thresholding less-than.
 */
#if RC_IMPL(rc_thresh_lt_u8, 1)
#if defined RC_VEC_HINT_CMPGT || defined RC_THRESH_CMPLE
void
rc_thresh_lt_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                int width, int height, int thresh)
{
#ifdef RC_VEC_HINT_CMPGT
    /* Use the faster CMPGT, reversed to CMPLT */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, thresh, 0, RC_THRESH_CMPLT,
                       RC_UNROLL(rc_thresh_lt_u8));
#else
    /* Use the faster CMPGE, reversed to CMPLE, with an adjusted threshold */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, thresh - 1, 0, RC_THRESH_CMPLE,
                       RC_UNROLL(rc_thresh_lt_u8));
#endif
}
#endif
#endif


/**
 *  Double thresholding greater-than AND less-than.
 */
#if RC_IMPL(rc_thresh_gtlt_u8, 1)
#if (defined RC_VEC_HINT_CMPGT && defined RC_THRESH_CMPGTLT) || \
     defined RC_THRESH_CMPGELE
void
rc_thresh_gtlt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int low, int high)
{
#ifdef RC_VEC_HINT_CMPGT
    /* Use the faster CMPGT/CMPLT directly */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, low, high, RC_THRESH_CMPGTLT,
                       RC_UNROLL(rc_thresh_gtlt_u8));
#else
    /* Use the faster CMPGE/CMPLE with adjusted thresholds */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, low + 1, high - 1, RC_THRESH_CMPGELE,
                       RC_UNROLL(rc_thresh_gtlt_u8));
#endif
}
#endif
#endif


/**
 *  Double thresholding less-than OR greater-than.
 */
#if RC_IMPL(rc_thresh_ltgt_u8, 1)
#if (defined RC_VEC_HINT_CMPGT && defined RC_THRESH_CMPLTGT) || \
     defined RC_THRESH_CMPLEGE
void
rc_thresh_ltgt_u8(uint8_t *restrict dst, int dst_dim,
                  const uint8_t *restrict src, int src_dim,
                  int width, int height, int low, int high)
{
#ifdef RC_VEC_HINT_CMPGT
    /* Use the faster CMPGT/CMPLT directly */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, low, high, RC_THRESH_CMPLTGT,
                       RC_UNROLL(rc_thresh_ltgt_u8));
#else
    /* Use the faster CMPGE/CMPLE with adjusted thresholds */
    RC_THRESH_TEMPLATE(dst, dst_dim, src, src_dim, width,
                       height, low - 1, high + 1, RC_THRESH_CMPLEGE,
                       RC_UNROLL(rc_thresh_ltgt_u8));
#endif
}
#endif
#endif

#endif /* RC_THRESH_TEMPLATE */
