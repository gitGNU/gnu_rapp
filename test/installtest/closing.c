/*  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
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
 *  @file   hello.c
 *  @brief  A "hello world" test, trivial square closing.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <rapp/rapp.h>

static void
fatal_err(const char *msg, int err)
{
    fprintf(stderr, msg, err, rapp_error(err));
    abort();
}

static void
fatal(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fflush(stderr);
    abort();
}

/* Global, to avoid it being optimized away; for calling from a debugger. */

void
dump(uint8_t *buf, int dim, int width, int height)
{
    int x,y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++)
	    putc(rapp_pixel_get_bin(buf, dim, 0, x, y) ? '1' : '0', stderr);
        putc('\n', stderr);
    }
}

#undef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))

/**
 *  Dilate, then erode a single point at the centre of the image with a
 *  square and verify the result.
 */

static void
do_closing(void)
{
    int width = 5, height = 5, side = 2;
    int center_x = width / 2, center_y = height / 2;
    int padding = MIN((side + 1) / 2, 16);
    /**
     *  We pad each side by itself, so we can have the image inside the
     *  padding at offset 0.
     */
    int horiz_padsize = rapp_align((padding + 7) / 8);
    int dim = rapp_align((width + 7) / 8) + 2 * horiz_padsize;
    int padded_height = height + padding * 2;
    int size_from_start = dim * padded_height;
    uint8_t *src_buf_start = rapp_malloc(size_from_start, 0);
    uint8_t *src_buf = src_buf_start + dim * padding + horiz_padsize;
    uint8_t *dst_buf_start = rapp_malloc(size_from_start, 0);
    uint8_t *dst_buf = dst_buf_start + dim * padding + horiz_padsize;
    uint8_t *ref_buf_start = rapp_malloc(size_from_start, 0);
    uint8_t *ref_buf = ref_buf_start + dim * padding + horiz_padsize;
    void *work = rapp_malloc(rapp_morph_worksize_bin(side, side), 0);
    int err;
    int x, y;

    /* We create a one-dot image with the given height and width. */
    memset(src_buf_start, 0, size_from_start);
    err = rapp_pixel_set_bin(src_buf, dim, 0, center_x, center_y, 1);

    if (err != 0)
        fatal_err("error %d setting pixel: %s\n", err);

    /**
     *  Set pixels around the center in the reference, corresponding to
     *  the structuring element.
     */
    memset(ref_buf_start, 0, size_from_start);
    for (y = 0; y < side; y++)
        for (x = 0; x < side; x++) {
	    err = rapp_pixel_set_bin(ref_buf, dim, 0,
				     center_x - side/2 + x,
				     center_y - side/2 + y, 1);

	    if (err != 0)
	        fatal_err("error %d setting pixel: %s\n", err);
	}

    /**
     *  Then, dilate the single dot. The initial memset guarantees that
     *  border pixels are 0, else we'd have to call rapp_pad_const_bin.
     */
    err = rapp_morph_dilate_rect_bin(dst_buf, dim, src_buf, dim, width, height,
				     side, side, work);

    if (err != 0)
        fatal_err("error %d dilating: %s\n", err);

    /**
     *  Check that we've got the expected square. Don't check inside the
     *  padding.
     */
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++) {
	    int val_dst = rapp_pixel_get_bin(dst_buf, dim, 0, x, y);
	    int val_ref = rapp_pixel_get_bin(ref_buf, dim, 0, x, y);

	    if (val_dst < 0)
	        fatal("error %d getting dilated pixel: %s at (%d,%d)\n",
		      val_dst, rapp_error(val_dst), x, y);

	    if (val_ref < 0)
	        fatal("error %d getting dilated pixel: %s at (%d,%d)\n",
		      val_ref, rapp_error(val_ref), x, y);

	    if (val_dst != val_ref)
	        fatal("Got a %d-pixel after dilation,"
		      " expected %d at (%d,%d)/(%d,%d)\n",
		      val_dst, val_ref, x, y, width, height);
	}

    /**
     *  Set the now-dilated border pixels to 1.  If we didn't, it
     *  probably wouldn't make a difference when we do erosion later, as
     *  we'll expect 0-pixels at the border and we started with a single
     *  pixel in an otherwise empty buffer, but for the general case, this
     *  needs be done (for example, for width = height = 3 and side = 2).
     *  Then we'd probably also want to make sure we have a clear border
     *  the size of the padding, to avoid small objects near the border
     *  "sticking" to it.
     */ 
    err = rapp_pad_const_bin(dst_buf, dim, 0, width, height, padding, 1);
    if (err != 0)
        fatal_err("error %d padding dilated image: %s\n", err);

    /**
     *  Erode the dilation result. We abuse the reference buffer as the
     *  result.
     */
    err = rapp_morph_erode_rect_bin(ref_buf, dim, dst_buf, dim, width, height,
				    side, side, work);
    if (err != 0)
        fatal_err("error %d eroding image: %s\n", err);

    /* And finally, verify that we're back at the single dot. */
    for (x = 0; x < width; x++)
      for (y = 0; y < height; y++) {
	  int val_dst = rapp_pixel_get_bin(ref_buf, dim, 0, x, y);
	  int val_src = rapp_pixel_get_bin(src_buf, dim, 0, x, y);

	  if (val_dst < 0)
	      fatal("error %d getting eroded pixel: %s at (%d,%x)\n",
		    val_dst, rapp_error(val_dst), x, y);

	  if (val_src < 0)
	      fatal("error %d getting original pixel: %s at (%d,%d)\n",
		    val_src, rapp_error(val_src), x, y);

	  if (val_dst != val_src)
	      fatal("Got a %d-pixel after erosion,"
		    " expected %d at (%d,%d)/(%d,%d)\n",
		    val_dst, val_src, x, y, width, height);
      }

    rapp_free(ref_buf_start);
    rapp_free(src_buf_start);
    rapp_free(dst_buf_start);
    rapp_free(work);
}

int
main(void)
{
    rapp_initialize();

    if (rapp_major_version != RAPP_MAJOR_VERSION
        || rapp_minor_version != RAPP_MINOR_VERSION
        || RAPP_MAJOR_VERSION != EXPECTED_MAJOR_VERSION
        || RAPP_MINOR_VERSION != EXPECTED_MINOR_VERSION
        || rapp_info == NULL || strlen(rapp_info) == 0)
        abort();

    fprintf(stderr, "Trivial test; \"closing\" with RAPP build: '%s'\n",
	    rapp_info);

    do_closing();

    rapp_terminate();
    exit(0);
}
