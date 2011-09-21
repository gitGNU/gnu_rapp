/*  Copyright (C) 2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_armv6.h
 *  @brief  RAPP compute layer vector operations for ARMv6
 *          parallel arithmetic instructions.
 */

#ifndef RC_VEC_ARMV6_H
#define RC_VEC_ARMV6_H

#ifndef RC_VECTOR_H
#error "Do not include this file directly! Use rc_vector.h instead."
#endif /* !RC_VECTOR_H */

/**
 *  This file was written with the help of the documentation found in the
 *  ARM1136JF-S Technical Reference Manual:
 *  <http://infocenter.arm.com/help/topic/com.arm.doc.ddi0211i/DDI0211.pdf>
 */

#include "rc_vec_swar.h" /* SWAR vector instructions */

/* Performance hints */
#define RC_VEC_HINT_CMPGT
#define RC_VEC_HINT_AVGT


#undef RC_VEC_ADDS
#define RC_VEC_ADDS(dstv, srcv1, srcv2)             \
    __asm__("uqadd8 %[dst],%[op0],%[op1]" :         \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv1), [op1] "r" (srcv2))

#undef RC_VEC_AVGT
#define RC_VEC_AVGT(dstv, srcv1, srcv2)             \
    __asm__("uhadd8 %[dst],%[op0],%[op1]" :         \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv1), [op1] "r" (srcv2))

#undef RC_VEC_SUBS
#define RC_VEC_SUBS(dstv, srcv1, srcv2)             \
    __asm__("uqsub8 %[dst],%[op0],%[op1]" :         \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv1), [op1] "r" (srcv2))

#undef RC_VEC_SUBA
#define RC_VEC_SUBA(dstv, srcv1, srcv2)                 \
do {                                                    \
    rc_vec_t sv1__ = (srcv1);                           \
    rc_vec_t sv2__ = (srcv2);                           \
    rc_vec_t r__, s__;                                  \
    /* The __volatile__ are needed her to stop GCC from \
       reordering the instructions. The usub8           \
       instruction writs to the GE status flags and     \
       sel use them to decide the right field. */       \
    __asm__ __volatile__("usub8 %[dst],%[op0],%[op1]" : \
            [dst] "=r" (r__) :                          \
            [op0] "r" (sv1__), [op1] "r" (sv2__));      \
    __asm__ __volatile__("usub8 %[dst],%[op0],%[op1]" : \
            [dst] "=r" (s__) :                          \
            [op0] "r" (sv2__), [op1] "r" (sv1__));      \
    __asm__ __volatile__("sel %[dst],%[op0],%[op1]" :   \
            [dst] "=r" (dstv) :                         \
            [op0] "r" (s__), [op1] "r" (r__));          \
} while (0)


#undef RC_VEC_CMPGT
#define RC_VEC_CMPGT(dstv, srcv1, srcv2)            \
do {                                                \
    rc_vec_t z__ = RC_WORD_C8(0);                   \
    rc_vec_t a__ = RC_WORD_C8(80);                  \
    __asm__("usub8 %[dst],%[op0],%[op1]\n\t"        \
            "sel   %[dst],%[op2],%[op3]\n\t" :      \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv2), [op1] "r" (srcv1),   \
            [op2] "r" (z__), [op3] "r" (a__));      \
} while (0)

#undef RC_VEC_MIN
#define RC_VEC_MIN(dstv, srcv1, srcv2)              \
    __asm__("usub8 %[dst],%[op0],%[op1]\n\t"        \
            "sel   %[dst],%[op1],%[op0]\n\t" :      \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv1), [op1] "r" (srcv2));

#undef RC_VEC_MAX
#define RC_VEC_MAX(dstv, srcv1, srcv2)              \
    __asm__("usub8 %[dst],%[op0],%[op1]\n\t"        \
            "sel   %[dst],%[op0],%[op1]\n\t" :      \
            [dst] "=r" (dstv) :                     \
            [op0] "r" (srcv1), [op1] "r" (srcv2));

#undef RC_VEC_SUMV
#define RC_VEC_SUMV(accv, srcv)                     \
do {                                                \
    rc_vec_t av__ = (accv);                         \
    rc_vec_t sv__ = (srcv);                         \
    __asm__("uxtab16 %[dst],%[op0],%[op1], ror #0" :\
            [dst] "=r" (av__) :                     \
            [op0] "r" (av__), [op1] "r" (sv__));    \
    __asm__("uxtab16 %[dst],%[op0],%[op1], ror #8" :\
            [dst] "=r" (accv) :                     \
            [op0] "r" (av__), [op1] "r" (sv__));    \
} while (0)

#undef RC_VEC_SUMR
#define RC_VEC_SUMR(sum, accv)                      \
do {                                                \
    rc_vec_t s__ = 0;                               \
    rc_vec_t av__ = (accv);                         \
    __asm__("uxtah %[dst],%[op0],%[op1], ror #0" :  \
            [dst] "=r" (s__) :                      \
            [op0] "r" (s__), [op1] "r" (av__));     \
    __asm__("uxtah %[dst],%[op0],%[op1], ror #16" : \
            [dst] "=r" (sum) :                      \
            [op0] "r" (s__), [op1] "r" (av__));     \
} while (0);

#endif /* RC_VEC_ARMV6_H */
