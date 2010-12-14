#  Automake file for the RAPP benchmark application.

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

# Include the RAPP header
AM_CPPFLAGS += -I../include -I$(top_srcdir)/include

# The RAPP library to benchmark
lib_LTLIBRARIES = libbenchmark.la

# No explicit sources for the benchmarklibrary
libbenchmark_la_SOURCES =

# Add the actual RAPP library
libbenchmark_la_LIBADD  = ../driver/librappdriver.la \
                          ../compute/librappcompute.la

# The RAPP benchmark application
bin_PROGRAMS = benchmark

# The profiler sources
benchmark_SOURCES = rapp_benchmark.c

# Link with the RAPP library
benchmark_LDADD = libbenchmark.la

# The install directory for benchmark binaries
RB_INSTALLDIR = build

# The directory for arch-specific benchmark output
RB_ARCHDIR = arch

# The benchmark data file
RB_DATAFILE = benchmarkdata.py

# The final benchmark plot file
RB_PLOTFILE = benchmarkplot.html

# The platform-specific benchmark plot file
RB_HOSTPLOT = benchmarkplot-$(PLATFORM).html

# The self-extracting benchmark script
RB_BENCHMARK = rappbenchmark.run

# The data-to-plot plotting script
RB_PLOTTER = $(PYTHON) $(srcdir)/plotdata.py

# Add the scripts to the dist
EXTRA_DIST = plotdata.py bootstrap.sh

# Also clean the self-extracting benchmark script
CLEANFILES = $(RB_BENCHMARK) *.stamp

# Also distclean the generated files
DISTCLEANFILES = $(RB_PLOTFILE) $(RB_DATAFILE)

# Install everything in RB_INSTALLDIR
bindir = `pwd`/$(RB_INSTALLDIR)
libdir = `pwd`/$(RB_INSTALLDIR)


# ----------------------------------------------------
#  Rule section
# ----------------------------------------------------

# Build the html plot file on 'all'
all:
	if test ! -f $(RB_PLOTFILE); then \
	    $(MAKE) $(RB_PLOTFILE).stamp;      \
	fi

# Add all cached benchmarks to the dist
dist-hook:
	$(MKDIR_P) $(distdir)/$(RB_ARCHDIR)
	-cp -p $(srcdir)/$(RB_ARCHDIR)/*.html $(distdir)/$(RB_ARCHDIR)

# Also remove the install dir on clean
clean-local:
	rm -rf $(RB_INSTALLDIR)

# Everything installed here is supposed to be staged in the
# build tree using ordinary install rules with special values
# for libdir and bindir, but this doesn't work when DESTDIR is set.
# We can't stop installation or unset DESTDIR, but we can
# uninstall what was installed when DESTDIR is non-empty and
# hope that parent directories created just for this "install"
# are ignored.
install-exec-hook:
	d=$(DESTDIR); test -z "$$d" || \
	  { $(MAKE) uninstall && rm -rf $$d$(bindir) $$d$(libdir); }

# Create the html plot file
$(RB_PLOTFILE).stamp: $(RB_DATAFILE)
	rm -f $(RB_PLOTFILE)
	$(RB_PLOTTER)
	touch $@

export-new-archfiles: $(RB_PLOTFILE).stamp
	$(MKDIR_P) $(RAPP_EXPORT_DIR)/$(RB_ARCHDIR)
	$(INSTALL_DATA) $(RB_PLOTFILE) \
	  $(RAPP_EXPORT_DIR)/$(RB_ARCHDIR)/$(RB_HOSTPLOT)

update-benchmarks:
	$(MAKE) "RAPP_EXPORT_DIR=$(srcdir)" export-new-archfiles

# Create the benchmark data file.  Depend on a stamp-file instead
# of directly on $(RB_BENCHMARK) so we don't overwrite
# $(RB_DATAFILE) at the next "make all" (after manually copying
# $(RB_DATAFILE) from a target platform) due to the install-exec
# dependency for $(RB_BENCHMARK).
$(RB_DATAFILE): $(RB_BENCHMARK).stamp
	rm -f $(RB_DATAFILE)
	@if test $(build) = $(host); then                                   \
	    ./$(RB_BENCHMARK);                                              \
	else                                                                \
	    echo;                                                           \
	    echo "======================================================="; \
	    echo "Please execute benchmark/$(RB_BENCHMARK) on the";         \
	    echo "target platform ($(host)), copy the output";              \
	    echo "file \"$(RB_DATAFILE)\" to benchmark/ and type";          \
	    echo "\"make\" to continue.";                                   \
	    echo "======================================================="; \
	    echo;                                                           \
	    exit 1;                                                         \
	fi

$(RB_BENCHMARK).stamp:
	$(MAKE) $(RB_BENCHMARK)
	touch $@

# Create the self-extracting benchmark script
$(RB_BENCHMARK): install-exec
	$(LN_S) $(srcdir)/../compute/tools/selfextract.sh .
	$(LN_S) $(srcdir)/../compute/tools/selfheader.sh .
	$(SHELL) ./selfextract.sh \
	 $(srcdir)/bootstrap.sh $(RB_INSTALLDIR) > $@.tmp
	chmod a+x $@.tmp
	rm -f selfextract.sh selfheader.sh
	mv $@.tmp $@
