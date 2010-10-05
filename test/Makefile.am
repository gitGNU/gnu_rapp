#  Automake file for the RAPP library unit tests.

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

# Build the reference implementations
SUBDIRS = reference

# Include the needed headers
AM_CPPFLAGS += @CHECK_CFLAGS@                  \
               -I../include                    \
               -I$(top_srcdir)/include         \
               -I$(top_srcdir)/compute/include \
               -I$(srcdir)/reference

# Enable tests if the Check library is present
if HAVE_CHECK
testapp_src = rapp_check.c
else
testapp_src = rapp_test.c
endif

# Set the test program
TESTS = rapptest
check_PROGRAMS = $(TESTS)

# The test sources
rapptest_SOURCES = $(testapp_src)          \
                   rapp_tests.def          \
                   rapp_test_util.h        \
                   rapp_test_util.c        \
                   rapp_test_pixel.c       \
                   rapp_test_bitblt.c      \
                   rapp_test_pixop.c       \
                   rapp_test_type.c        \
                   rapp_test_thresh.c      \
                   rapp_test_reduce.c      \
                   rapp_test_reduce_bin.c  \
                   rapp_test_expand_bin.c  \
                   rapp_test_rotate.c      \
                   rapp_test_rotate_bin.c  \
                   rapp_test_stat.c        \
                   rapp_test_moment_bin.c  \
                   rapp_test_filter.c      \
                   rapp_test_morph_bin.c   \
                   rapp_test_fill.c        \
                   rapp_test_pad.c         \
                   rapp_test_pad_bin.c     \
                   rapp_test_margin.c      \
                   rapp_test_crop.c        \
                   rapp_test_contour.c     \
                   rapp_test_rasterize.c   \
                   rapp_test_cond.c        \
                   rapp_test_gather.c      \
                   rapp_test_gather_bin.c  \
                   rapp_test_scatter.c     \
                   rapp_test_scatter_bin.c \
                   rapp_test_integral.c

# Link with the RAPP library and the reference implementation
rapptest_LDADD = @CHECK_LIBS@ reference/librappref.la

# Additional files to clean
CLEANFILES = rapptest.log

# Distribute the installcheck-directory. It can't be configured
# until after RAPP has been installed, so we can't put it in
# DIST_SUBDIRS.
dist-hook:
	tar -C $(srcdir) --exclude=autom4te.cache \
	  --exclude=.svn --exclude=CVS -c -f - installtest \
	 | tar -C $(distdir) -x -f -

INSTALLCHECK_CONFIGURE_OPTIONS = @INSTALLCHECK_CONFIG_OPTIONS@
installcheck-local:
	test -d installtest || $(MKDIR_P) installtest
	d=installtest; dd="$(abs_srcdir)/$$d"; \
	test @BUILD_IN_SRCDIR@ = yes && dd=.; \
	cd $$d && $$dd/configure --host=$(host) --build=$(build) \
	 `test -n "$(target)" && echo '--target=$(target)'` \
	 'CPPFLAGS=-I$(includedir)' 'LDFLAGS=-L$(libdir)' \
	 $(INSTALLCHECK_CONFIGURE_OPTIONS)
	cd installtest && $(MAKE) check \
	 EXPECTED_MAJOR_VERSION=@RAPP_MAJOR_VERSION@ \
	 EXPECTED_MINOR_VERSION=@RAPP_MINOR_VERSION@
	cd installtest && $(MAKE) distclean
