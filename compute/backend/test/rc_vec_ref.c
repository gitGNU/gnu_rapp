/*  Copyright (C) 2005-2012, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_ref.c
 *  @brief  Vector operation reference implementation.
 */

#include <stdlib.h>      /* abs()         */
#include "rc_platform.h" /* RC_BIG_ENDIAN */
#include "rc_vec_ref.h"


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

#undef  MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef  MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef RC_BIG_ENDIAN
#define RC_VEC_REF_BIT(len, bit, pos) ((bit) << ((len) - (pos) - 1))
#else
#define RC_VEC_REF_BIT(len, bit, pos) ((bit) << (pos))
#endif


/*
 * -------------------------------------------------------------
 *  Field relocation
 * -------------------------------------------------------------
 */

rc_vec_ref_t
rc_vec_shl_ref(rc_vec_ref_t srcv, int bytes)
{
    rc_vec_ref_t dstv;

    memset(dstv.byte, 0, RC_VEC_SIZE);
    if (bytes < RC_VEC_SIZE) {
        memcpy(dstv.byte, &srcv.byte[bytes], RC_VEC_SIZE - bytes);
    }

    return dstv;
}

rc_vec_ref_t
rc_vec_shr_ref(rc_vec_ref_t srcv, int bytes)
{
    rc_vec_ref_t dstv;

    memset(dstv.byte, 0, RC_VEC_SIZE);
    if (bytes < RC_VEC_SIZE) {
        memcpy(&dstv.byte[bytes], srcv.byte, RC_VEC_SIZE - bytes);
    }

    return dstv;
}

rc_vec_ref_t
rc_vec_align_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int bytes)
{
    return rc_vec_or_ref(rc_vec_shl_ref(srcv1, bytes),
                         rc_vec_shr_ref(srcv2, RC_VEC_SIZE - bytes));
}

rc_vec_ref_t
rc_vec_pack_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE/2; k++) {
        dstv.byte[k                ] = srcv1.byte[2*k];
        dstv.byte[k + RC_VEC_SIZE/2] = srcv2.byte[2*k];
    }
    return dstv;
}


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

rc_vec_ref_t
rc_vec_not_ref(rc_vec_ref_t srcv)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = ~srcv.byte[k];
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_and_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = srcv1.byte[k] & srcv2.byte[k];
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_or_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = srcv1.byte[k] | srcv2.byte[k];
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_xor_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = srcv1.byte[k] ^ srcv2.byte[k];
    }
    return dstv;
}


/*
 * -------------------------------------------------------------
 *  Arithmetic operations on 8-bit fields
 * -------------------------------------------------------------
 */

rc_vec_ref_t
rc_vec_adds_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int sum = srcv1.byte[k] + srcv2.byte[k];
        dstv.byte[k] = sum < 0xff ? sum : 0xff;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_avgt_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = (srcv1.byte[k] + srcv2.byte[k]) >> 1;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_avgr_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = (srcv1.byte[k] + srcv2.byte[k] + 1) >> 1;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_subs_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int diff = srcv1.byte[k] - srcv2.byte[k];
        dstv.byte[k] = diff > 0 ? diff : 0;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_suba_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = abs(srcv1.byte[k] - srcv2.byte[k]);
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_subht_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = (srcv1.byte[k] - srcv2.byte[k] + 0xff) >> 1;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_subhr_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = (srcv1.byte[k] - srcv2.byte[k] + 0x100) >> 1;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_abs_ref(rc_vec_ref_t srcv)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int val = 2*abs(srcv.byte[k] - 0x80);
        dstv.byte[k] = val < 0xff ? val : 0xff;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_cmpgt_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = srcv1.byte[k] > srcv2.byte[k] ? 0xff : 0;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_cmpge_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = srcv1.byte[k] >= srcv2.byte[k] ? 0xff : 0;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_min_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = MIN(srcv1.byte[k], srcv2.byte[k]);
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_max_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        dstv.byte[k] = MAX(srcv1.byte[k], srcv2.byte[k]);
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_lerp_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int blend8)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int upd = (blend8*(srcv2.byte[k] - srcv1.byte[k]) + 0x80) >> 8;
        dstv.byte[k] = srcv1.byte[k] + upd;
    }
    return dstv;
}

rc_vec_ref_t
rc_vec_lerpz_ref(rc_vec_ref_t srcv1, rc_vec_ref_t srcv2, int blend8)
{
    rc_vec_ref_t dstv;
    int          k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int diff = srcv2.byte[k] - srcv1.byte[k];
        int upd  = (blend8*diff + (diff < 0 ? 0xff : 0)) >> 8;
        dstv.byte[k] = srcv1.byte[k] + upd;
    }
    return dstv;
}


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

unsigned
rc_vec_getmaskw_ref(rc_vec_ref_t srcv)
{
    unsigned mask = 0;
    int      k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        mask |= RC_VEC_REF_BIT(RC_VEC_SIZE, srcv.byte[k] >> 7, k);
    }
    return mask;
}

rc_vec_ref_t
rc_vec_getmaskv_ref(rc_vec_ref_t srcv)
{
    rc_vec_t dstv;
    int      k;

    memset(dstv.byte, 0, RC_VEC_SIZE);
    for (k = 0; k < (RC_VEC_SIZE + 7) / 8; k++) {
        unsigned mask = 0;
        int      b;
        for (b = 0; b < MIN(RC_VEC_SIZE, 8); b++) {
            mask |= RC_VEC_REF_BIT(8, srcv.byte[8*k + b] >> 7, b);
        }
        dstv.byte[k] = mask;
    }

    return dstv;
}

rc_vec_ref_t
rc_vec_setmaskv_ref(rc_vec_ref_t srcv)
{
    rc_vec_t dstv;
    int      k;

    memset(dstv.byte, 0, RC_VEC_SIZE);
    for (k = 0; k < RC_VEC_SIZE; k++)
        dstv.byte[k] = (RC_VEC_REF_BIT(8, 1, (k & 7)) & srcv.byte[k / 8] ?
                        0xff : 0);

    return dstv;
}


/*
 * -------------------------------------------------------------
 *  Reductions
 * -------------------------------------------------------------
 */

rc_vec_ref_t
rc_vec_cntv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv)
{
    int k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        int byte = srcv.byte[k];
        int i;
        for (i = 0; i < 8; i++) {
            accv.word += (byte >> i) & 1;
        }
    }
    return accv;
}

rc_vec_ref_t
rc_vec_sumv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv)
{
    int k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        accv.word += srcv.byte[k];
    }
    return accv;
}

rc_vec_ref_t
rc_vec_macv_ref(rc_vec_ref_t accv, rc_vec_ref_t srcv1, rc_vec_ref_t srcv2)
{
    int k;
    for (k = 0; k < RC_VEC_SIZE; k++) {
        accv.word += srcv1.byte[k]*srcv2.byte[k];
    }
    return accv;
}
