/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_error.c
 *  @brief  RAPP error handling.
 */

#include "rapp_api.h"        /* API symbol macro   */
#include "rapp_util.h"       /* RAPP_OK            */
#include "rapp_error.h"      /* Exported error API */
#include "rapp_error_int.h"  /* Internal error API */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Determine if a value is aligned.
 */
#define RAPP_ERROR_ALIGNED(val) \
    ((uintptr_t)(val) % RC_ALIGNMENT != 0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Get the error string for a given code.
 */
RAPP_API(const char*, rapp_error, (int code))
{
    static const char *desc[] = {
        "No error",                           /* RAPP_OK             */
        "Pixel buffer pointer is NULL",       /* RAPP_ERR_BUF_NULL   */
        "Invalid pixel buffer alignment",     /* RAPP_ERR_BUF_ALIGN  */
        "Invalid pixel buffer bit offset",    /* RAPP_ERR_BUF_OFFSET */
        "Invalid buffer dimension alignment", /* RAPP_ERR_DIM_ALIGN  */
        "Invalid image size",                 /* RAPP_ERR_IMG_SIZE   */
        "Parameter is NULL",                  /* RAPP_ERR_PARM_NULL  */
        "Parameter is out-of-range",          /* RAPP_ERR_PARM_RANGE */
        "Missing call to rapp_initialize()",  /* RAPP_ERR_UNINITIALIZED */
        "Buffer parameters overlap",          /* RAPP_ERR_OVERLAP    */
        "Invalid error code"                  /* Invalid             */
    };

    return desc[CLAMP(-code, 0, (int)(sizeof desc / sizeof *desc) - 1)];
}


/*
 * -------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------
 */

/**
 *  Get the error code for a binary image.
 */
int
rapp_error_bin(const uint8_t *buf, int dim, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(buf)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_ALIGN;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (width <= 0              ||
        ((width + 7) / 8) > dim ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}

/**
 *  Get the error code for a binary image without alignment.
 */
int
rapp_error_noalign_bin(const uint8_t *buf, int dim,
                       int off, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (off < 0 || off > 7) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_OFFSET;
    }
    if (width <= 0              ||
        ((width + 7) / 8) > dim ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}

/**
 *  Get the error code for an 8-bit image.
 */
int
rapp_error_u8(const uint8_t *buf, int dim, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(buf)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_ALIGN;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (width  <= 0  ||
        width  > dim ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}

/**
 *  Get the error code for a 16-bit image.
 */
int
rapp_error_u16(const uint16_t *buf, int dim, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(buf)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_ALIGN;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (width  <= 0  ||
        width  > dim / 2 ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}

/**
 *  Get the error code for a 32-bit image.
 */
int
rapp_error_u32(const uint32_t *buf, int dim, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(buf)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_ALIGN;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (width  <= 0  ||
        width  > dim / 4 ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}


/**
 *  Get the error code for an 8-bit image without alignment.
 */
int
rapp_error_noalign_u8(const uint8_t *buf, int dim, int width, int height)
{
    if (!buf) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_BUF_NULL;
    }
    if (RAPP_ERROR_ALIGNED(dim)) {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_DIM_ALIGN;
    }
    if (width  <= 0  ||
        width  > dim ||
        height <= 0)
    {
        RAPP_ABORT_FOR_ASSERTED_RETURNS();
        return RAPP_ERR_IMG_SIZE;
    }

    return RAPP_OK;
}

/**
 *  Get the error code for two binary images.
 */
int
rapp_error_bin_bin(const uint8_t *buf1, int dim1, int width1, int height1,
                   const uint8_t *buf2, int dim2, int width2, int height2)
{
    int err = rapp_error_bin(buf1, dim1, width1, height1);

    if (!err) {
        err = rapp_error_bin(buf2, dim2, width2, height2);
    }

    return err;
}

/**
 *  Get the error code for two binary images without alignment.
 */
int
rapp_error_noalign_bin_bin(const uint8_t *buf1, int dim1, int off1,
                           const uint8_t *buf2, int dim2, int off2,
                           int width, int height)
{
    int err = rapp_error_noalign_bin(buf1, dim1, off1, width, height);

    if (!err) {
        err = rapp_error_noalign_bin(buf2, dim2, off2, width, height);
    }

    return err;
}

/**
 *  Get the error code for a binary image and an 8-bit image.
 */
int
rapp_error_bin_u8(const uint8_t *bin, int bin_dim,
                  const uint8_t *u8,  int u8_dim,
                  int width, int height)
{
    int err = rapp_error_bin(bin, bin_dim, width, height);

    if (!err) {
        err = rapp_error_u8(u8, u8_dim, width, height);
    }

    return err;
}

/**
 *  Get the error code for an 8-bit image and a binary image.
 */
int
rapp_error_u8_bin(const uint8_t *u8,  int u8_dim,
                  const uint8_t *bin, int bin_dim,
                  int width, int height)
{
    int err = rapp_error_u8(u8, u8_dim, width, height);

    if (!err) {
        err = rapp_error_bin(bin, bin_dim, width, height);
    }

    return err;
}

/**
 *  Get the error code for a binary image and a 16-bit image.
 */
int
rapp_error_bin_u16(const uint8_t *bin, int bin_dim,
                   const uint16_t *u16, int u16_dim,
                   int width, int height)
{
    int err = rapp_error_bin(bin, bin_dim, width, height);

    if (!err) {
        err = rapp_error_u16(u16, u16_dim, width, height);
    }

    return err;
}

/**
 *  Get the error code for a binary image and a 32-bit image.
 */
int
rapp_error_bin_u32(const uint8_t *bin, int bin_dim,
                   const uint32_t *u32, int u32_dim,
                   int width, int height)
{
    int err = rapp_error_bin(bin, bin_dim, width, height);

    if (!err) {
        err = rapp_error_u32(u32, u32_dim, width, height);
    }

    return err;
}
/**
 *  Get the error code for two 8-bit images.
 */
int
rapp_error_u8_u8(const uint8_t *buf1, int dim1, int width1, int height1,
                 const uint8_t *buf2, int dim2, int width2, int height2)
{
    int err = rapp_error_u8(buf1, dim1, width1, height1);

    if (!err) {
        err = rapp_error_u8(buf2, dim2, width2, height2);
    }

    return err;
}

/**
 *  Get the error code for an 8-bit image and a 16-bit image.
 */
int
rapp_error_u8_u16(const uint8_t *buf1, int dim1, int width1, int height1,
                  const uint16_t *buf2, int dim2, int width2, int height2)
{
    int err = rapp_error_u8(buf1, dim1, width1, height1);

    if (!err) {
        err = rapp_error_u16(buf2, dim2, width2, height2);
    }

    return err;
}

/**
 *  Get the error code for an 8-bit image and a 32-bit image.
 */
int
rapp_error_u8_u32(const uint8_t *buf1, int dim1, int width1, int height1,
                  const uint32_t *buf2, int dim2, int width2, int height2)
{
    int err = rapp_error_u8(buf1, dim1, width1, height1);

    if (!err) {
        err = rapp_error_u32(buf2, dim2, width2, height2);
    }

    return err;
}

#if RC_ASSERTED_RETURNS
/**
 *  Call abort.
 *  Indirected to improve backtrace quality for gcc versions where calls
 *  to abort() otherwise negatively affect the debug quality of the
 *  current stack-frame.
 */
void
rapp_abort(void)
{
  abort();
}
#endif /* RC_ASSERTED_RETURNS */
