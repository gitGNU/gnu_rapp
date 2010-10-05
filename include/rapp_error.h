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
 *  @file   rapp_error.h
 *  @brief  RAPP error handling.
 */

/**
 *  @defgroup grp_error Error Handling
 *  @brief Error codes and descriptions.
 *
 *  Errors are indicated by the return value of the function. Only
 *  argument errors are possible &ndash; functions have no internal
 *  failure modes (besides when RAPP is not @ref grp_init "initialized").
 *  A negative value indicate an error condition. In case of several
 *  errors after initialization, the first one in argument-order is
 *  returned. This number can be passed to the rapp_error() function
 *  to obtain a string describing the error condition.
 *
 *  <p>@ref grp_pixel "Next section: Pixel Access"</p>
 *
 *  @{
 */

#ifndef RAPP_ERROR_H
#define RAPP_ERROR_H

#include <stdint.h> /* uint8_t */
#include "rapp_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -------------------------------------------------------------
 *  Error codes
 * -------------------------------------------------------------
 */

/**
 *  Pixel buffer pointer is NULL.
 */
#define RAPP_ERR_BUF_NULL   -1

/**
 *  Invalid pixel buffer alignment.
 */
#define RAPP_ERR_BUF_ALIGN  -2

/**
 *  Invalid pixel buffer bit offset.
 */
#define RAPP_ERR_BUF_OFFSET -3

/**
 *  Invalid buffer dimension alignment.
 */
#define RAPP_ERR_DIM_ALIGN  -4

/**
 *  Invalid image size.
 */
#define RAPP_ERR_IMG_SIZE   -5

/**
 *  Parameter is NULL.
 */
#define RAPP_ERR_PARM_NULL  -6

/**
 *  Parameter is out-of-range.
 */
#define RAPP_ERR_PARM_RANGE -7

/**
 *  Missing call to rapp_initialize().
 */
#define RAPP_ERR_UNINITIALIZED -8

/**
 *  Buffer parameters overlap.
 */
#define RAPP_ERR_OVERLAP -9


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Get a description string for a given error code.
 *  @param code  A negative error code from a RAPP function.
 *  @return      A C (NUL-terminated) string from constant memory
 *               describing the error condition.  This function is
 *               valid to call also when RAPP is not initialized.
 */
RAPP_EXPORT const char*
rapp_error(int code);

#ifdef __cplusplus
};
#endif

#endif /* RAPP_ERROR_H */
/** @} */
