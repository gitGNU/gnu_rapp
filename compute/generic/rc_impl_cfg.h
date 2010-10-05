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
 *  @file   rc_impl_cfg.h
 *  @brief  RAPP Compute layer generic implementation config.
 */

#ifndef RC_IMPL_CFG_H
#define RC_IMPL_CFG_H

#include "rc_impl.h" /* Implementation names */

/**
 *  Define the implementation selection macro.
 */
#ifdef RAPP_FORCE_GENERIC

/* Select all generic implementations */
#define RC_IMPL(func, unroll) \
     (RC_UNROLL(func) == 1 || (unroll))

#elif defined RAPP_FORCE_SIMD || defined RAPP_FORCE_SWAR

/* Discard all generic implementations */
#define RC_IMPL(func, unroll) 0

#else

/* Select implementation from tuning file */
#include "rapptune.h"
#define RC_IMPL(func, unroll)          \
    ((func ## _IMPL == RC_IMPL_GEN) && \
     (RC_UNROLL(func) == 1 || (unroll)))
#endif

/**
 *  Define the unroll factor macro.
 */
#ifdef RAPP_FORCE_UNROLL
#define RC_UNROLL(func) RAPP_FORCE_UNROLL
#else
#include "rapptune.h"
#define RC_UNROLL(func) (func ## _UNROLL)
#endif

#endif /* RC_IMPL_CFG_H */
