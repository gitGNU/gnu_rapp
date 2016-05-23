#  Automake file for the RAPP Compute vector implementations.

#  Copyright (C) 2005-2010, Axis Communications AB, LUND, SWEDEN
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

# Include the RAPP Compute API and the common functionality headers
AM_CPPFLAGS += -I../include           \
               -I$(srcdir)/../include \
               -I$(srcdir)/../common  \
               -I$(srcdir)/../backend

# The RAPP Compute vector convenience libraries
noinst_LTLIBRARIES = librappcompute_swar.la \
                     librappcompute_simd.la

librappcompute_swar_la_LDFLAGS = -no-undefined
librappcompute_simd_la_LDFLAGS = -no-undefined

# The source files are the same for both vector implementations
librappcompute_swar_la_SOURCES = \
 rc_impl_cfg.h \
 rc_bitblt_rop.h \
 rc_bitblt_va.c \
 rc_bitblt_vm.c \
 rc_pixop.c \
 rc_type.c \
 rc_thresh.c \
 rc_thresh_tpl.h \
 rc_thresh_pixel_tpl.h \
 rc_reduce.c \
 rc_stat.c \
 rc_filter.c \
 rc_margin.c

librappcompute_simd_la_SOURCES = $(librappcompute_swar_la_SOURCES)

# Make RAPP Compute use non-SWAR vector backend, if available
librappcompute_simd_la_CPPFLAGS  = $(AM_CPPFLAGS) -DRAPP_USE_SIMD
