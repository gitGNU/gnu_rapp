#  Automake file for the RAPP Compute vector backend tests.

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

#  We test the SWAR and SIMD implementations of the vector API.

# Include the headers needed
AM_CPPFLAGS += -I$(srcdir)/../../include \
               -I$(srcdir)/../../common  \
               -I$(srcdir)/..

# Set the test app sources
TESTS = rctest_swar16 rctest_swar32 rctest_swar64 rctest_simd

# Set the test programs
check_PROGRAMS = $(TESTS)

# The test sources
rctest_swar16_SOURCES = rc_test.c         \
                        rc_test_vec_def.h \
                        rc_vec_ref.h      \
                        rc_vec_ref.c

rctest_swar32_SOURCES = $(rctest_swar16_SOURCES)
rctest_swar64_SOURCES = $(rctest_swar16_SOURCES)
rctest_simd_SOURCES   = $(rctest_swar16_SOURCES)

# Add the SWAR CPPFLAGS
rctest_swar16_CPPFLAGS = $(AM_CPPFLAGS) -DRAPP_FORCE_SIZE=2
rctest_swar32_CPPFLAGS = $(AM_CPPFLAGS) -DRAPP_FORCE_SIZE=4
rctest_swar64_CPPFLAGS = $(AM_CPPFLAGS) -DRAPP_FORCE_SIZE=8

# Add the SIMD CPPFLAGS
rctest_simd_CPPFLAGS = $(AM_CPPFLAGS) -DRAPP_USE_SIMD
