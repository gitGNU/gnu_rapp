#  Automake file for the RAPP installtest.

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

# Search the toplevel m4 directory
ACLOCAL_AMFLAGS = -I ../../m4

TESTS = closing
check_PROGRAMS = $(TESTS)
closing_CPPFLAGS = \
 -DEXPECTED_MAJOR_VERSION=$(EXPECTED_MAJOR_VERSION) \
 -DEXPECTED_MINOR_VERSION=$(EXPECTED_MINOR_VERSION)

# This arrangement is not critical, just make sure only
# dummy.cxx (and nothing else) is compiled as C++ and make sure
# whatever is built isn't installed.
EXTRA_LTLIBRARIES = libdummy.la
libdummy_la_SOURCES = dummy.cxx
CLEANFILES = $(EXTRA_LTLIBRARIES)
all-local: $(EXTRA_LTLIBRARIES)
