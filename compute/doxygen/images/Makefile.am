#  Automake file for RAPP internals documentation images.

#  Copyright (C) 2005-2010, 2014, Axis Communications AB, LUND, SWEDEN
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

# See $(top_srcdir)/doxygen/images/Makefile.am for comments; one
# day we may generate both from a common template.

IMAGES = design tuning

PNGS = $(IMAGES:%=%.png)
SVGS = $(IMAGES:%=%.svg)

dist_noinst_DATA = $(PNGS) $(SVGS)

noinst_DATA = $(MAYBE_LOCAL_PNG)
CLEANFILES = $(noinst_DATA)

INKSCAPE = @INKSCAPE@
INKSCAPE_OPTIONS = @INKSCAPE_OPTIONS@

if HAVE_DOXYGEN
MAYBE_LOCAL_PNG = png-local.stamp

png-local.stamp: @MAINTAINER_MODE_TRUE@ $(PNGS)
	ff="$(PNGS)"; \
	for f in $$ff; do \
	    test -f $$f || cp -p $(srcdir)/$$f . || exit; \
	done
	touch $@

.svg.png:
	$(INKSCAPE) $(INKSCAPE_OPTIONS) -f $< -e $@

all html install-html: png-local.stamp

endif

clean-local:
	test $(BUILD_IN_SRCDIR) = yes || rm -rf *.png
