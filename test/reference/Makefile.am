#  Automake file for the RAPP library reference implementation.

#  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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

# Include the RAPP headers
AM_CPPFLAGS += -I$(top_srcdir)/include \
               -I$(top_srcdir)/compute/include \
               -I$(top_builddir)/include

# The reference library
check_LTLIBRARIES = librappref.la

librappref_la_LIBADD = ../../librapp.la

# The reference sources
librappref_la_SOURCES = \
 rapp_ref_bitblt.h \
 rapp_ref_bitblt.c \
 rapp_ref_pixop.h \
 rapp_ref_pixop.c \
 rapp_ref_type.h \
 rapp_ref_type.c \
 rapp_ref_thresh.h \
 rapp_ref_thresh.c \
 rapp_ref_reduce.h \
 rapp_ref_reduce.c \
 rapp_ref_reduce_bin.h \
 rapp_ref_reduce_bin.c \
 rapp_ref_expand_bin.h \
 rapp_ref_expand_bin.c \
 rapp_ref_rotate.h \
 rapp_ref_rotate.c \
 rapp_ref_rotate_bin.h \
 rapp_ref_rotate_bin.c \
 rapp_ref_stat.h \
 rapp_ref_moment_bin.h \
 rapp_ref_moment_bin.c \
 rapp_ref_stat.c \
 rapp_ref_filter.h \
 rapp_ref_filter.c \
 rapp_ref_fill.h \
 rapp_ref_fill.c \
 rapp_ref_pad.h \
 rapp_ref_pad.c \
 rapp_ref_pad_bin.h \
 rapp_ref_pad_bin.c \
 rapp_ref_margin.h \
 rapp_ref_margin.c \
 rapp_ref_crop.h \
 rapp_ref_crop.c \
 rapp_ref_contour.h \
 rapp_ref_contour.c \
 rapp_ref_cond.h \
 rapp_ref_cond.c \
 rapp_ref_gather.h \
 rapp_ref_gather.c \
 rapp_ref_gather_bin.h \
 rapp_ref_gather_bin.c \
 rapp_ref_scatter.h \
 rapp_ref_scatter.c \
 rapp_ref_scatter_bin.h \
 rapp_ref_scatter_bin.c \
 rapp_ref_integral.h \
 rapp_ref_integral.c
