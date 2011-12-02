#  Autoconf file for the installation check test for the RAPP library.

#  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
#
#  This file is part of RAPP.
#
#  RAPP is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  You can use the comments under either the terms of the GNU Lesser General
#  Public License version 3 as published by the Free Software Foundation,
#  either version 3 of the License or (at your option) any later version, or
#  the GNU Free Documentation License version 1.3 or any later version
#  published by the Free Software Foundation; with no Invariant Sections, no
#  Front-Cover Texts, and no Back-Cover Texts.
#  A copy of the license is included in the documentation section entitled
#  "GNU Free Documentation License".
#
#  RAPP is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License and a copy of the GNU Free Documentation License along
#  with RAPP. If not, see <http://www.gnu.org/licenses/>.

# Initialize
AC_INIT([RAPPinstalltest], [0.0])
AC_CONFIG_SRCDIR([closing.c])
AC_CONFIG_AUX_DIR([../../config])
AC_CONFIG_MACRO_DIR([../../m4])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Save the user CFLAGS before AC_PROG_C89 modifies it
USER_CFLAGS=$CFLAGS

# Check for programs
LT_INIT
AC_PROG_CC_C89
AC_PROG_LN_S
AC_PROG_MKDIR_P

# For checking syntax correctness of header-files.
AC_PROG_CXX

# Restore user CFLAGS
CFLAGS=$USER_CFLAGS

# Check for headers
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_CHECK_HEADER([stdint.h],,
  [AC_MSG_ERROR([[Required header stdint.h missing]])])

AC_CHECK_HEADER([rapp/rapp.h],,
  [AC_MSG_ERROR([[rapp/rapp.h isn't installed]])])

AC_CHECK_LIB([rapp],[rapp_initialize],,
  [AC_MSG_ERROR([[librapp isn't installed]])])

# Check C compiler vendor
AX_COMPILER_VENDOR

AX_CFLAGS_GCC_OPTION([-Werror], [AM_CFLAGS])
AX_CFLAGS_GCC_OPTION([-W], [AM_CFLAGS])
AX_CFLAGS_GCC_OPTION([-Wall], [AM_CFLAGS])

AC_SUBST([AM_CFLAGS])

# Output
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
