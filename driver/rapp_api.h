/*  Copyright (C) 2005-2012, Axis Communications AB, LUND, SWEDEN
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

/**
 *  An indirection through a macro is necessary to avoid the
 *  preprocessor seeing a comma in the argument list of a macro.
 */
#define RAPP_COMMA ,

#ifdef RAPP_LOGGING
/* We need gettimeofday when logging. */
#include <sys/time.h>
#define RAPP_LOG_DOIT rapp_log_rappcall
extern void RAPP_LOG_DOIT(const char[], const struct timeval[],
                          const char[], const char[], ...) RAPP_EXPORT;

/**
 *  The generated file rapp_logdefs.h provides the
 *  RAPP_LOG_<something>_<id> macros for each function <id> to which
 *  RAPP_API is applied.
 */

#include "rapp_logdefs.h"

#define RAPP_LOG_STUB_DECL(t, id, args) \
  static t id ## __logged args;

#define RAPP_LOG_STUB(t, id, args)                                      \
{                                                                       \
  RAPP_LOG_IDENTITY_IF_RETURN_ ## id(t val_;)                           \
  struct timeval tv_[2];                                                \
  int ret_ = gettimeofday(&tv_[0], NULL);                               \
  assert(ret_ == 0);                                                    \
  RAPP_LOG_IDENTITY_IF_RETURN_ ## id(val_ =)                            \
      id ## __logged(RAPP_LOG_ARGLIST_ ## id);                          \
  ret_ = gettimeofday(&tv_[1], NULL);                                   \
  assert(ret_ == 0);                                                    \
  RAPP_LOG_DOIT(#id, tv_, RAPP_LOG_RETFORMAT_ ## id,                    \
                RAPP_LOG_ARGSFORMAT_ ## id                              \
                RAPP_LOG_IDENTITY_IF_ARGS_ ## id(RAPP_COMMA)            \
                RAPP_LOG_ARGLIST_ ## id                                 \
                RAPP_LOG_IDENTITY_IF_RETURN_ ## id(RAPP_COMMA val_));   \
  RAPP_LOG_IDENTITY_IF_RETURN_ ## id(return val_;)                      \
  (void)ret_; /* Avoid set-but-unused warnings. */                      \
}                                                                       \
static t id ## __logged args

#else /* !RAPP_LOGGING */
#define RAPP_LOG_STUB_DECL(t, id, args)
#define RAPP_LOG_STUB(t, id, args)
#endif /* !RAPP_LOG */

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
    RAPP_LOG_STUB_DECL(type, name, args)                                   \
    type name ## __base args                                               \
    RAPP_LOG_STUB(type, name, args)
#else
#define RAPP_API(rtype, name, args)             \
    RAPP_EXPORT rtype name args;                \
    RAPP_LOG_STUB_DECL(rtype, name, args)       \
    rtype name args                             \
    RAPP_LOG_STUB(rtype, name, args)
#endif

#endif /* RAPP_API_H */
