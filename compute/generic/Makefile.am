#  Automake file for the RAPP Compute generic (scalar) implementations.

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

# Include the RAPP Compute API and the common functionality headers
AM_CPPFLAGS += -I../include           \
               -I$(srcdir)/../include \
               -I$(srcdir)/../common

# The RAPP Compute generic convenience library
noinst_LTLIBRARIES = librappcompute_gen.la

librappcompute_gen_la_LDFLAGS = -no-undefined

# The source files to use
librappcompute_gen_la_SOURCES = \
 rc_impl_cfg.h \
 rc_thresh_tpl.h \
 rc_thresh_pixel_tpl.h \
 rc_bitblt_rop.h \
 rc_bitblt_wa.c \
 rc_bitblt_wm.c \
 rc_pixop.c \
 rc_type.c \
 rc_thresh.c \
 rc_stat.c \
 rc_moment_bin.c \
 rc_reduce.c \
 rc_reduce_bin.c \
 rc_expand_bin.c \
 rc_rotate.c \
 rc_rotate_bin.c \
 rc_filter.c \
 rc_morph_bin.c \
 rc_fill.c \
 rc_pad.c \
 rc_pad_bin.c \
 rc_margin.c \
 rc_crop.c \
 rc_contour.c \
 rc_rasterize.c \
 rc_cond.c \
 rc_gather.c \
 rc_gather_bin.c \
 rc_scatter.c \
 rc_scatter_bin.c \
 rc_integral.c \
 rc_integral_bin.c
