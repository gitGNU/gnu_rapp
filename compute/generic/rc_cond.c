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
 *  @file   rc_cond.c
 *  @brief  RAPP Compute layer conditional operations.
 */

#include <string.h>      /* memset()        */
#include "rc_platform.h" /* RC_UNLIKELY()   */
#include "rc_word.h"     /* Word operations */
#include "rc_table.h"    /* Lookup tables   */
#include "rc_cond.h"     /* Exported API    */
#include "rc_util.h"     /* MIN(), MAX(). */
#include "rc_impl_cfg.h" /* Tuning. */


/**
 *  Conditional pixel operation template.
 *  Single-operand: op1 is the working buffer and arg1 and arg2 are
 *  additional arguments.
 *  double-operand: op1 is the dst buffer and arg1 is the src buffer
 *  and arg2 is an additional argument.
 *  pixop is the pixelwise operation macro, same as in standard pixop.
 *  mask should be the values 0x0 or 0xff.
 *
 *  The result depends on the mask value:
 *  If the mask is set the result of pixop is stored.
 *  If the mask is not set, the previous value is retained
 *  (actually, written back).
 */
#define RC_COND_PIXOP_TEMPLATE(op1, pixop, arg1, mask)  \
do {                                                    \
    int cdst_ = (op1);                                  \
    pixop(cdst_, (arg1));                               \
    (op1) = ((op1) & ~(mask)) | (cdst_ & (mask));       \
} while (0)

/**
 *  Saturated addition.
 */
#define RC_PIXOP_ADDS(op1, op2)      \
do {                                 \
    int sum_ = (op1) + (op2);        \
    (op1) = MIN(sum_, 0xff);         \
} while (0)

/**
 *  Saturated subtraction.
 */
#define RC_PIXOP_SUBS(op1, op2)      \
do {                                 \
    int diff_ = (op1) - (op2);       \
    (op1) = MAX(diff_, 0);           \
} while (0)

/*
 * -------------------------------------------------------------
 *  Single-operand word template macro.
 * -------------------------------------------------------------
 */
#define RC_COND_WORD_TEMPLATE(dst, pixop, arg1, word)                    \
do {                                                                     \
    /* Handle individual bytes. */                                       \
    uint32_t *d32_ = (uint32_t*)(dst);                                   \
    rc_word_t mask_word_ = word;                                         \
    int b_;                                                              \
    for (b_ = 0;                                                         \
         b_ < 8*RC_WORD_SIZE && mask_word_;                              \
         b_ += 8, mask_word_ = RC_WORD_SHL(mask_word_, 8))               \
    {                                                                    \
        /* Read 8 bits from the conditional mask. */                     \
        rc_word_t byte_ = mask_word_ & RC_WORD_INSERT(0xff, 0, 8);       \
        if (!byte_) {                                                    \
            /* All conditions false. Skip two 32-bit words. */           \
            d32_ += 2;                                                   \
        }                                                                \
        else if (byte_ == RC_WORD_INSERT(0xff, 0, 8)) {                  \
            /* Apply pixop on two 32-bit words without condition. */     \
            int words_;                                                  \
            for (words_ = 0; words_ < 2; words_++, d32_++) {             \
                uint32_t src32_ = *d32_;                                 \
                uint32_t res32_ = 0;                                     \
                unsigned bn_;                                            \
                                                                         \
                for (bn_ = 0; bn_ < sizeof(src32_); bn_++) {             \
                    unsigned d_;                                         \
                    d_ = RC_32_EXTRACT(src32_, 8*bn_, 8);                \
                    pixop(d_, arg1);                                     \
                    res32_ |= RC_32_INSERT(d_, 8*bn_, 8);                \
                }                                                        \
                                                                         \
                *d32_ = res32_;                                          \
            }                                                            \
        }                                                                \
        else {                                                           \
            /* Handle nibbles. */                                        \
            unsigned  nibble_;                                           \
            int words_;                                                  \
            for (words_ = 0; words_ < 2; words_++, d32_++) {             \
                nibble_ = RC_WORD_EXTRACT(byte_, words_ * 4, 4);         \
                if (nibble_) {                                           \
                    uint32_t m32_ = rc_table_expand[nibble_];            \
                    uint32_t src32_ = *d32_;                             \
                    uint32_t res32_ = 0;                                 \
                    unsigned bn_;                                        \
                                                                         \
                    for (bn_ = 0; bn_ < sizeof(src32_); bn_++) {         \
                        unsigned d_;                                     \
                        unsigned m_;                                     \
                        m_ = RC_32_EXTRACT(m32_,  8*bn_, 8);             \
                        d_ = RC_32_EXTRACT(src32_, 8*bn_, 8);            \
                        RC_COND_PIXOP_TEMPLATE(d_, pixop, arg1, m_);     \
                        res32_ |= RC_32_INSERT(d_, 8*bn_, 8);            \
                    }                                                    \
                    *d32_ = res32_;                                      \
                }                                                        \
            }                                                            \
        }                                                                \
    }                                                                    \
} while (0)

#define RC_COND_TEMPLATE(dst, dst_dim, map, map_dim, width, height,     \
                         pixop, arg1)                                   \
do {                                                                    \
    /* Full words. */                                                   \
    int len_ = (width) / (8*RC_WORD_SIZE);                              \
    /* Remaining pixels. */                                             \
    int rem_ = (width) % (8*RC_WORD_SIZE);                              \
    /* Partial word bit mask. */                                        \
    rc_word_t mask_ = RC_WORD_SHL(RC_WORD_ONE, 8*RC_WORD_SIZE - rem_);  \
                                                                        \
    /* Process all rows. */                                             \
    int y_;                                                             \
    for (y_ = 0; y_ < (height); y_++) {                                 \
        int i_ = y_*(map_dim);                                          \
        int j_ = y_*(dst_dim);                                          \
        int x_;                                                         \
                                                                        \
        /* Handle all full words. */                                    \
        for (x_ = 0;                                                    \
             x_ < len_;                                                 \
             x_++, i_ += RC_WORD_SIZE, j_ += 8*RC_WORD_SIZE) {          \
            rc_word_t word_ = RC_WORD_LOAD(&(map)[i_]);                 \
            if (RC_UNLIKELY(word_)) {                                   \
                RC_COND_WORD_TEMPLATE(&(dst)[j_], pixop, arg1, word_);  \
            }                                                           \
        }                                                               \
                                                                        \
        /* Handle the partial word. */                                  \
        if (rem_) {                                                     \
            rc_word_t word_ = RC_WORD_LOAD(&(map)[i_]) & mask_;         \
            if (RC_UNLIKELY(word_)) {                                   \
                RC_COND_WORD_TEMPLATE(&(dst)[j_], pixop, arg1, word_);  \
            }                                                           \
        }                                                               \
    }                                                                   \
} while (0)

/*
 * -------------------------------------------------------------
 *  Double-operand word template macro.
 * -------------------------------------------------------------
 */
#define RC_COND_WORD_TEMPLATE2(dst, src, pixop, word)                  \
do {                                                                   \
    /* Handle individual bytes. */                                     \
    uint32_t *d32_ = (uint32_t*)(dst);                                 \
    const uint32_t *s32_ = (const uint32_t*)(src);                     \
    rc_word_t mask_word_ = word;                                       \
    int b_;                                                            \
    for (b_ = 0;                                                       \
         b_ < 8*RC_WORD_SIZE && mask_word_;                            \
         b_ += 8, mask_word_ = RC_WORD_SHL(mask_word_, 8))             \
    {                                                                  \
        /* Read 8 bits from the conditional mask. */                   \
        rc_word_t byte_ = mask_word_ & RC_WORD_INSERT(0xff, 0, 8);     \
        if (!byte_) {                                                  \
            /* All conditions false. Skip two 32-bit words. */         \
            d32_ += 2;                                                 \
            s32_ += 2;                                                 \
        }                                                              \
        else if (byte_ == RC_WORD_INSERT(0xff, 0, 8)) {                \
            /* Apply pixop on two 32-bit words without condition. */   \
            int words_;                                                \
            for (words_ = 0; words_ < 2; words_++, d32_++, s32_++) {   \
                const uint32_t src32_ = *s32_;                         \
                uint32_t dst32_ = *d32_;                               \
                uint32_t res32_ = 0;                                   \
                unsigned bn_;                                          \
                                                                       \
                for (bn_ = 0; bn_ < sizeof(res32_); bn_++) {           \
                    unsigned d_, s_;                                   \
                    s_ = RC_32_EXTRACT(src32_, 8*bn_, 8);              \
                    d_ = RC_32_EXTRACT(dst32_, 8*bn_, 8);              \
                    pixop(d_, s_);                                     \
                    res32_ |= RC_32_INSERT(d_, 8*bn_, 8);              \
                }                                                      \
                *d32_ = res32_;                                        \
            }                                                          \
        }                                                              \
        else {                                                         \
            /* Handle nibbles. */                                      \
            unsigned  nibble_;                                         \
            int words_;                                                \
            for (words_ = 0; words_ < 2; words_++, d32_++, s32_++) {   \
                nibble_ = RC_WORD_EXTRACT(byte_, words_ * 4, 4);       \
                if (nibble_) {                                         \
                    uint32_t m32_ = rc_table_expand[nibble_];          \
                    const uint32_t src32_ = *s32_;                     \
                    uint32_t dst32_ = *d32_;                           \
                    uint32_t res32_ = 0;                               \
                    unsigned bn_;                                      \
                                                                       \
                    for (bn_ = 0; bn_ < sizeof(res32_); bn_++) {       \
                        unsigned d_, s_, m_;                           \
                        s_ = RC_32_EXTRACT(src32_, 8*bn_, 8);          \
                        d_ = RC_32_EXTRACT(dst32_, 8*bn_, 8);          \
                        m_ = RC_32_EXTRACT(m32_, 8*bn_, 8);            \
                        RC_COND_PIXOP_TEMPLATE(d_, pixop, s_, m_);     \
                        res32_ |= RC_32_INSERT(d_, 8*bn_, 8);          \
                    }                                                  \
                    *d32_ = res32_;                                    \
                }                                                      \
            }                                                          \
        }                                                              \
    }                                                                  \
} while (0)

#define RC_COND_TEMPLATE2(dst, dst_dim, map, map_dim, width, height,    \
                          pixop)                                        \
do {                                                                    \
    /* Full words. */                                                   \
    int len_ = (width) / (8*RC_WORD_SIZE);                              \
    /* Remaining pixels. */                                             \
    int rem_ = (width) % (8*RC_WORD_SIZE);                              \
    /* Partial word bit mask. */                                        \
    rc_word_t mask_ = RC_WORD_SHL(RC_WORD_ONE, 8*RC_WORD_SIZE - rem_);  \
    int y_;                                                             \
    /* Process all rows. */                                             \
    for (y_ = 0; y_ < (height); y_++) {                                 \
        int i_ = y_*(map_dim);                                          \
        int j_ = y_*(dst_dim);                                          \
        int k_ = y_*(src_dim);                                          \
        int x_;                                                         \
                                                                        \
        /* Handle all full words. */                                    \
        for (x_ = 0;                                                    \
             x_ < len_;                                                 \
             x_++,                                                      \
               i_ += RC_WORD_SIZE,                                      \
               j_ += 8*RC_WORD_SIZE,                                    \
               k_ += 8*RC_WORD_SIZE)                                    \
        {                                                               \
            rc_word_t word_ = RC_WORD_LOAD(&(map)[i_]);                 \
            if (RC_UNLIKELY(word_)) {                                   \
                RC_COND_WORD_TEMPLATE2(&(dst)[j_], &(src)[k_],          \
                                       pixop, word_);                   \
            }                                                           \
        }                                                               \
                                                                        \
        /* Handle the partial word. */                                  \
        if (rem_) {                                                     \
            rc_word_t word_ = RC_WORD_LOAD(&(map)[i_]) & mask_;         \
            if (RC_UNLIKELY(word_)) {                                   \
                RC_COND_WORD_TEMPLATE2(&(dst)[j_], &(src)[k_],          \
                                       pixop, word_);                   \
            }                                                           \
        }                                                               \
    }                                                                   \
} while (0)

/*
 * -------------------------------------------------------------
 *  Local functions fwd declare.
 * -------------------------------------------------------------
 */
#if RC_IMPL(rc_cond_set_u8, 1)
static void
rc_cond_set_word(uint8_t *buf, rc_word_t word, uint32_t v32);
#endif

#if RC_IMPL(rc_cond_copy_u8, 1)
static void
rc_cond_copy_word(uint8_t *restrict dst,
                  const uint8_t *restrict src,
                  rc_word_t word);
#endif

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

#if RC_IMPL(rc_cond_set_u8, 1)
void
rc_cond_set_u8(uint8_t *restrict dst, int dst_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height, unsigned value)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    uint32_t  v32;
    int       y;

    /* Set the value to all fields */
    v32  = value & 0xff;
    v32 |= (v32 << 8) | (v32 << 16) | (v32 << 24);

    /* Process all rows */
    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*dst_dim;
        int x;

        /* Handle all full words */
        for (x = 0; x < len; x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE) {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                rc_cond_set_word(&dst[j], word, v32);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                rc_cond_set_word(&dst[j], word, v32);
            }
        }
    }
}
#endif

#if RC_IMPL(rc_cond_copy_u8, 1)
void
rc_cond_copy_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height)
{
    int       len  = width / (8*RC_WORD_SIZE); /* Full words            */
    int       rem  = width % (8*RC_WORD_SIZE); /* Remaining pixels      */
    rc_word_t mask = RC_WORD_SHL(RC_WORD_ONE,  /* Partial word bit mask */
                                 8*RC_WORD_SIZE - rem);
    int       y;

    /* Process all rows */
    for (y = 0; y < height; y++) {
        int i = y*map_dim;
        int j = y*dst_dim;
        int k = y*src_dim;
        int x;

        /* Handle all full words */
        for (x = 0;
             x < len;
             x++, i += RC_WORD_SIZE, j += 8*RC_WORD_SIZE, k += 8*RC_WORD_SIZE)
        {
            rc_word_t word = RC_WORD_LOAD(&map[i]);
            if (RC_UNLIKELY(word)) {
                rc_cond_copy_word(&dst[j], &src[k], word);
            }
        }

        /* Handle the partial word */
        if (rem) {
            rc_word_t word = RC_WORD_LOAD(&map[i]) & mask;
            if (RC_UNLIKELY(word)) {
                rc_cond_copy_word(&dst[j], &src[k], word);
            }
        }
    }
}
#endif


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

#if RC_IMPL(rc_cond_set_u8, 1)
static void
rc_cond_set_word(uint8_t *buf, rc_word_t word, uint32_t v32)
{
    if (word == RC_WORD_ONE) {
        /* Set 8 full words */
        memset(buf, v32 & 0xff, 8*RC_WORD_SIZE);
    }
    else {
        /* Handle individual bytes */
        uint32_t *p32 = (uint32_t*)buf;
        int       k;
        for (k = 0;
             k < 8*RC_WORD_SIZE && word;
             k += 8, word = RC_WORD_SHL(word, 8))
        {
            rc_word_t byte = word & RC_WORD_INSERT(0xff, 0, 8);
            if (!byte) {
                /* Skip two 32-bit words */
                p32 += 2;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Set two 32-bit words */
                *p32++ = v32;
                *p32++ = v32;
            }
            else {
                /* Handle nibbles */
                unsigned  nibble;

                nibble = RC_WORD_EXTRACT(byte, 0, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *p32 = (*p32 & ~m32) | (v32 & m32);
                }
                p32++;

                nibble = RC_WORD_EXTRACT(byte, 4, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *p32 = (*p32 & ~m32) | (v32 & m32);
                }
                p32++;
            }
        }
    }
}
#endif

#if RC_IMPL(rc_cond_addc_u8, 1)
void
rc_cond_addc_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height, unsigned value)
{
    RC_COND_TEMPLATE(dst, dst_dim, map, map_dim, width, height,
                     RC_PIXOP_ADDS, value);
}
#endif

#if RC_IMPL(rc_cond_subc_u8, 1)
void
rc_cond_subc_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height, unsigned value)
{
    RC_COND_TEMPLATE(dst, dst_dim, map, map_dim, width, height,
                     RC_PIXOP_SUBS, value);
}
#endif

#if RC_IMPL(rc_cond_copy_u8, 1)
static void
rc_cond_copy_word(uint8_t *restrict dst,
                  const uint8_t *restrict src,
                  rc_word_t word)
{
    if (word == RC_WORD_ONE) {
        /* Copy 8 full words */
        memcpy(dst, src, 8*RC_WORD_SIZE);
    }
    else {
        /* Handle individual bytes */
        uint32_t       *d32 = (uint32_t*)dst;
        const uint32_t *s32 = (const uint32_t*)src;
        int             k;
        for (k = 0;
             k < 8*RC_WORD_SIZE && word;
             k += 8, word = RC_WORD_SHL(word, 8))
        {
            rc_word_t byte = word & RC_WORD_INSERT(0xff, 0, 8);
            if (!byte) {
                /* Skip two 32-bit words */
                d32 += 2;
                s32 += 2;
            }
            else if (byte == RC_WORD_INSERT(0xff, 0, 8)) {
                /* Copy two 32-bit words */
                *d32++ = *s32++;
                *d32++ = *s32++;
            }
            else {
                /* Handle nibbles */
                unsigned  nibble;

                nibble = RC_WORD_EXTRACT(byte, 0, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *d32 = (*d32 & ~m32) | (*s32 & m32);
                }
                d32++, s32++;

                nibble = RC_WORD_EXTRACT(byte, 4, 4);
                if (nibble) {
                    uint32_t m32 = rc_table_expand[nibble];
                    *d32 = (*d32 & ~m32) | (*s32 & m32);
                }
                d32++, s32++;
            }
        }
    }
}
#endif

#if RC_IMPL(rc_cond_add_u8, 1)
void
rc_cond_add_u8(uint8_t *restrict dst, int dst_dim,
               const uint8_t *restrict src, int src_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height)
{
    RC_COND_TEMPLATE2(dst, dst_dim, map, map_dim,
                      width, height, RC_PIXOP_ADDS);
}
#endif
