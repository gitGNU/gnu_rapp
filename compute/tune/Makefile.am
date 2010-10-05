#  Automake file for the RAPP performance tuner.

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

# This directory is only visited (and this Makefile used) when
# we tune (or make dist), so we don't have to test for that.

# The data analyzer python script
RB_ANALYZE = $(PYTHON) $(srcdir)/analyze.py

# The generated self-extracting performance measure shell script
RB_MEASURE = rappmeasure.run

# The tuning data file
RB_TUNEDATA = tunedata.py

# The configuration header output from the analyzer script
RB_TUNECONF = rapptune.h

# The HTML report file from the tuning process
RB_TUNEREPORT = tunereport.html

# The host-specific configuration header
RB_HOSTCONF = rapptune-$(PLATFORM).h

# The host-specific configuration report
RB_HOSTREPORT = tunereport-$(PLATFORM).html

# The directory where to store the benchmark app and candidate libs
RB_ARCHIVEDIR = archive

# The directory where to store tuned configurations
RB_ARCHDIR = arch

# Clean everything except platform-specific files
CLEANFILES = $(RB_MEASURE)    \
             $(RB_TUNEDATA)   \
             $(RB_TUNEREPORT) \
             *.x *.?stamp *.stamp *.tmp *.log benchmark/*.log

# Include the benchmark directory in the dist
DIST_SUBDIRS = benchmark

# Remove the configuration header on distclean
DISTCLEANFILES = $(RB_TUNECONF)

# Also distribute tuning scripts
EXTRA_DIST = progress.sh measure.sh bootstrap.sh analyze.py

# When building in srcdir, we need to have an ordering
# dependency, so we can safely restore the toplevel
# config.status in one target.
RB_ISTAMPS_yes = $(RB_XCANDIDATES:.x=.istamp)

# Try to be as quiet as we can
RB_V_silent = $(RB__v_silent_$(V))
RB__v_silent_ = $(RB__v_silent_$(AM_DEFAULT_VERBOSITY))
RB__v_silent_0 = --silent
RB_V_e = $(RB__v_e_$(V))
RB__v_e_ = $(RB__v_e_$(AM_DEFAULT_VERBOSITY))
RB__v_e_0 = :
RB_V_ne = $(RB__v_ne_$(V))
RB__v_ne_ = $(RB__v_ne_$(AM_DEFAULT_VERBOSITY))
RB__v_ne_1 = :
RB_V_at = $(AM_V_at)
RB_V_logex = $(RB__v_logex_$(V))
RB__v_logex_ = $(RB__v_logex_$(AM_DEFAULT_VERBOSITY))
RB__v_logex_0 = > $@.log 2>&1 || (x=$$?; echo '$@ failed:'; cat $@.log; exit $$x)

# ----------------------------------------------------
#  Rule section
# ----------------------------------------------------

check all: $(RB_TUNECONF).stamp

# Add all cached configurations to the dist
dist-hook:
	$(MKDIR_P) $(distdir)/$(RB_ARCHDIR)
	-cp -p $(srcdir)/$(RB_ARCHDIR)/*.{html,h} \
	       $(distdir)/$(RB_ARCHDIR)

# Clean all generated tuning files if tuned
clean-local:
	-rm -rf $(RB_ARCHIVEDIR) $(RB_XCANDIDATES:.x=.dir)

$(RB_MEASURE): $(RB_XCANDIDATES:.x=.istamp) benchmark.stamp
	@$(RB_V_ne) echo
	$(RB_V_at)$(INSTALL_SCRIPT) $(srcdir)/measure.sh  $(RB_ARCHIVEDIR)
	$(RB_V_at)$(INSTALL_SCRIPT) $(srcdir)/progress.sh $(RB_ARCHIVEDIR)
	$(RB_V_at)$(LN_S) $(srcdir)/../tools/selfextract.sh .
	$(RB_V_at)$(LN_S) $(srcdir)/../tools/selfheader.sh  .
	$(RB_V_at)$(SHELL) ./selfextract.sh \
	 $(srcdir)/bootstrap.sh $(RB_ARCHIVEDIR) > $@.tmp
	$(RB_V_at)chmod a+x $@.tmp
	$(RB_V_at)rm -f selfextract.sh selfheader.sh
	$(RB_V_at)mv $@.tmp $@

# By calling it *.bstamp, we can ls *.bstamp without error and
# having the "+1" automatic; see .cstamp.bstamp.
# Support building in srcdir by moving its config.status here
# (every .cstamp ultimately depends on this target),
# temporarily, and restoring it in benchmark.stamp.
progress0.bstamp:
	@$(RB_V_ne) echo 'Building implementation candidates...'
	@$(RB_V_ne) $(SHELL) $(srcdir)/progress.sh 0
	$(RB_V_at)if test $(BUILD_IN_SRCDIR) = yes; then \
	 mv $(top_srcdir)/config.status ./config.status.tmp; \
	fi
	$(RB_V_at)touch $@

$(RB_XCANDIDATES): progress0.bstamp
	$(RB_V_at)touch $@

# Dependencies are added last, by configure
.x.cstamp:
	@$(RB_V_e) echo '*** Configuring implementation candidate $*...'
	$(RB_V_at)d=$*.dir; \
	unroll=`echo $* | sed 's,.*_,,'`; \
	impl=`echo $* | sed 's,_.*,,'`; \
	rm -rf $$d && $(MKDIR_P) $$d && cd $$d \
	    && $(abs_srcdir)/../../configure $(RB_V_silent) \
		$(CONFIG_ARGS_FOR_TUNING) \
		--with-internal-tune-generation=$$impl,$$unroll
	$(RB_V_at)touch $@

.cstamp.bstamp:
	@$(RB_V_e) echo '*** Building implementation candidate $*...'
	$(RB_V_at)cd $*.dir/compute && $(MAKE) $(RB_V_logex)
	@tot=$(RB_NCANDIDATES); cnt=`ls *.bstamp | wc -w`; \
	 $(RB_V_ne) $(SHELL) $(srcdir)/progress.sh \
	  `expr '(' 100 '*' $$cnt ')' / $$tot`
	$(RB_V_at)touch $@

archive.stamp:
	$(RB_V_at)rm -rf archive
	$(RB_V_at)$(MKDIR_P) archive
	$(RB_V_at)touch $@

# Looks like Solaris 10 make doesn't pass on command-line
# arguments to sub-makes, therefore we need to pass the override
# in AM_MAKEFLAGS too.
benchmark.stamp: $(RB_ISTAMPS_$(BUILD_IN_SRCDIR))
	$(RB_V_at)if test $(BUILD_IN_SRCDIR) = yes; then \
	 mv ./config.status.tmp $(top_srcdir)/config.status; \
	fi
	$(RB_V_at)o=pkglib_LTLIBRARIES=; cd benchmark \
	 && $(MAKE) $$o AM_MAKEFLAGS=$$o install $(RB_V_logex)
	$(RB_V_at)$(INSTALL_PROGRAM) benchmark/build/benchmark$(EXEEXT) \
	 $(RB_ARCHIVEDIR); \
	touch $@

# This depends on archive.stamp; configure adds that dependency
.bstamp.istamp:
	@$(RB_V_e) echo 'Installing implementation candidate $*...'
	$(RB_V_at)o=bin_PROGRAMS=; cd $*.dir/compute/tune/benchmark \
	 && $(MAKE) $$o AM_MAKEFLAGS=$$o install $(RB_V_logex)
	$(RB_V_at)d=$*.dir/compute/tune/benchmark/build; \
	 for ext in so dylib dll; do \
	     f=$$d/rappcompute_tune.$$ext; \
	     if test -f $$f; then \
	         $(INSTALL_PROGRAM) $$f \
		 $(RB_ARCHIVEDIR)/rappcompute_tune_$*.$$ext || exit 1; \
	        break; \
	     fi; \
	 done
	$(RB_V_at)touch $@

# Run the measure script to create the measurement data file
$(RB_TUNEDATA): $(RB_MEASURE)
	@if test $(build) = $(host); then                                   \
	    ./$(RB_MEASURE);                                                \
	else                                                                \
	    echo;                                                           \
	    echo "======================================================="; \
	    echo "Please execute compute/tune/$(RB_MEASURE) ";              \
	    echo "on the target platform ($(host)), ";                      \
	    echo "copy the output file \"$(RB_TUNEDATA)\" to ";             \
	    echo "compute/tune and type \"make\" to continue.";             \
	    echo "======================================================="; \
	    echo;                                                           \
	    exit 1;                                                         \
	fi

# Run the data analyzer script to generate the configuration header
$(RB_TUNECONF).stamp: $(RB_TUNEDATA)
	$(RB_V_at)$(RB_ANALYZE) -f $(RB_TUNEDATA)
	$(RB_V_at)touch $@

# Export the new tuning files anywhere
export-new-archfiles: $(RB_TUNECONF).stamp
	$(MKDIR_P) $(RAPP_EXPORT_DIR)/$(RB_ARCHDIR)
	$(INSTALL_DATA) $(RB_TUNECONF) \
	  $(RAPP_EXPORT_DIR)/$(RB_ARCHDIR)/$(RB_HOSTCONF)
	$(INSTALL_DATA) $(RB_TUNEREPORT) \
	  $(RAPP_EXPORT_DIR)/$(RB_ARCHDIR)/$(RB_HOSTREPORT)

# ...and as a special case, just install them in the source directory
update-tune-cache:
	$(MAKE) "RAPP_EXPORT_DIR=$(srcdir)" export-new-archfiles

@RAPP_TUNING_DEPENDENCIES@
