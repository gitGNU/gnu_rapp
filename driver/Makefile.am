#  Automake file for the RAPP driver directory.

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

# Include exported API headers and RAPP Compute headers
AM_CPPFLAGS += -I../include            \
               -I../compute/include    \
               -I$(top_srcdir)/include \
               -I$(top_srcdir)/compute/include

# The RAPP driver convenience library
noinst_LTLIBRARIES = librappdriver.la

librappdriver_la_LDFLAGS = -no-undefined

# The RAPP driver source files
librappdriver_la_SOURCES = rapp_api.h         \
                           rapp_util.h        \
                           rapp_main.c        \
                           rapp_version.c     \
                           rapp_info.c        \
                           rapp_error_int.h   \
                           rapp_error.c       \
                           rapp_malloc.c      \
                           rapp_pixel.c       \
                           rapp_bitblt.c      \
                           rapp_pixop.c       \
                           rapp_type.c        \
                           rapp_thresh.c      \
                           rapp_reduce.c      \
                           rapp_reduce_bin.c  \
                           rapp_expand_bin.c  \
                           rapp_rotate.c      \
                           rapp_rotate_bin.c  \
                           rapp_filter.c      \
                           rapp_stat.c        \
                           rapp_moment_bin.c  \
                           rapp_fill.c        \
                           rapp_pad.c         \
                           rapp_pad_bin.c     \
                           rapp_morph_bin.c   \
                           rapp_margin.c      \
                           rapp_crop.c        \
                           rapp_contour.c     \
                           rapp_rasterize.c   \
                           rapp_cond.c        \
                           rapp_gather.c      \
                           rapp_gather_bin.c  \
                           rapp_scatter.c     \
                           rapp_scatter_bin.c \
                           rapp_integral.c
