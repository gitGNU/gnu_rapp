#  Automake file for the RAPP Compute tuned configuration header.

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

# The tuned configuration header file
RB_TUNECONFIG = rapptune.h

# The tuned configuration header is generated (sym-linked)
BUILT_SOURCES = $(RB_TUNECONFIG)


# ----------------------------------------------------
#  Rule section
# ----------------------------------------------------

# Create the tuned configuration header as a symlink
# which we first need to remove silently, as a symlink isn't
# up-to-date when the base-file isn't there, causing "ln -s" to
# emit an error if the symlink is already there.  (It may
# already be there if make is called more than once.)
$(RB_TUNECONFIG):
	rm -rf $@
	$(LN_S) ../tune/$(RB_TUNECONFIG) $@
