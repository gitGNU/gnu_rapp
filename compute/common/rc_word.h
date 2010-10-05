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
 *  @file   rc_word.h
 *  @brief  RAPP Compute layer word type and operations.
 */

#ifndef RC_WORD_H
#define RC_WORD_H

#include <stdint.h>      /* Fixed-size integers           */
#include "rc_platform.h" /* Platform-specific definitions */
#include "rc_table.h"    /* Lookup tables                 */

/*
 * -------------------------------------------------------------
 *  The word type
 * -------------------------------------------------------------
 */

/**
 *  The number of bytes in a word is the native
 *  machine word size if not set explicitly.
 */
#ifndef RC_WORD_SIZE
#define RC_WORD_SIZE RC_NATIVE_SIZE
#endif

/**
 *  The word type definition.
 */
#if   RC_WORD_SIZE == 8
typedef uint64_t rc_word_t;
#elif RC_WORD_SIZE == 4
typedef uint32_t rc_word_t;
#elif RC_WORD_SIZE == 2
typedef uint16_t rc_word_t;
#else
#error "Unsupported word size."
#endif


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  A word with all zero bits.
 */
#define RC_WORD_ZERO ((rc_word_t)0)

/**
 *  A word with all one bits.
 */
#define RC_WORD_ONE  ((rc_word_t)~RC_WORD_ZERO)


/*
 * -------------------------------------------------------------
 *  Memory access
 * -------------------------------------------------------------
 */

/**
 *  Load a word from memory.
 */
#define RC_WORD_LOAD(ptr) \
    (*(const rc_word_t*)(ptr))

/**
 *  Store a word in memory.
 */
#define RC_WORD_STORE(ptr, word) \
    (*(rc_word_t*)(ptr) = (word))


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  Word constants.
 */
#if RC_WORD_SIZE == 8
#define RC_WORD_C8(field) \
    RC_WORD_HEX_8__(field, UINT64_C)
#define RC_WORD_C16(field) \
    RC_WORD_HEX_4__(field, UINT64_C)
#define RC_WORD_C32(field) \
    RC_WORD_HEX_2__(field, UINT64_C)
#define RC_WORD_C64(field) \
    RC_WORD_HEX_1__(field, UINT64_C)

#elif RC_WORD_SIZE == 4
#define RC_WORD_C8(field) \
    RC_WORD_HEX_4__(field, UINT32_C)
#define RC_WORD_C16(field) \
    RC_WORD_HEX_2__(field, UINT32_C)
#define RC_WORD_C32(field) \
    RC_WORD_HEX_1__(field, UINT32_C)

#elif RC_WORD_SIZE == 2
#define RC_WORD_C8(field) \
    RC_WORD_HEX_2__(field, UINT16_C)
#define RC_WORD_C16(field) \
    RC_WORD_HEX_1__(field, UINT16_C)
#endif


/*
 * -------------------------------------------------------------
 *  Field selection and relocation
 * -------------------------------------------------------------
 */

/**
 *  A word with the bit at the logical position @e pos set.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_WORD_BIT(pos) \
    ((rc_word_t)1 << (8*RC_WORD_SIZE - (pos) - 1))
#else
#define RC_WORD_BIT(pos) \
    ((rc_word_t)1 << (pos))
#endif

/**
 *  Nominal right bit shift.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_WORD_SHR(word, bits) \
    ((rc_word_t)(word) >> (bits))
#else
#define RC_WORD_SHR(word, bits) \
    ((rc_word_t)(word) << (bits))
#endif

/**
 *  Nominal left bit shift.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_WORD_SHL(word, bits) \
    ((rc_word_t)(word) << (bits))
#else
#define RC_WORD_SHL(word, bits) \
    ((rc_word_t)(word) >> (bits))
#endif

/**
 *  Align word1 and word2, starting at bit @e bits
 *  into concatenation of word1 and word2.
 */
#define RC_WORD_ALIGN(word1, word2, bits) \
    (RC_WORD_SHL(word1, bits) |           \
     RC_WORD_SHR(word2, 8*RC_WORD_SIZE - (bits)))

/**
 *  Insert bits into the logical positions
 *  @e pos, ..., @e pos + @e bits of a word.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_WORD_INSERT(value, pos, bits) \
    ((rc_word_t)(value) << (8*RC_WORD_SIZE - (pos) - (bits)))
#else
#define RC_WORD_INSERT(value, pos, bits) \
    ((rc_word_t)(value) << (pos))
#endif

/**
 *  Extract the bits at the logical positions
 *  @e pos, ..., @e pos + @e bits of a word into an integer.
 */
#ifdef  RC_BIG_ENDIAN
#define RC_WORD_EXTRACT(word, pos, bits)                        \
    (((rc_word_t)(word) >> (8*RC_WORD_SIZE - (pos) - (bits))) & \
      RC_WORD_MASK__(bits))
#else
#define RC_WORD_EXTRACT(word, pos, bits) \
    (((rc_word_t)(word) >> (pos)) & RC_WORD_MASK__(bits))
#endif

/**
 *  Count the number of set bits in a word.
 */
#if defined __GNUC__ && RC_GCC_VERSION > 3004 && \
    RC_WORD_SIZE == RC_NATIVE_SIZE
#if RC_WORD_SIZE == 8
#define RC_WORD_BITCOUNT(cnt, word) \
    ((cnt) = __builtin_popcountl(word))

#else /* RC_WORD_SIZE <= 4 */
#define RC_WORD_BITCOUNT(cnt, word) \
    ((cnt) = __builtin_popcount(word))
#endif

#else /* Fall back on table lookups */
#define RC_WORD_BITCOUNT(cnt, word)                        \
do {                                                       \
    rc_word_t w__ = (word);                                \
    (cnt)  = rc_table_bitcount[w__ & 0xff], w__ >>= 8;     \
    (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8;     \
    if (RC_WORD_SIZE >= 4) {                               \
        (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8; \
        (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8; \
    }                                                      \
    if (RC_WORD_SIZE == 8) {                               \
        (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8; \
        (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8; \
        (cnt) += rc_table_bitcount[w__ & 0xff], w__ >>= 8; \
        (cnt) += rc_table_bitcount[w__ & 0xff];            \
    }                                                      \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  Internal support macros
 * -------------------------------------------------------------
 */

/**
 *  A bit mask with the @e bits least significant bits set.
 */
#define RC_WORD_MASK__(bits) \
    (RC_WORD_ONE >> (8*RC_WORD_SIZE - (bits)))

/**
 *  An hexadecimal word constant with eight fields.
 */
#define RC_WORD_HEX_8__(digs, suff) \
    suff(0x ## digs ## digs ## digs ## digs ## digs ## digs ## digs ## digs)

/**
 *  An hexadecimal word constant with four fields.
 */
#define RC_WORD_HEX_4__(digs, suff) \
    suff(0x ## digs ## digs ## digs ## digs)

/**
 *  An hexadecimal word constant with two fields.
 */
#define RC_WORD_HEX_2__(digs, suff) \
    suff(0x ## digs ## digs)

/**
 *  An hexadecimal word constant with one field.
 */
#define RC_WORD_HEX_1__(digs, suff) \
    suff(0x ## digs)


#endif /* RC_WORD_H */
