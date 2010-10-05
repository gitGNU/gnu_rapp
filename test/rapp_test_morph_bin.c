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
 *  @file   rapp_test_morph_bin.c
 *  @brief  Correctness tests for binary morphology.
 */

#include <string.h>         /* memcpy()       */
#include "rapp.h"           /* RAPP API       */
#include "rapp_test_util.h" /* Test utilities */


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

/**
 *  The area of the octagon SEs.
 */
static const int rapp_test_octagon_area[32] = {
       0,    0,    9,   21,   37,  69,    97,  145,
     185,  249,  301,  357,  445,  513,  585,  697,
     817,  909, 1005, 1149, 1257, 1417, 1537, 1661,
    1845, 1981, 2121, 2329, 2545, 2705, 2869, 3109
};

/**
 *  The area of the disc SEs.
 */
static const int rapp_test_disc_area[32] = {
       0,    0,    5,   13,   29,   49,   73,  105,
     141,  197,  245,  317,  377,  441,  521,  597,
     701,  789,  881,  993, 1121, 1265, 1381, 1525,
    1653, 1821, 1977, 2121, 2313, 2453, 2613, 2829
};


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_test_get_pattern(uint8_t **pat, int *dim,
                      int (*morph)(), int radius, bool dilate);
static void
rapp_test_blit_pattern(uint8_t *buf, const uint8_t *pat, int dim,
                       int width, int height, int dx, int dy);
static bool
rapp_test_duality_driver(int (*erode)(), int (*dilate)(),
                         int width, int height, bool isotropic);
static bool
rapp_test_rectangle_driver(int (*morph)(), int width,
                           int height, bool dilate);
static bool
rapp_test_isotropic_driver(int (*morph)(), int radius, int area, bool dilate);


/*
 * -------------------------------------------------------------
 *  Test cases
 * -------------------------------------------------------------
 */

bool
rapp_test_morph_erode_rect_bin(void)
{
    int  width, height;
    bool ok = true;

    for (height = 1; height < 64 && ok; height++) {
        for (width = 1; width < 64 && ok; width++) {
            if (width != 1 || height != 1) {
                ok &= rapp_test_rectangle_driver(&rapp_morph_erode_rect_bin,
                                                 width, height, false);

                ok &= rapp_test_duality_driver(&rapp_morph_erode_rect_bin,
                                               &rapp_morph_dilate_rect_bin,
                                               width, height, false);
            }
        }
    }

    return ok;
}

bool
rapp_test_morph_dilate_rect_bin(void)
{
    int  width, height;
    bool ok = true;

    for (height = 1; height < 64 && ok; height++) {
        for (width = 1; width < 64 && ok; width++) {
            if (width != 1 || height != 1) {
                ok = rapp_test_rectangle_driver(&rapp_morph_dilate_rect_bin,
                                                width, height, true);
            }
        }
    }

    return ok;
}

bool
rapp_test_morph_erode_diam_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok &= rapp_test_isotropic_driver(&rapp_morph_erode_diam_bin,
                                         rad, 2*rad*(rad - 1) + 1, false);

        ok &= rapp_test_duality_driver(&rapp_morph_erode_diam_bin,
                                       &rapp_morph_dilate_diam_bin,
                                       2*rad - 1, 2*rad - 1, true);
    }
    return ok;
}

bool
rapp_test_morph_dilate_diam_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok = rapp_test_isotropic_driver(&rapp_morph_dilate_diam_bin,
                                        rad, 2*rad*(rad - 1) + 1, true);
    }
    return ok;
}

bool
rapp_test_morph_erode_oct_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok &= rapp_test_isotropic_driver(&rapp_morph_erode_oct_bin, rad,
                                         rapp_test_octagon_area[rad], false);

        ok &= rapp_test_duality_driver(&rapp_morph_erode_oct_bin,
                                       &rapp_morph_dilate_oct_bin,
                                       2*rad - 1, 2*rad - 1, true);
    }
    return ok;
}

bool
rapp_test_morph_dilate_oct_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok = rapp_test_isotropic_driver(&rapp_morph_dilate_oct_bin, rad,
                                        rapp_test_octagon_area[rad], true);
    }
    return ok;
}

bool
rapp_test_morph_erode_disc_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok &= rapp_test_isotropic_driver(&rapp_morph_erode_disc_bin, rad,
                                         rapp_test_disc_area[rad], false);

        ok &= rapp_test_duality_driver(&rapp_morph_erode_disc_bin,
                                       &rapp_morph_dilate_disc_bin,
                                       2*rad - 1, 2*rad - 1, true);
    }
    return ok;
}

bool
rapp_test_morph_dilate_disc_bin(void)
{
    int  rad;
    bool ok = true;

    for (rad = 2; rad < 32 && ok; rad++) {
        ok = rapp_test_isotropic_driver(&rapp_morph_dilate_disc_bin, rad,
                                        rapp_test_disc_area[rad], true);
    }
    return ok;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

/**
 *  Get the SE pattern for a morphological function.
 */
static void
rapp_test_get_pattern(uint8_t **pat, int *dim,
                      int (*morph)(), int radius, bool dilate)
{
    int      size    = 2*radius - 1;
    int      dst_dim = rapp_align((size + 7) / 8);
    int      src_dim = dst_dim + 2*rapp_alignment;
    uint8_t *dst_buf = rapp_malloc(size*dst_dim, 0);
    uint8_t *src_buf = rapp_malloc((size + 2*16)*src_dim, 0);
    uint8_t *pad_buf = &src_buf[rapp_alignment + 16*src_dim];
    void    *work    = rapp_malloc(rapp_morph_worksize_bin(size, size), 0);

    /* Set up source buffer with one seed pixel */
    memset(src_buf, dilate ? 0 : 0xff, (size + 2*16)*src_dim);
    rapp_pixel_set_bin(pad_buf, src_dim, 0, radius - 1, radius - 1, dilate);

    /* Call morphological function */
    (*morph)(dst_buf, dst_dim, pad_buf, src_dim, size, size, radius, work);

    /* Invert the pattern if erosion */
    if (!dilate) {
        rapp_pixop_not_u8(dst_buf, dst_dim, dst_dim, size);
    }

    /* Clear all padding */
    rapp_pad_align_bin(dst_buf, dst_dim, 0, size, size, 0);

    *pat = dst_buf;
    *dim = dst_dim;

    rapp_free(src_buf);
    rapp_free(work);
}

/**
 *  Blit a pattern to a buffer at a given offset.
 */
static void
rapp_test_blit_pattern(uint8_t *buf, const uint8_t *pat, int dim,
                       int width, int height, int dx, int dy)
{
    int x  = abs(dx);
    int y  = abs(dy);
    int xb = dx > 0 ? x : 0;
    int yb = dy > 0 ? y : 0;
    int xp = dx > 0 ? 0 : x;
    int yp = dy > 0 ? 0 : y;
    int w  = width  - x;
    int h  = height - y;

    /* Blit the pattern at the translated position */
    rapp_bitblt_copy_bin(&buf[xb / 8 + yb*dim], dim, xb % 8,
                         &pat[xp / 8 + yp*dim], dim, xp % 8, w, h);

    /* Clear pixels outside the pattern area */
    rapp_pad_align_bin(&buf[xb / 8 + yb*dim], dim, xb % 8, w, h, 0);
}

/**
 *  Check erosion/dilation duality, i.e.
 *  erosion on a dilated point should return the original point.
 */
static bool
rapp_test_duality_driver(int (*erode)(), int (*dilate)(),
                         int width, int height, bool isotropic)
{
    int      dim     = rapp_align((width + 7) / 8) + 2*rapp_alignment;
    uint8_t *dst_pad = rapp_malloc((height + 2*16)*dim, 0);
    uint8_t *src_pad = rapp_malloc((height + 2*16)*dim, 0);
    uint8_t *dst_buf = &dst_pad[rapp_alignment + 16*dim];
    uint8_t *src_buf = &src_pad[rapp_alignment + 16*dim];
    void    *work    = rapp_malloc(rapp_morph_worksize_bin(width, height), 0);
    int      radius  = (width + 1) / 2;
    int      ok      = -1; /* Needs to be int, to cover negative values */
    /**
     *  Glibc 2.2 implements memset with helper macros that don't
     *  properly parenthesize memset arguments. Putting in extra
     *  parentheses is too ugly, but a separate variable is ok.
     */
    int   fill_value = dilate != NULL ? 0 : 0xff;

    /* Set up source buffer with one seed pixel */
    memset(src_pad, fill_value, (height + 2*16)*dim);
    rapp_pixel_set_bin(src_buf, dim, 0, width / 2, height / 2, 1);

    /* Call morphological dilation function */
    if (isotropic) {
        ok = (*dilate)(dst_buf, dim, src_buf, dim,
                       width, height, radius, work);
    }
    else {
        ok = (*dilate)(dst_buf, dim, src_buf, dim,
                       width, height, width, height, work);
    }
    if (ok < 0) {
        DBG("Got FAIL return value\n");
        goto Done;
    }

    /* Pad with zeros */
    rapp_pad_const_bin(dst_buf, dim, 0, width, height, 16, 0);

    /* Call morphological erosion function */
    if (isotropic) {
        ok = (*erode)(src_buf, dim, dst_buf, dim,
                      width, height, radius, work);
    }
    else {
        ok = (*erode)(src_buf, dim, dst_buf, dim, width,
                      height, width, height, work);
    }
    if (ok < 0) {
        DBG("Got FAIL return value\n");
        goto Done;
    }

    /* Clear all padding */
    rapp_pad_align_bin(src_buf, dim, 0, width, height, 0);

    /* Check the result */
    ok = rapp_stat_sum_bin(src_buf, dim, width, height) == 1 &&
         rapp_pixel_get_bin(src_buf, dim, 0, width / 2, height / 2) == 1;
    if (!ok) {
        DBG("Failed\n");
        DBG("buf=\n");
        rapp_test_dump_bin(src_buf, dim, 0, width, height);
    }

Done:
    rapp_free(dst_pad);
    rapp_free(src_pad);
    rapp_free(work);

    return ok;
}

/**
 *  Check a rectangular erosion or dilation operation.
 *  This is checked:
 *    - The SE pattern must be rectangular with the correct dimensions.
 *    - No boundary artifacts due to the decomposition scheme occur.
 */
static bool
rapp_test_rectangle_driver(int (*morph)(), int width,
                           int height, bool dilate)
{
    uint8_t *pat_buf = NULL;  /* Pattern buffer                */
    uint8_t *pad_buf = NULL;  /* Source buffer with padding    */
    uint8_t *src_buf = NULL;  /* Source buffer                 */
    uint8_t *dst_buf = NULL;  /* Destination buffer            */
    uint8_t *ref_buf = NULL;  /* Reference buffer              */
    void    *work    = NULL;  /* Working buffer (scratch)      */
    int      src_dim;         /* Source buffer dimension       */
    int      dst_dim;         /* Destination buffer dimension  */
    int      pad_len;         /* Padded source buffer in bytes */
    int      size    = MIN(width, height);
    int      pos;
    bool     ok = false;

    /* Allocate the buffers */
    dst_dim = rapp_align((width + 7) / 8);
    src_dim = dst_dim + 2*rapp_alignment;
    pat_buf = rapp_malloc(height*dst_dim, 0);
    pad_len = (height + 2*16)*src_dim;
    pad_buf = rapp_malloc(pad_len, 0);
    src_buf = &pad_buf[16*src_dim + rapp_alignment];
    dst_buf = rapp_malloc(height*dst_dim, 0);
    ref_buf = rapp_malloc(height*dst_dim, 0);
    work    = rapp_malloc(rapp_morph_worksize_bin(width, height), 0);

    /* Generate the rectangular pattern image */
    memset(pat_buf, 0xff, height*dst_dim);
    rapp_pad_align_bin(pat_buf, dst_dim, 0, width, height, 0);

    /* Verify that we get an overlap error for overlapping buffers */
    if (/* src == dst */
        (*morph)(dst_buf, dst_dim, dst_buf, src_dim, width, height,
                 width, height, work) != RAPP_ERR_OVERLAP
        /* src = far end of dst */
        || (*morph)(dst_buf, dst_dim,
                    dst_buf + dst_dim*(height - 1) +
                    rapp_align((width + 7) / 8) - rapp_alignment, src_dim,
                    width, height, width, height, work) != RAPP_ERR_OVERLAP
        /* src = before dst, but not long enough */
        || (*morph)(dst_buf, dst_dim,
                    dst_buf - (dst_dim*(height - 1) +
                               rapp_align((width + 7) / 8) - rapp_alignment),
                    src_dim,
                    width, height, width, height, work) != RAPP_ERR_OVERLAP
        /* dst = work */
        || (*morph)(work, dst_dim, src_buf, src_dim, width, height,
                    width, height, work) != RAPP_ERR_OVERLAP
        /* dst = far end of work */
        || (*morph)(work + rapp_morph_worksize_bin(width, height) - 1, dst_dim,
                    src_buf, src_dim,
                    width, height, width, height, work) != RAPP_ERR_OVERLAP
        /* src = work */
        || (*morph)(dst_buf, dst_dim, work, src_dim, width, height,
                    width, height, work) != RAPP_ERR_OVERLAP
        /* src = far end of work */
        || (*morph)(dst_buf, dst_dim,
                    work + rapp_morph_worksize_bin(width, height) - 1, src_dim,
                    width, height, width, height, work) != RAPP_ERR_OVERLAP)
    {
        DBG("Overlap undetected\n");
        goto Done;
    }

    /* Check all operations on a pixel along the two diagonals */
    for (pos = 0; pos < size; pos++) {
        int xpos = pos;
        int k;

        for (k = 0; k < 2; k++) {
            int ypos = k ? pos : size - pos - 1;

            /* Initialize buffers */
            memset(pad_buf, dilate ? 0 : 0xff, pad_len);
            memset(dst_buf, dilate ? 0xff : 0, height*dst_dim);
            memset(ref_buf, 0, height*dst_dim);

            /* Set position pixel in the test buffer */
            rapp_pixel_set_bin(src_buf, src_dim, 0, xpos, ypos, dilate);

            /* Call morphological function */
            if ((*morph)(dst_buf, dst_dim, src_buf, src_dim,
                         width, height, width, height, work) < 0)
            {
                DBG("Got FAIL return value\n");
                goto Done;
            }

            /* Blit the pattern to the reference buffer */
            rapp_test_blit_pattern(ref_buf, pat_buf, dst_dim, width, height,
                                   xpos - (width  - !dilate) / 2,
                                   ypos - (height - !dilate) / 2);

            /* Invert the reference buffer on erosion */
            if (!dilate) {
                rapp_pixop_not_u8(ref_buf, dst_dim, dst_dim, height);
            }

            /* Check the result */
            if (!rapp_test_compare_bin(ref_buf, dst_dim,
                                       dst_buf, dst_dim,
                                       0, width, height))
            {
                DBG("Invalid result\n");
                DBG("dst=\n");
                rapp_test_dump_bin(dst_buf, dst_dim, 0, width, height);
                DBG("ref=\n");
                rapp_test_dump_bin(ref_buf, dst_dim, 0, width, height);
                goto Done;
            }
        }
    }

    ok = true;

Done:
    rapp_free(pat_buf);
    rapp_free(pad_buf);
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    rapp_free(work);

    return ok;
}

/**
 *  Check an isotropic (diamond/octagon/disc) erosion or dilation operation.
 *  This is checked:
 *    - The SE pattern must be rectangular with the correct dimensions.
 *    - No boundary artifacts due to the decomposition scheme occur.
 */
static bool
rapp_test_isotropic_driver(int (*morph)(), int radius, int area, bool dilate)
{
    uint8_t *pat_buf = NULL;  /* Pattern buffer                */
    uint8_t *pad_buf = NULL;  /* Source buffer with padding    */
    uint8_t *src_buf = NULL;  /* Source buffer                 */
    uint8_t *dst_buf = NULL;  /* Destination buffer            */
    uint8_t *ref_buf = NULL;  /* Reference buffer              */
    void    *work    = NULL;  /* Working buffer (scratch)      */
    int      src_dim;         /* Source buffer dimension       */
    int      dst_dim;         /* Destination buffer dimension  */
    int      pad_len;         /* Padded source buffer in bytes */
    int      pos;
    int      size    = 2*radius - 1;
    bool     ok      = false;

    /* Get the SE pattern image */
    rapp_test_get_pattern(&pat_buf, &dst_dim, morph, radius, dilate);

    /* Check the pattern area */
    if (rapp_stat_sum_bin(pat_buf, dst_dim, size, size) != area) {
        DBG("Invalid pattern area\n");
        goto Done;
    }

    /* Allocate the buffers */
    src_dim = dst_dim + 2*rapp_alignment;
    pad_len = (size + 2*16)*src_dim;
    pad_buf = rapp_malloc(pad_len, 0);
    src_buf = &pad_buf[16*src_dim + rapp_alignment];
    dst_buf = rapp_malloc(size*dst_dim, 0);
    ref_buf = rapp_malloc(size*dst_dim, 0);
    work    = rapp_malloc(rapp_morph_worksize_bin(size, size), 0);

    /* Verify that we get an overlap error for overlapping buffers */
    if (/* src == dst */
        (*morph)(dst_buf, dst_dim, dst_buf, src_dim, size, size, radius,
                 work) != RAPP_ERR_OVERLAP
        /* src = far end of dst */
        || (*morph)(dst_buf, dst_dim,
                    dst_buf + dst_dim*(size - 1) +
                    rapp_align((size + 7) / 8) - rapp_alignment, src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP
        /* src = before dst, but not long enough */
        || (*morph)(dst_buf, dst_dim,
                    dst_buf - (dst_dim*(size - 1) +
                               rapp_align((size + 7) / 8) - rapp_alignment),
                    src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP
        /* dst = work */
        || (*morph)(work, dst_dim, src_buf, src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP
        /* dst = far end of work */
        || (*morph)(work + rapp_morph_worksize_bin(size, size) - 1, dst_dim,
                    src_buf, src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP
        /* src = work */
        || (*morph)(dst_buf, dst_dim, work, src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP
        /* src = far end of work */
        || (*morph)(dst_buf, dst_dim,
                    work + rapp_morph_worksize_bin(size, size) - 1, src_dim,
                    size, size, radius, work) != RAPP_ERR_OVERLAP)
    {
        DBG("Overlap undetected\n");
        goto Done;
    }

    /* Check all operations on a pixel along the two diagonals */
    for (pos = 0; pos < size; pos++) {
        int xpos = pos;
        int k;

        for (k = 0; k < 2; k++) {
            int ypos = k ? pos : size - pos - 1;

            /* Initialize buffers */
            memset(pad_buf, dilate ? 0 : 0xff, pad_len);
            memset(dst_buf, dilate ? 0xff : 0, size*dst_dim);
            memset(ref_buf, 0, size*dst_dim);

            /* Set position pixel in the test buffer */
            rapp_pixel_set_bin(src_buf, src_dim, 0, xpos, ypos, dilate);

            /* Call morphological function */
            if ((*morph)(dst_buf, dst_dim, src_buf, src_dim,
                         size, size, radius, work) < 0)
            {
                DBG("Got FAIL return value\n");
                goto Done;
            }

            /* Blit the pattern to the reference buffer */
            rapp_test_blit_pattern(ref_buf, pat_buf, dst_dim, size, size,
                                   xpos - radius + 1, ypos - radius + 1);

            /* Invert the reference buffer on erosion */
            if (!dilate) {
                rapp_pixop_not_u8(ref_buf, dst_dim, dst_dim, size);
            }

            /* Check the result */
            if (!rapp_test_compare_bin(ref_buf, dst_dim,
                                       dst_buf, dst_dim,
                                       0, size, size))
            {
                DBG("Invalid result\n");
                DBG("dst=\n");
                rapp_test_dump_bin(dst_buf, dst_dim, 0, size, size);
                DBG("ref=\n");
                rapp_test_dump_bin(ref_buf, dst_dim, 0, size, size);
                goto Done;
            }
        }
    }

    ok = true;

Done:
    rapp_free(pat_buf);
    rapp_free(pad_buf);
    rapp_free(dst_buf);
    rapp_free(ref_buf);
    rapp_free(work);

    return ok;
}
