#  Automake file for the RAPP library unit tests.

#  Copyright (C) 2005-2012, Axis Communications AB, LUND, SWEDEN
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
HAVE_CHECK_YESNO = yes
else
testapp_src = rapp_test.c
HAVE_CHECK_YESNO = no
endif

# Set the test program
TESTS = rapptest

if RAPP_LOGGING

# Inside the conditional, to catch stray references left outside
# (that aren't satisfied with the default empty contents).
RAPP_LOGFILE = rapp_log_results.txt

# If logging is on, we'll have some #warning directives; make
# sure they're not promoted to errors. N.B., we need this at
# *_CFLAGS and not *_CPPFLAGS, as the latter would be emitted
# before the -Werror and thus not override it.
AM_CFLAGS += $(HASHWARN_CFLAGS)

# Unfortunately, the simplest way to get the logfile anywhere
# else than the default in /tmp, we have to setenv in the C
# source files. While it'd be preferable to pass it from
# outside of the program, that's still reasonably remote from
# the library.
AM_CFLAGS += -DRAPP_LOGFILE='"'`pwd`'/$(RAPP_LOGFILE)"'

# N.B.: On a successful run, this also removes the logfile. It
# has to run *after* the main test (each main test), but
# thankfully automake currently (1.11) iterates on TESTS as a
# list thus parallel-safe at the moment.
TESTS += verify_logfile

# We're not inspecting this file, we just want to make sure it's
# out of the way and will be automatically removed.
RAPP_INSTALLCHECK_ENVIRONMENT = env RAPP_LOGFILE=`pwd`/../$(RAPP_LOGFILE).tmp
endif # RAPP_LOGGING

check_PROGRAMS = $(TESTS)

# The test sources
rapptest_SOURCES = \
 $(testapp_src) \
 rapp_tests.def \
 rapp_test_util.h \
 rapp_test_util.c \
 rapp_test_pixel.c \
 rapp_test_bitblt.c \
 rapp_test_pixop.c \
 rapp_test_type.c \
 rapp_test_thresh.c \
 rapp_test_reduce.c \
 rapp_test_reduce_bin.c \
 rapp_test_expand_bin.c \
 rapp_test_rotate.c \
 rapp_test_rotate_bin.c \
 rapp_test_stat.c \
 rapp_test_moment_bin.c \
 rapp_test_filter.c \
 rapp_test_morph_bin.c \
 rapp_test_fill.c \
 rapp_test_pad.c \
 rapp_test_pad_bin.c \
 rapp_test_margin.c \
 rapp_test_crop.c \
 rapp_test_contour.c \
 rapp_test_rasterize.c \
 rapp_test_cond.c \
 rapp_test_gather.c \
 rapp_test_gather_bin.c \
 rapp_test_scatter.c \
 rapp_test_scatter_bin.c \
 rapp_test_framework.c \
 rapp_test_integral.c

verify_logfile_SOURCES = verify_logfile.in

# Link with the RAPP library and the reference implementation
rapptest_LDADD = @CHECK_LIBS@ reference/librappref.la

# Additional files to clean
CLEANFILES = $(RAPP_LOGFILE) *.tmp

if RAPP_LOGGING
verify_logfile: verify_logfile.in Makefile
	sed -e 's,@LOGFILE@,$(RAPP_LOGFILE),g' \
            -e 's,@HAVE_CHECK_YESNO@,$(HAVE_CHECK_YESNO),g' \
            < $< > $@.tmp
	chmod +x $@.tmp
	mv $@.tmp $@
endif

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
	cd installtest && $(RAPP_INSTALLCHECK_ENVIRONMENT) \
	 $(MAKE) check \
	 EXPECTED_MAJOR_VERSION=@RAPP_MAJOR_VERSION@ \
	 EXPECTED_MINOR_VERSION=@RAPP_MINOR_VERSION@
	cd installtest && $(MAKE) distclean
