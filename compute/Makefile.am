#  Automake file for the RAPP Compute convenience library.

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

# The subdirectories to process
if !RAPP_USE_TUNECACHE
if !RAPP_TUNING
MAYBE_TUNE = tune
endif
endif
SUBDIRS = $(MAYBE_TUNE) include common backend generic vector tools

# The RAPP Compute convenience library
noinst_LTLIBRARIES = librappcompute.la

# List the RAPP Compute API headers so they are included in the dist
librappcompute_la_SOURCES = include/rappcompute.h    \
                            include/rc_export.h      \
                            include/rc_platform.h    \
                            include/rc_impl.h        \
                            include/rc_stdbool.h     \
                            include/rc_malloc.h      \
                            include/rc_pixel.h       \
                            include/rc_bitblt_wm.h   \
                            include/rc_bitblt_wa.h   \
                            include/rc_bitblt_vm.h   \
                            include/rc_bitblt_va.h   \
                            include/rc_pixop.h       \
                            include/rc_type.h        \
                            include/rc_thresh.h      \
                            include/rc_stat.h        \
                            include/rc_moment_bin.h  \
                            include/rc_reduce.h      \
                            include/rc_reduce_bin.h  \
                            include/rc_expand_bin.h  \
                            include/rc_rotate.h      \
                            include/rc_rotate_bin.h  \
                            include/rc_filter.h      \
                            include/rc_morph_bin.h   \
                            include/rc_fill.h        \
                            include/rc_pad.h         \
                            include/rc_pad_bin.h     \
                            include/rc_margin.h      \
                            include/rc_crop.h        \
                            include/rc_contour.h     \
                            include/rc_rasterize.h   \
                            include/rc_cond.h        \
                            include/rc_gather.h      \
                            include/rc_gather_bin.h  \
                            include/rc_scatter.h     \
                            include/rc_scatter_bin.h \
                            include/rc_integral.h    \
                            include/rc_integral_bin.h


# Add convenience sub-libraries
librappcompute_la_LIBADD = common/librappcompute_com.la   \
                           generic/librappcompute_gen.la  \
                           vector/librappcompute_swar.la  \
                           vector/librappcompute_simd.la

librappcompute_la_LDFLAGS = -no-undefined

# The directory for arch-specific configurations
RB_ARCHDIR = tune/arch

# The Doxygen configuration file
RB_DOXYFILE = Doxyfile

# The Doxygen tag file that we use for timestamping the doxygen output
RB_DOXYGEN_TAG = doxygen.tag

# The output directory for generated Doxygen output
RB_DOXYGEN_DIR = doxygen.out


# ----------------------------------------------------
#  Rule section
# ----------------------------------------------------

# Also distribute doxygen files outside the images directory.
dist-hook:
	$(MKDIR_P) $(distdir)/doxygen
	cp -p $(srcdir)/doxygen/listing.sh $(srcdir)/doxygen/style.css \
	    $(distdir)/doxygen

if INSTALLALL_INCLUDES_HTML
# By default, this isn't implied, but we document it in README
install-data-local: install-html
endif

if HAVE_DOXYGEN

# Generate documentation on 'all' and 'html' targets
all-local html-local: $(RB_DOXYGEN_TAG)

# Install documentation hook
install-html-local: install-doxygen-html-dev

# Uninstall documentation hook
uninstall-local: uninstall-doxygen-html-dev

# Remove all files generated by Doxygen on 'clean'
clean-local:
	rm -rf $(RB_DOXYGEN_TAG) $(RB_DOXYGEN_DIR)

# Generate all doxygen documentation
$(RB_DOXYGEN_TAG): $(RB_DOXYFILE)
	$(MKDIR_P) $(RB_DOXYGEN_DIR)
	$(SHELL) $(srcdir)/doxygen/listing.sh $(srcdir)/$(RB_ARCHDIR) > \
                                     $(RB_DOXYGEN_DIR)/tunelist.html
	@DOXYGEN@ $(RB_DOXYFILE)
	$(INSTALL_DATA) $(srcdir)/$(RB_ARCHDIR)/*.html $(RB_DOXYGEN_DIR)/html
	$(INSTALL_DATA) doxygen/images/*.png $(RB_DOXYGEN_DIR)/html

# Install all generated html pages
install-doxygen-html-dev: $(RB_DOXYGEN_TAG)
	$(MKDIR_P) $(DESTDIR)$(htmldir)-dev
	$(INSTALL_DATA) $(RB_DOXYGEN_DIR)/html/* $(DESTDIR)$(htmldir)-dev

# Uninstall all generated html pages
uninstall-doxygen-html-dev:
	rm -rf $(DESTDIR)$(htmldir)-dev

endif
