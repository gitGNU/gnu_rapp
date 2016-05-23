#  Automake file for the RAPP library.

#  Copyright (C) 2005-2012, 2014, Axis Communications AB, LUND, SWEDEN
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

# Tell aclocal to search the m4 directory
ACLOCAL_AMFLAGS = -I m4

# The subdirectories to process. We separate "benchmark" from
# the others so we only have to cater to our own use and not to
# supporting standard rules like "make install DESTDIR=/some/prefix". 
# Actual dependencies replace its order within SUBDIRS.
SUBDIRS = doxygen/images compute/doxygen/images compute driver . test
DIST_SUBDIRS = $(SUBDIRS) benchmark

EXTRA_DIST = COPYING COPYING.FDL COPYING.LESSER \
             README AUTHORS NEWS TODO HACKING   \
             autogen.sh .gitignore

# Install all headers in a rapp subdirectory
rapp_includedir = $(includedir)/rapp

# Install also the pkg-config data file
pkgconfigdir   = ${libdir}/pkgconfig
pkgconfig_DATA = rapp.pc

# Distribute the specfile with version numbers subst'ed.
dist_noinst_DATA = rapp.spec

# The RAPP API headers to install
rapp_include_HEADERS = \
 include/rapp.h \
 include/rapp_export.h \
 include/rapp_main.h \
 include/rapp_info.h \
 include/rapp_error.h \
 include/rapp_malloc.h \
 include/rapp_pixel.h \
 include/rapp_bitblt.h \
 include/rapp_pixop.h \
 include/rapp_type.h \
 include/rapp_thresh.h \
 include/rapp_stat.h \
 include/rapp_moment_bin.h \
 include/rapp_reduce.h \
 include/rapp_reduce_bin.h \
 include/rapp_expand_bin.h \
 include/rapp_rotate.h \
 include/rapp_rotate_bin.h \
 include/rapp_filter.h \
 include/rapp_fill.h \
 include/rapp_pad.h \
 include/rapp_pad_bin.h \
 include/rapp_morph_bin.h \
 include/rapp_margin.h \
 include/rapp_crop.h \
 include/rapp_contour.h \
 include/rapp_rasterize.h \
 include/rapp_cond.h \
 include/rapp_gather.h \
 include/rapp_gather_bin.h \
 include/rapp_scatter.h \
 include/rapp_scatter_bin.h \
 include/rapp_integral.h

# This one is generated at configure-time, not distributed
nodist_rapp_include_HEADERS = include/rapp_version.h

# The librapp library to build
lib_LTLIBRARIES = librapp.la

# No top-level sources
librapp_la_SOURCES =

# Add convenience sub-libraries
librapp_la_LIBADD = driver/librappdriver.la \
                    compute/librappcompute.la

# Set the library version
librapp_la_LDFLAGS = -no-undefined -version-info \
                     ${RAPP_LIB_CUR}:${RAPP_LIB_REV}:${RAPP_LIB_AGE}

# The Doxygen configuration file
RB_DOXYFILE = Doxyfile

# The Doxygen tag file that we use for timestamping the doxygen output
RB_DOXYGEN_TAG = doxygen.tag

# The output directory for generated Doxygen output
RB_DOXYGEN_DIR = doxygen.out

# The directory for arch-specific benchmark plots
RB_ARCHDIR = benchmark/arch


# ----------------------------------------------------
#  Rule section
# ----------------------------------------------------

# This is a continuation of "distcheck" (see Makefile.in of
# automake-1.11.1) but with builddir = srcdir and without
# calling distcheck-hook and with enabling writes to directories
# in srcdir and without running distcleancheck, as it's not
# supported for builddir = srcdir at least not in
# automake-1.11.1. This target is only supported when run as
# part of distcheck-hook.
distcheck-srcdirbuild:
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	rm -rf $(distdir)/_build
	find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';'
	dc_install_base=`$(am__cd) $(distdir)/_inst && pwd | sed -e 's,^[^:\\/]:[\\/],/,'` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && am__cwd=`pwd` \
	  && $(am__cd) $(distdir) \
	  && ./configure --prefix="$$dc_install_base" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && $(MAKE) $(AM_MAKEFLAGS) distuninstallcheck_dir="$$dc_install_base" \
	        distuninstallcheck \
	  && chmod -R a-w "$$dc_install_base" \
	  && ({ \
	       (cd .. && umask 077 && mkdir "$$dc_destdir") \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" install \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" uninstall \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" \
	            distuninstallcheck_dir="$$dc_destdir" distuninstallcheck; \
	      } || { rm -rf "$$dc_destdir"; exit 1; }) \
	  && rm -rf "$$dc_destdir" \
	  && $(MAKE) $(AM_MAKEFLAGS) dist \
	  && rm -rf $(DIST_ARCHIVES) \
	  && cd "$$am__cwd" \
	  || exit 1
	$(am__remove_distdir)
	@(echo "$(distdir) archives ready for distribution: "; \
	  list='$(DIST_ARCHIVES)'; for i in $$list; do echo $$i; done) | \
	  sed -e 1h -e 1s/./=/g -e 1p -e 1x -e '$$p' -e '$$x'
	case '$(DIST_ARCHIVES)' in \
	*.tar.gz*) \
	  GZIP=$(GZIP_ENV) gzip -dc $(distdir).tar.gz | $(am__untar) ;;\
	*.tar.bz2*) \
	  bzip2 -dc $(distdir).tar.bz2 | $(am__untar) ;;\
	*.tar.lzma*) \
	  lzma -dc $(distdir).tar.lzma | $(am__untar) ;;\
	*.tar.xz*) \
	  xz -dc $(distdir).tar.xz | $(am__untar) ;;\
	*.tar.Z*) \
	  uncompress -c $(distdir).tar.Z | $(am__untar) ;;\
	*.shar.gz*) \
	  GZIP=$(GZIP_ENV) gzip -dc $(distdir).shar.gz | unshar ;;\
	*.zip*) \
	  unzip $(distdir).zip ;;\
	esac
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/_build
	mkdir $(distdir)/_inst
	chmod a-w $(distdir)
	test -d $(distdir)/_build || exit 0

# Make it trivially overridable, in case it breaks, perhaps with newer automake
DISTCHECK_HOOK_DEP = distcheck-srcdirbuild

# This is expected to break for at least one tune-file, but at
# least we can keep track automatically.
distcheck-tune-cache:
	sed -ne '$(RC_BMARK_SED)' \
	    < ${srcdir}/compute/tune/benchmark/rc_benchmark.c \
	    > srcfuncs.tmp
	bad=; \
	for f in $(srcdir)/compute/tune/arch/rapptune-*.h; do \
	    sed -ne '$(RC_TUNE_SED)' < $$f > cachefuncs.tmp; \
	    if test $$f = $(srcdir)/compute/tune/arch/rapptune-generic.h \
		|| diff -u srcfuncs.tmp cachefuncs.tmp > result.tmp 2>&1; \
	    then :; \
	    else \
		echo "*** outdated tune-file: `basename $$f`"; \
		cat result.tmp; \
		bad="$${bad}$${bad+ }$$f"; \
	    fi; \
	done; \
	rm -f result.tmp cachefuncs.tmp sourcefuncs.tmp; \
	test -z "$${bad}"

DISTCHECK_TUNE_CACHE = distcheck-tune-cache

# Check that all .svg's have .png and that the .png is newer than the .svg
# and that there are no .png's without .svg's. Beware: bashism "-ot" used.
distcheck-hook: $(DISTCHECK_HOOK_DEP) $(DISTCHECK_TUNE_CACHE)
	for s in `find $(distdir) -name \*.svg`; do \
	    p=`echo $$s | sed -e 's,.svg$$,.png,'`; \
	    echo "Checking $$s / $$p ..."; \
	    test -f $$p || exit 1; \
	    test ! $$p -ot $$s || exit 1; \
	done
	for p in `find $(distdir) -name \*.png`; do \
	    s=`echo $$s | sed -e 's,.png$$,.svg,'`; \
	    echo "Checking $$p ..."; \
	    test -f $$s || exit 1; \
	done

# Things we always want on when checking
DISTCHECK_CONFIGURE_FLAGS = --enable-werror

# Option sets of which we want to test combinations. The order
# is supposed to cause the most error-prone combinations to be
# tested first. Coverage of supported configure combinations
# should be near complete.
RELEASE_DISTCHECKS = \
 --disable-debug/--enable-debug \
 --disable-static/--enable-static \
 --disable-tune-cache/--enable-tune-cache \
 --enable-backend=auto/--enable-backend=none/--enable-backend=nonvector \
 --with-doxygen/--without-doxygen \
 --disable-silent-rules/--enable-silent-rules

# Test all combinations of /-delimeted elements in the
# whitespace-delimeted sets of $(RELEASE_DISTCHECKS)
# as DISTCHECK_CONFIGURE_FLAGS. Suitable for release-checking.
release-distchecks:
	set x $(RELEASE_DISTCHECKS); shift; car=$$1; shift; \
	cdr=$${1+"$$@"}; opts=`echo $$car | sed -e 's,/, ,g'`; \
	for opt in $$opts; do \
	   dcf='DISTCHECK_CONFIGURE_FLAGS=$(DISTCHECK_CONFIGURE_FLAGS)'; \
	   dc="RELEASE_DISTCHECKS=$$cdr"; \
	   $(MAKE) release-distchecks "$$dcf $$opt" "$$dc" \
	      || exit; \
	done; \
	dcf='DISTCHECK_CONFIGURE_FLAGS=$(DISTCHECK_CONFIGURE_FLAGS)'; \
	if test -z "$$car"; then \
	    $(MAKE) distcheck "$$dcf" DISTCHECK_TUNE_CACHE= \
	     || { echo "*** distcheck failed for '$$dcf'"; exit $$x; }; \
	fi

# Test each whitespace-delimeted sets of /-delimeted options in
# $(MANY_DISTCHECKS) as DISTCHECK_CONFIGURE_FLAGS (in addition
# to, or perhaps overriding, its original value). These are just
# the most interesting combinations; see RELEASE_DISTCHECKS for
# a fuller set. Suitable for everyday configure.ac/Makefile.am
# hacking.
MANY_DISTCHECKS = \
 --enable-debug/--disable-tune-cache \
 --enable-debug/--enable-logging \
 --disable-debug/--enable-logging \
 --disable-debug/--disable-tune-cache/--enable-backend=auto \
 --disable-debug/--disable-tune-cache/--enable-backend=nonvector \
 --disable-debug/--enable-tune-cache/--enable-backend=auto

many-distchecks:
	dc="$(MANY_DISTCHECKS)"; \
	for combo in $$dc; do \
	   opts=`echo $$combo | sed -e 's,/, ,g'`; \
	   dcf='DISTCHECK_CONFIGURE_FLAGS=$(DISTCHECK_CONFIGURE_FLAGS)'; \
	   $(MAKE) distcheck "$$dcf $$opts" DISTCHECK_TUNE_CACHE= \
	     || { echo "*** distcheck failed for '$$dcf $$opts'"; exit $$x; }; \
	done

if RAPP_USE_TUNECACHE
export-new-archfiles update-tune-cache update-benchmarks:
	echo "Did you forget to configure with --disable-tune-cache?"
	exit 1
else
# Just a value suitable for a single target sets of files
RAPP_EXPORT_DIR = exports
RAPP_EXPORT_FILE = $(abs_builddir)/rapp-$(VERSION)-archfiles-$(PLATFORM).tar.gz
RAPP_EXPORT_SUBDIRS = compute/tune benchmark

export-new-archfiles: $(RAPP_EXPORT_FILE)

$(RAPP_EXPORT_FILE): all
	absp=; \
	d="$(RAPP_EXPORT_DIR)"; \
	case $$d in \
	    [\\/]* | ?:[\\/]*) ;; \
	    *) absp=$(abs_builddir)/;; \
	esac; \
	dd="$(RAPP_EXPORT_SUBDIRS)"; \
	for d in $$dd; do \
	    $(MKDIR_P) $(RAPP_EXPORT_DIR)/$$d || exit 1; \
	    (cd $$d \
	      && $(MAKE) "RAPP_EXPORT_DIR=$${absp}$(RAPP_EXPORT_DIR)/$$d" \
		   export-new-archfiles) \
	      || exit 1; \
	done
	rm -f $@ $@.tmp*
	cd $(RAPP_EXPORT_DIR) && tar -c -f $@.tmp $(RAPP_EXPORT_SUBDIRS)
	gzip $@.tmp
	mv $@.tmp.gz $@

update-tune-cache: all
	cd compute/tune && $(MAKE) $@

update-benchmarks: all
	cd benchmark && $(MAKE) $@
endif

update-archfiles: update-benchmarks update-tune-cache

# Clean the default value of RAPP_EXPORT_DIR and the export-file.
clean-local: clean-local-doxy
	rm -rf exports $(RAPP_EXPORT_FILE)

if INSTALLALL_INCLUDES_HTML
# By default, this isn't implied, but we document it in README
install-data-local: install-html
endif

if HAVE_DOXYGEN

# Generate documentation on 'all' and 'html' targets
all-local html-local: $(RB_DOXYGEN_TAG)

# Install documentation hook
install-html-local: install-doxygen-html

# Uninstall documentation hook
uninstall-local: uninstall-doxygen-html

# Remove all files generated by Doxygen on 'clean'
clean-local-doxy:
	rm -rf $(RB_DOXYGEN_TAG) $(RB_DOXYGEN_DIR)

# Generate all doxygen documentation
$(RB_DOXYGEN_TAG): $(RB_DOXYFILE) $(rapp_include_HEADERS)
	$(MKDIR_P) $(RB_DOXYGEN_DIR)
	$(SHELL) $(srcdir)/compute/doxygen/listing.sh \
          $(srcdir)/$(RB_ARCHDIR) > $(RB_DOXYGEN_DIR)/benchmarklist.html
	@DOXYGEN@ $(RB_DOXYFILE)
	$(INSTALL_DATA) $(srcdir)/$(RB_ARCHDIR)/*.html $(RB_DOXYGEN_DIR)/html
	$(INSTALL_DATA) doxygen/images/*.png $(RB_DOXYGEN_DIR)/html

# Install all generated html pages
install-doxygen-html: $(RB_DOXYGEN_TAG)
	$(MKDIR_P) $(DESTDIR)$(htmldir)
	$(INSTALL_DATA) $(RB_DOXYGEN_DIR)/html/* $(DESTDIR)$(htmldir)

# Uninstall all generated html pages
uninstall-doxygen-html:
	rm -rf $(DESTDIR)$(htmldir)

else
clean-local-doxy:
endif

all-local: $(librapp_la_LIBADD)
	cd benchmark && $(MAKE) $@
