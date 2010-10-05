#  Automake file for the RAPP Compute tuning app and modules.

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

# The RAPP Compute benchmark application
bin_PROGRAMS = benchmark

# The RAPP Compute implementation module
pkglib_LTLIBRARIES = rappcompute_tune.la

# The benchmark sources
benchmark_SOURCES = rc_benchmark.c

# No explicit sources for implementation modules
rappcompute_tune_la_SOURCES =

# Link with libdl for dlopen(), dlsym() etc
benchmark_LDFLAGS = $(DLLIB)

# Add the RAPP Compute library
rappcompute_tune_la_LIBADD  = ../../librappcompute.la

# Tell Libtool that we are building a module, and that we want
# the "raw" suffix, without version numbers, e.g. ".so"
rappcompute_tune_la_LDFLAGS = -module -no-undefined -avoid-version

clean-local:
	-rm -rf $(pkglibdir)

# Install everything here
bindir    = `pwd`/build
pkglibdir = `pwd`/build
