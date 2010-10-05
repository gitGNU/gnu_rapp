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
 *  @file   rapp_bitblt.c
 *  @brief  RAPP bitblt operations.
 *
 *  ALGORITHM
 *  ---------
 *  First, the bit aligment value of the buffers are computed.
 *  It is in the range 0 - (8*RC_ALIGNMENT - 1). Both buffers
 *  are then offset to the nearest previous alignment boundary.
 *  When doing the actual blit we use the fastest function available.
 *  The list of functions to choose from is determined by the relative
 *  buffer alignment values:
 *
 *  Alignment  Functions
 *  ----------------------------
 *  vector     vector-aligned
 *             word-aligned
 *  word       vector-misaligned*
 *             word-aligned
 *  byte       vector-misaligned
 *             word-misaligned
 *  bit        word-misaligned
 *
 *  *) Only if the vector size is greater than the word size.
 *
 *  When there is more than one candidate, the fastest one is
 *  determined by the value of the tuning constant <function>_SCORE.
 *
 *  If possible, a 1 x width*height blit is used instead of a
 *  width x height blit, since the since the former is usually faster.
 */

#include <string.h>         /* memcpy()         */
#include "rappcompute.h"    /* RAPP Compute API */
#include "rapptune.h"       /* RAPP tune result */
#include "rapp_api.h"       /* API symbol macro */
#include "rapp_util.h"      /* Validation       */
#include "rapp_error.h"     /* Error codes       */
#include "rapp_error_int.h" /* Error handling   */
#include "rapp_bitblt.h"    /* Bitblt API       */

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Construct the compute-layer function name.
 */
#define RAPP_BITBLT_FUNC(rop, impl) \
    rc_bitblt_ ## impl ## _ ## rop ## _bin

/**
 *  Construct the benchmark score constant.
 */
#define RAPP_BITBLT_SCORE(rop, impl) \
    rc_bitblt_ ## impl ## _ ## rop ## _bin ## _SCORE

/**
 *  SIMD implementation boolean.
 */
#define RAPP_BITBLT_SIMD(rop, impl) \
    (rc_bitblt_ ## impl ## _ ## rop ## _bin ## _IMPL == RC_IMPL_SIMD)

/**
 *  Get the best (fastest) compute-layer function of two implementations.
 *  The selection is based on nominal score values from the tuning process.
 */
#define RAPP_BITBLT_BEST(rop, impl1, impl2)      \
    RAPP_BITBLT_SCORE(rop, impl1) >              \
    RAPP_BITBLT_SCORE(rop, impl2) ?              \
    (void (*)())&RAPP_BITBLT_FUNC(rop, impl1)  : \
    (void (*)())&RAPP_BITBLT_FUNC(rop, impl2)

/**
 *  Get the best vector-aligned compute-layer function for an operation.
 *  Choose from vector-aligned and word-aligned implementations.
 */
#define RAPP_BITBLT_VECTOR(rop) \
    RAPP_BITBLT_BEST(rop, va, wa)

/**
 *  Get the best word-aligned compute-layer function for an operation.
 *  Choose from vector-misaligned and word-aligned implementations if
 *  the vector size is greater than the word size.
 */
#if RAPP_BITBLT_SIMD(rop, vm) && RC_ALIGNMENT > RC_NATIVE_SIZE
#define RAPP_BITBLT_WORD(rop) \
    RAPP_BITBLT_BEST(rop, vm, wa)
#else
#define RAPP_BITBLT_WORD(rop) \
    &rc_bitblt_wa_ ## rop ## _bin
#endif

/**
 *  Get the best byte-aligned compute-layer function for an operation.
 *  Choose from vector-misaligned and word-misaligned implementations.
 */
#define RAPP_BITBLT_BYTE(rop) \
    RAPP_BITBLT_BEST(rop, vm, wm)

/**
 *  Get the best bit-aligned compute-layer function for an operation.
 *  There is only one alternative.
 */
#define RAPP_BITBLT_BIT(rop) \
    &rc_bitblt_wm_ ## rop ## _bin

/**
 *  Construct an entry in the compute-layer dispatcher table.
 */
#define RAPP_BITBLT_ENTRY(rop) \
    {RAPP_BITBLT_VECTOR(rop),  \
     RAPP_BITBLT_WORD(rop),    \
     RAPP_BITBLT_BYTE(rop),    \
     RAPP_BITBLT_BIT(rop)}

/**
 *  Align a pointer down to the nearest vector-boundary.
 */
#define RAPP_BITBLT_ALIGN(ptr) \
    ((uint8_t*)((intptr_t)(ptr) & ~(RC_ALIGNMENT - 1)))

/**
 *  Get the bit offset of a misaligned pointer.
 */
#define RAPP_BITBLT_OFFSET(ptr) \
    (8*((uintptr_t)(ptr) & (RC_ALIGNMENT - 1)))

/**
 *  Get the number of vector blocks for a given width and bit offset.
 */
#define RAPP_BITBLT_BLOCKS(width, offset) \
    (((width) + (offset) + 8*RC_ALIGNMENT - 1) / (8*RC_ALIGNMENT))


/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

/**
 *  The raster operation symbolic constants.
 *  The numbers MUST match the indices in the dispatcher
 *  table rapp_bitblt_tab[].
 */
typedef enum rapp_bitblt_rop_e {
    RAPP_BITBLT_COPY  = 0,
    RAPP_BITBLT_NOT   = 1,
    RAPP_BITBLT_AND   = 2,
    RAPP_BITBLT_OR    = 3,
    RAPP_BITBLT_XOR   = 4,
    RAPP_BITBLT_NAND  = 5,
    RAPP_BITBLT_NOR   = 6,
    RAPP_BITBLT_XNOR  = 7,
    RAPP_BITBLT_ANDN  = 8,
    RAPP_BITBLT_ORN   = 9,
    RAPP_BITBLT_NANDN = 10,
    RAPP_BITBLT_NORN  = 11
} rapp_bitblt_rop_t;

/**
 *  The compute-layer dispatcher table type.
 */
typedef struct rapp_bitblt_st {
    void (*vector)();
    void (*word)();
    void (*byte)();
    void (*bit)();
} rapp_bitblt_t;


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  The compute-layer dispatcher table.
 *  Contains pointers to the fastest compute-layer functions for
 *  different alignments, on a per-operation basis.
 */
static const rapp_bitblt_t rapp_bitblt_tab[] = {
    RAPP_BITBLT_ENTRY(copy),
    RAPP_BITBLT_ENTRY(not),
    RAPP_BITBLT_ENTRY(and),
    RAPP_BITBLT_ENTRY(or),
    RAPP_BITBLT_ENTRY(xor),
    RAPP_BITBLT_ENTRY(nand),
    RAPP_BITBLT_ENTRY(nor),
    RAPP_BITBLT_ENTRY(xnor),
    RAPP_BITBLT_ENTRY(andn),
    RAPP_BITBLT_ENTRY(orn),
    RAPP_BITBLT_ENTRY(nandn),
    RAPP_BITBLT_ENTRY(norn)
};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_bitblt_driver(uint8_t *restrict dst, int dst_dim, int dst_off,
                   const uint8_t *restrict src, int src_dim, int src_off,
                   int width, int height, rapp_bitblt_rop_t rop);
static void
rapp_bitblt_misaligned(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int dst_len, int src_len, int height, int align,
                       void (*blit)());

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */


RAPP_API(int, rapp_bitblt_copy_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_COPY);
}

RAPP_API(int, rapp_bitblt_not_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_NOT);
}

RAPP_API(int, rapp_bitblt_and_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_AND);
}

RAPP_API(int, rapp_bitblt_or_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_OR);
}

RAPP_API(int, rapp_bitblt_xor_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_XOR);
}

RAPP_API(int, rapp_bitblt_nand_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_NAND);
}

RAPP_API(int, rapp_bitblt_nor_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_NOR);
}

RAPP_API(int, rapp_bitblt_xnor_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_XNOR);
}

RAPP_API(int, rapp_bitblt_andn_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_ANDN);
}

RAPP_API(int, rapp_bitblt_orn_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_ORN);
}

RAPP_API(int, rapp_bitblt_nandn_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_NANDN);
}

RAPP_API(int, rapp_bitblt_norn_bin,
         (uint8_t *restrict dst, int dst_dim, int dst_off,
          const uint8_t *restrict src, int src_dim, int src_off,
          int width, int height))
{
    return rapp_bitblt_driver(dst, dst_dim, dst_off,
                              src, src_dim, src_off,
                              width, height, RAPP_BITBLT_NORN);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

/**
 *  Bitblit driver.
 *  Handles bitblits of any alignment and with any raster operation.
 */
static int
rapp_bitblt_driver(uint8_t *restrict dst, int dst_dim, int dst_off,
                   const uint8_t *restrict src, int src_dim, int src_off,
                   int width, int height, rapp_bitblt_rop_t rop)
{
    int align;
    int blocks;

    /* Check arguments */
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, src, src_dim, height,
                                     rc_align((width + dst_off + 7) / 8),
                                     rc_align((width + src_off + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_NOALIGN_BIN(dst, dst_dim, dst_off, width, height) ||
        !RAPP_VALIDATE_NOALIGN_BIN(src, src_dim, src_off, width, height))
    {
        /* Return the error code */
        return rapp_error_noalign_bin_bin(dst, dst_dim, dst_off,
                                          src, src_dim, src_off,
                                          width, height);
    }

    /* Compute the vector-aligned bit offsets */
    dst_off += RAPP_BITBLT_OFFSET(dst);
    src_off += RAPP_BITBLT_OFFSET(src);

    /* Align data pointers down to nearest vector boundary */
    dst = RAPP_BITBLT_ALIGN(dst);
    src = RAPP_BITBLT_ALIGN(src);

    /* Compute the destination width in vector block units */
    blocks = RAPP_BITBLT_BLOCKS(width, dst_off);

    /* Check if we can perform a more efficient 1D blit */
    if (dst_dim == src_dim &&
        blocks  == dst_dim / RC_ALIGNMENT)
    {
        width  += 8 * RC_ALIGNMENT * blocks * (height - 1);
        blocks *= height;
        height  = 1;
    }

    /* Compute the relative bit alignment */
    align = src_off - dst_off;

    /* Perform alignment-dependent handling */
    if (align == 0) {
        /* Handle vector-level alignment */
        (*rapp_bitblt_tab[rop].vector)(dst, dst_dim, src, src_dim,
                                       8*RC_ALIGNMENT*blocks, height);
    }
    else {
        void (*blit)();

        if (align % (8*RC_NATIVE_SIZE) == 0) {
            /* Set word-level alignment blitter */
            blit = rapp_bitblt_tab[rop].word;
        }
        else if (align % 8 == 0) {
            /* Set byte-level alignment blitter */
            blit = rapp_bitblt_tab[rop].byte;
        }
        else {
            /* Set bit-level alignment blitter */
            blit = rapp_bitblt_tab[rop].bit;
        }

        /* Call the misaligned bitblit driver */
        rapp_bitblt_misaligned(dst, dst_dim, src, src_dim, blocks,
                               RAPP_BITBLT_BLOCKS(width, src_off),
                               height, align, blit);
    }

    return RAPP_OK;
}

/**
 *  Misaligned bitblit driver.
 *  Operation-independent handling of misaligned bitblits.
 */
static void
rapp_bitblt_misaligned(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int dst_len, int src_len, int height, int align,
                       void (*blit)())
{
    uint8_t  raw[3*RC_ALIGNMENT] = {0};                /* Misaligned buffer */
    uint8_t *buf = &raw[RC_ALIGNMENT -                 /* Portably aligned  */
                        (uintptr_t)raw % RC_ALIGNMENT];

    assert(((uintptr_t)buf % RC_ALIGNMENT) == 0);

    if (align > 0) {
        if (src_len > dst_len) {
            /* Blit everything at once */
            (*blit)(dst, dst_dim, &src[align / 8], src_dim,
                    8*RC_ALIGNMENT*dst_len, height, align % 8);
        }
        else {
            /* Blit everything except the last row */
            if (height > 1) {
                (*blit)(dst, dst_dim, &src[align / 8], src_dim,
                        8*RC_ALIGNMENT*dst_len, height - 1, align % 8);
            }

            /* Blit the last row except the last dst vector block */
            (*blit)(&dst[(height - 1)*dst_dim], dst_dim,
                    &src[(height - 1)*src_dim + align/8], src_dim,
                    8*RC_ALIGNMENT*(dst_len - 1), 1, align % 8);

            /* Copy the last vector block to a two-block buffer */
            memcpy(buf, &src[(height - 1)*src_dim +
                             (dst_len - 1)*RC_ALIGNMENT], RC_ALIGNMENT);

            /* Blit the last vector block from the buffer */
            (*blit)(&dst[(height - 1)*dst_dim + (dst_len - 1)*RC_ALIGNMENT],
                    0, &buf[align / 8], 0, 8*RC_ALIGNMENT, 1, align % 8);
        }
    }
    else {
        /* Adjust the align value to be positive by
         * letting the src buffer start one block in advance.
         */
        align += 8*RC_ALIGNMENT;

        /* Copy the first vector block to a two-block buffer */
        memcpy(&buf[RC_ALIGNMENT], src, RC_ALIGNMENT);

        /* Blit the first vector block from the padded buffer */
        (*blit)(dst, 0, &buf[align / 8], 0, 8*RC_ALIGNMENT, 1, align % 8);

        /* Blit the first row except the first dst vector block */
        (*blit)(&dst[RC_ALIGNMENT], dst_dim,
                &src[align / 8], src_dim,
                8*RC_ALIGNMENT*(dst_len - 1), 1, align % 8);

        /* Blit the remaining data */
        if (height > 1) {
            (*blit)(&dst[dst_dim], dst_dim,
                    &src[src_dim - RC_ALIGNMENT + align / 8], src_dim,
                    8*RC_ALIGNMENT*dst_len, height - 1, align % 8);
        }
    }
}
