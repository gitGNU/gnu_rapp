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
 *  @file   rapp_api.h
 *  @brief  API symbol definition macros.
 */

#ifndef RAPP_API_H
#define RAPP_API_H

#include <assert.h>
#include "rapp_export.h" /* RAPP_EXPORT */

extern int rc_initialized;

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  API symbol definition macro.
 *  If the "weak" and "alias" attributes are available,
 *  we define the exported symbol as a weak alias to the
 *  exported stable symbol that defines function.
 */
#if defined __ELF__ && __GNUC__ >= 3
#define RAPP_API(type, name, args)                                         \
    type name args RAPP_EXPORT __attribute__((__weak__,                    \
                                              __alias__(#name "__base"))); \
    type name ## __base args RAPP_EXPORT;                                  \
    type name ## __base args

#else
#define RAPP_API(rtype, name, args) \
    RAPP_EXPORT rtype name args;    \
    rtype name args

#endif

#endif /* RAPP_API_H */
