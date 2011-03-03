#  Autoconf file for the RAPP library.

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

# AC_RULES: Just follow existing practice in this file, also see
# HACKING.  Some inconsistencies of which to beware:
#
# - Always quote all macro arguments; doublequote literal string
# arguments (e.g. AC_MSG_NOTICE, AC_MSG_ERROR, first argument to
# AS_HELP_STRING).
#
# - Mark empty arguments with explicit [] (optionally: other
# than the first, in which case a space after the comma is also
# optional).
#
# - Indent to after open parentheses (and quote) for continued
# lines *or* two spaces for macro arguments.

# Versioning
m4_define([rapp_major_version], [0])
m4_define([rapp_minor_version], [7])
# Empty for a release
m4_define([rapp_release_phase], [dev])

# Initialize
AC_PREREQ([2.63]) # May work with 2.62, but developers haven't tried it.
AC_INIT([RAPP], [rapp_major_version.rapp_minor_version[]rapp_release_phase])
AC_CONFIG_SRCDIR([include/rapp.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET
# See conditional AM_SILENT_RULES call below
AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign check-news filename-length-max=99])

HOMESITE=`sed -ne 's,^<\(http@<:@^>@:>@*\)>.*,\1,p' < ${srcdir}/README`
homewc=`wc -w << EOF
${HOMESITE}
EOF
`
if test $homewc -ne 1; then
    AC_MSG_ERROR([[Invalid site markup (for HOMESITE) in ${srcdir}/README: ]]
                 [[found ${homewc} words, expected 1]])
fi
AC_SUBST([HOMESITE])

# -----------------------------------------------------------

# VERSION NUMBERS
# ===============
# Sets the compile-time and run-time major and minor version numbers.
# Update values accordingly when making a new release.

# Major version number
RAPP_MAJOR_VERSION=rapp_major_version

# Minor version number
RAPP_MINOR_VERSION=rapp_minor_version

# Development phase suffix to distinguish releases from pre-release code
RAPP_RELEASE_PHASE=rapp_release_phase

# LIBTOOL VERSIONING
# ==================
# Controls the libtool version numbers CURRENT.REVISION.AGE.
# Do not try to synchronize these version numbers with the
# major and minor version numbers above - they serve two different
# purposes. For more information, consult the libtool manual
# http://www.gnu.org/software/libtool/manual.

# Current interface version
RAPP_LIB_CUR=1

# Interface revision
RAPP_LIB_REV=0

# Interface age
RAPP_LIB_AGE=1

# -----------------------------------------------------------

# Check version number consistency. We quote only because we
# verify the contents.
if test "${RAPP_MAJOR_VERSION}.${RAPP_MINOR_VERSION}${RAPP_RELEASE_PHASE}" \
    != "${VERSION}"; then
    AC_MSG_ERROR([[Conflicting version numbers set: ]]
                 [[${RAPP_MAJOR_VERSION}.${RAPP_MINOR_VERSION}]]
                 [[${RAPP_RELEASE_PHASE} != ${VERSION}]])
fi

# Save the user CFLAGS before AC_PROG_C89 modifies it
USER_CFLAGS=${CFLAGS}

# Check for programs

# Mostly this just changes the default, avoiding by-default LGPL
# violations, but also we avoid generating static libraries for
# each implementation candidate when tuning; unused as we'll
# dlopen them
LT_INIT([dlopen disable-static])

AC_PROG_CC_C89
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_EGREP
AC_ARG_VAR([PYTHON], [interpreter for the Python language])
# Prefer the newest version that is known working with this package
AC_CHECK_PROGS([PYTHON],
  [python2.7 python2.6 python2.5 python2.4 python2.8 python2.9 python])

# Restore user CFLAGS
CFLAGS=${USER_CFLAGS}

# Check C language features
AC_C_BIGENDIAN
AC_C_RESTRICT

# Check for headers
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_CHECK_HEADER([stdint.h], [],
  [AC_MSG_ERROR([[Required header stdint.h missing]])])

# Check C compiler vendor
AX_COMPILER_VENDOR

# Check for the CHECK unit test framework
AC_ARG_VAR([CHECK], [the CHECK unit test framework])
# Need to do this rather than the automatic AC_PATH_TOOL
# invocation from the PKG_CHECK_MODULES usage, in order to avoid
# nagging that a cross-version wasn't found (over and over again
# at tuning; PKG_CONFIG being declared precious apparently has
# no effect).
AC_PATH_PROGS([PKG_CONFIG], [${host}-pkg-config pkg-config])
PKG_CHECK_MODULES([CHECK],
                  [check >= 0.9.4],
                  [have_check=yes],
                  [have_check=no])
AM_CONDITIONAL([HAVE_CHECK], [test ${have_check} = yes])

# Check for Doxygen
AC_ARG_VAR([DOXYGEN], [documentation generator])
AC_CHECK_PROGS([DOXYGEN], [doxygen])

# We use this program to create .png from .svg (and to edit .svg).
AC_CHECK_PROGS([INKSCAPE], [inkscape], [${srcdir}/config/missing inkscape])
INKSCAPE_OPTIONS="--export-use-hints --export-area-drawing --without-gui"
AC_SUBST([INKSCAPE_OPTIONS])

# Enable various developer-centric option choices unless this is
# a release: if we find the word 'prerelease' in the first two
# lines of NEWS, then this isn't a release. If it's not there,
# and the word 'release' isn't either, then it isn't a release.
rapp_finalized_release=yes
sed -e 2q < "${srcdir}/NEWS" \
 | ${EGREP} '(Prerelease|prerelease|PRERELEASE)' > /dev/null 2>&1 \
 && rapp_finalized_release=no
sed -e 2q < "${srcdir}/NEWS" \
 | ${EGREP} '(Release|release|RELEASE)' > /dev/null 2>&1 \
 || rapp_finalized_release=no

if { test -n "${RAPP_RELEASE_PHASE}" \
      && test ${rapp_finalized_release} = yes; } \
    || { test -z "${RAPP_RELEASE_PHASE}" \
          && test ${rapp_finalized_release} = no; }; then
    AC_MSG_ERROR(
        [[${srcdir}/NEWS does not match dev-phase "${RAPP_RELEASE_PHASE}"]])
fi

# Silent build support. Requires automake-1.11. If you don't
# like the default choice, explicitly enable by passing
# --enable-silent-rules. Disable by either passing
# --disable-silent-rules or passing V=1 to make.
m4_ifdef([AM_SILENT_RULES],
  [if test ${rapp_finalized_release} = yes; then
       AM_SILENT_RULES([yes])
   else
       AM_SILENT_RULES([no])
   fi])

# For use with tr.  Neither "tr a-z A-Z" nor "tr '[a-z]'
# '[A-Z]'" work with Solaris 2.10 unless you set e.g. LANG=C.
# Better do what config.guess does and use explicit lists.
AZ=ABCDEFGHIJKLMNOPQRSTUVWXYZ
az=abcdefghijklmnopqrstuvwxyz

# Internal option, hence not documented
rapp_tune_generation=no
AC_ARG_WITH([internal-tune-generation], [],
  [case ${withval} in
   "generic,"@<:@124@:>@ | "swar,"@<:@124@:>@ | "simd,"@<:@124@:>@)
       rapp_tune_generation=yes
       tune_unroll_factor=`echo ${withval} | sed -e 's/.*,//'`
       tune_implementation=`echo ${withval} | sed -e 's/,.*//' | tr $az $AZ`
       AM_CFLAGS="${AM_CFLAGS} -DRAPP_FORCE_${tune_implementation}"
       AM_CFLAGS="${AM_CFLAGS} -DRAPP_FORCE_UNROLL=${tune_unroll_factor}"
       AM_CFLAGS="${AM_CFLAGS} -DRAPP_FORCE_EXPORT"
       ;;
   *)
       AC_MSG_ERROR(
           [[Bad value "${withval}" for --with-internal-tune-generation]])
       ;;
   esac])

# Determine the vector backend from the predefined macros
AC_CHECK_DEFINED([__MMX__], [OPT_BACKEND=mmx])
AC_CHECK_DEFINED([__SSE__], [OPT_BACKEND=sse])
AC_CHECK_DEFINED([__SSE2__], [OPT_BACKEND=sse2])
AC_CHECK_DEFINED([__SSSE3__], [OPT_BACKEND=ssse3])
AC_CHECK_DEFINED([__VEC__], [OPT_BACKEND=altivec])
AC_CHECK_DEFINED([__ALTIVEC__], [OPT_BACKEND=altivec])
AC_CHECK_DEFINED([__VADMX__], [OPT_BACKEND=vadmx])

# Set the PLATFORM string to be the AC 'host_cpu' plus the compiler vendor
PLATFORM=${host_cpu}-${ax_cv_c_compiler_vendor}

OPT_DOXYGEN=yes
OPT_INSTALLALL_INCLUDES_HTML=no
# Check for doxygen option
AC_ARG_WITH([doxygen],
  [AS_HELP_STRING([[--with-doxygen]], [Generate Doxygen documentation])],
  [OPT_DOXYGEN=${withval}
   case ${withval} in
   yes)
       if test -z "${DOXYGEN}"; then
           AC_MSG_ERROR([[Doxygen explicitly requested but not found]])
       fi
       OPT_INSTALLALL_INCLUDES_HTML=yes
       ;;
   no) ;;
   *)
       AC_MSG_ERROR([[Bad value "${withval}" for --with-doxygen option]])
       ;;
   esac])
AM_CONDITIONAL([HAVE_DOXYGEN],
  [test ${OPT_DOXYGEN} = yes \
    && test -n "${DOXYGEN}" \
    && test ${rapp_tune_generation} = no])
AM_COND_IF([HAVE_DOXYGEN], [], [OPT_INSTALLALL_INCLUDES_HTML=no])
AM_CONDITIONAL([INSTALLALL_INCLUDES_HTML],
               [test ${OPT_INSTALLALL_INCLUDES_HTML} = yes])

# Check for debug option
OPT_DEBUG=default
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([[--enable-debug@<:@=no@:>@]], [Enable debugging])],
  [OPT_DEBUG=${enableval}
   case ${enableval} in
   yes|no) ;;
   *)
       AC_MSG_ERROR([[Bad value "${enableval}" for --enable-debug option]])
       ;;
   esac])

# Check for vector backend option.
# Unfortunately, the second field in the help-strings will
# not be vertically aligned as they are below.
backend_info_name=
AC_ARG_ENABLE([backend],
  [AS_HELP_STRING([[--enable-backend@<:@=auto@:>@]],
                  [Use vector accelerator backend:])
   AS_HELP_STRING(,[auto:      Select backend automatically])
   AS_HELP_STRING(,[nonvector: Disable all vector operations])
   AS_HELP_STRING(,[none:      Disable all vector operations and tuned operations])
   AS_HELP_STRING(,[mmx:       64-bit MMX vector backend])
   AS_HELP_STRING(,[sse:       64-bit SSE vector backend])
   AS_HELP_STRING(,[sse2:      128-bit SSE2 vector backend])
   AS_HELP_STRING(,[ssse3:     128-bit SSSE3 vector backend])
   AS_HELP_STRING(,[altivec:   128-bit AltiVec vector backend])
   AS_HELP_STRING(,[vadmx:     64-bit VADMX vector backend])],
  [case ${enableval} in
   auto)
       ;;
   nonvector)
       OPT_BACKEND=
       ;;
   none)
       OPT_BACKEND=
       PLATFORM=generic
       ;;
   mmx | sse | sse2 | ssse3 | altivec | vadmx)
       OPT_BACKEND=${enableval}
       AM_CFLAGS="${AM_CFLAGS} -m${OPT_BACKEND}"
       ;;
   *)
       AC_MSG_ERROR([[Bad value "${enableval}" for --enable-backend option]])
       ;;
   esac])

# Allow for pretentious names and other special handling
case ${OPT_BACKEND} in
   altivec)
          backend_info_name=AltiVec

          # Need to check the supported syntax for vector initializers:
          # FSF GCC never supported ALTIVECPIM parentheses syntax and early
          # Apple GCC didn't support curly-braces.
          AC_MSG_CHECKING([for working vector initializer syntax])
          initializer=unknown
          saved_CFLAGS="$CFLAGS"
          CFLAGS="$CFLAGS $AM_CFLAGS"
          AC_COMPILE_IFELSE(
            [AC_LANG_PROGRAM(
               [[#include <altivec.h>]],
               [[vector signed int one = (vector signed int) {1, 1, 1, 1};]])],
            [curlies=1
             initializer='{}'],
            [AC_COMPILE_IFELSE(
               [AC_LANG_PROGRAM(
                  [[#include <altivec.h>]],
                  [[vector signed int one = (vector signed int) (1);]])],
               [curlies=0
                initializer='()'],
               [AC_MSG_FAILURE([[unknown vector initializer syntax]])])])
          CFLAGS="$saved_CFLAGS"
          AC_DEFINE_UNQUOTED(
            [RC_ALTIVEC_CURLY_BRACES],
            [$curlies],
            [Define to 1 to use {1, 1, ...} instead of (1) in vector literals])
          AC_MSG_RESULT([$initializer])
          ;;
   *)
          backend_info_name=`echo ${OPT_BACKEND} | tr $az $AZ`
          ;;
esac

# Check for tune cache option
OPT_TUNECACHE=default
AC_ARG_ENABLE([tune-cache],
  [AS_HELP_STRING([[--enable-tune-cache@<:@=yes@:>@]],
                  [Use the pre-tuned cached configuration if it exists.])
   AS_HELP_STRING(,[It has no effect if --enable-backend=none.])],
  [case ${enableval} in
   yes)
       OPT_TUNECACHE=yes
       ;;
   no)
       if test ${PLATFORM} != generic; then
           OPT_TUNECACHE=no
       else # Avoid special-casing below as much as possible.
           OPT_TUNECACHE=yes
       fi
       ;;
   *)
       AC_MSG_ERROR([[Bad value "${enableval}" for --enable-tune-cache option]])
       ;;
   esac])

# Update the PLATFORM identifier
if test -n "${OPT_BACKEND}"; then
    AC_MSG_NOTICE([[Using vector backend: ${OPT_BACKEND}]])
    PLATFORM=${PLATFORM}-${OPT_BACKEND}
    enable_simd=1
    rapp_vector_backend_header='"rc_vec_'${OPT_BACKEND}'.h"'
else
    AC_MSG_NOTICE([[No vector backend will be used]])
    rapp_vector_backend_header='"no-such-backend-header-exists.h"'
    enable_simd=0
fi
AC_DEFINE_UNQUOTED([RAPP_ENABLE_SIMD], [${enable_simd}],
                   [Define to 1 when a SIMD back-end exists and is enabled])
AC_DEFINE_UNQUOTED([RAPP_VECTOR_BACKEND_HEADER],
                   [${rapp_vector_backend_header}],
                   [Name of SIMD back-end header file])
AC_DEFINE_UNQUOTED([RAPP_INFO_SIMD], ["${backend_info_name}"],
                   [Name of SIMD back-end])

# Add the BUILD flag
AM_CPPFLAGS="${AM_CPPFLAGS} -DRAPP_BUILD"

# Default to -Werror iff this is not a final release
enable_werror=no
test ${rapp_finalized_release} = no && enable_werror=yes
AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([[--enable-werror]],
                  [Treat compiler warnings as errors, with GCC])],
  [enable_werror=${enableval}
   case ${enableval} in
   yes|no) ;;
   *) AC_MSG_ERROR([[Bad value "${enableval}" for --enable-werror option]]);;
   esac])
if test ${enable_werror} = yes; then
    AX_CFLAGS_GCC_OPTION([-Werror], [AM_CFLAGS])
fi

# Set the default compiler flags. See m4/ax_compiler_vendor.m4
# for more "vendors".
rapp_gflag=
case ${ax_cv_c_compiler_vendor} in
gnu)
    # -Wextra is an alias for -W but it's not in old releases
    AM_CFLAGS="${AM_CFLAGS} -W -Wall"
    AX_CFLAGS_GCC_OPTION([-fvisibility=hidden], [AM_CFLAGS])
    AX_CFLAGS_GCC_OPTION([-g3], [rapp_gflag])
    ;;
esac

# Set debug/release compiler flags
if test ${OPT_DEBUG} = yes; then
    AM_CFLAGS="${AM_CFLAGS} -O0"
else
    AM_CPPFLAGS="${AM_CPPFLAGS} -DNDEBUG=1"
    AM_CFLAGS="${AM_CFLAGS} -O2"
    case ${ax_cv_c_compiler_vendor} in
    gnu)
        AX_CFLAGS_GCC_OPTION([-fomit-frame-pointer], [AM_CFLAGS])
        AX_CFLAGS_GCC_OPTION([-funroll-loops], [AM_CFLAGS])
        ;;
    esac
fi

test "x${rapp_gflag}" = x && test "$ac_cv_prog_cc_g" != no && rapp_gflag=-g
test ${OPT_DEBUG} != no && AM_CFLAGS="${AM_CFLAGS} ${rapp_gflag}"

# Check if we will use a tune-cache
TUNEFILE=${srcdir}/compute/tune/arch/rapptune-${PLATFORM}.h
if test ${OPT_TUNECACHE} = yes && test ! -f "${TUNEFILE}"; then
    AC_MSG_WARN([[Option --enable-tune-cache=yes ignored (no "${TUNEFILE}")]])
fi

RC_BMARK_SED='s:.*@<:@^"@:>@RC_BMARK_ENTRY(\(rc_@<:@a-zA-Z0-9_@:>@*\).*:\1:p'
RC_TUNE_SED='s,^.define.\(rc_@<:@a-zA-Z0-9_@:>@*\)_IMPL.*,\1,p'
AC_SUBST([RC_BMARK_SED])
AC_SUBST([RC_TUNE_SED])
rapp_use_tunecache=no
rapp_do_fallbacks=no
if test ${OPT_TUNECACHE} != no && test -f "${TUNEFILE}" \
    && test ${rapp_tune_generation} = no; then
    # If the tunefile is up-to-date (same tuned set as would be
    # generated) or if --enable-tune-cache(=yes) was explicit,
    # then yes.
    sed -ne ${RC_BMARK_SED} \
       < ${srcdir}/compute/tune/benchmark/rc_benchmark.c \
       > srcfuncs
    sed -ne ${RC_TUNE_SED} < ${TUNEFILE} > cachefuncs
    if (cmp srcfuncs cachefuncs) > /dev/null 2>&1; then
        rapp_use_tunecache=yes
    else
        if test ${PLATFORM} = generic; then
            rapp_do_fallbacks=yes
            rapp_use_tunecache=yes
        elif test ${OPT_TUNECACHE} = default; then
            AC_MSG_NOTICE([[Re-tuning: outdated tune-file "${TUNEFILE}"]])
        else
            # Don't lie about the fallbacks, just fail
            if test ${rapp_finalized_release} = yes; then
                AC_MSG_WARN(
                    [[Untuned fallbacks: outdated tune-file "${TUNEFILE}"]])
                rapp_do_fallbacks=yes
            else
                AC_MSG_ERROR(
                    [[Outdated "${TUNEFILE}".
  A configure without --enable-tune-cache will cause it to be rebuilt]])
            fi
            rapp_use_tunecache=yes
        fi
    fi
    rm -f srcfuncs cachefuncs
fi

# True when tuning in compute/
AM_CONDITIONAL([RAPP_TUNING], [test ${rapp_tune_generation} = yes])

# True when using the pre-computed tunecache
AM_CONDITIONAL([RAPP_USE_TUNECACHE], [test ${rapp_use_tunecache} = yes])

CONFIG_ARGS_FOR_TUNING=${ac_configure_args}

DLLIB=
AM_COND_IF([RAPP_USE_TUNECACHE],
  [],
  [if test -z "${PYTHON}"; then
       AC_MSG_ERROR(
         [[No python found, required to create tune-files (try 2.4 - 2.7)]])
   fi

   # Besides the known working versions, accept future 2.x series
   if test ${PYTHON} = python; then
       if python -V | ${EGREP} 'Python 2.@<:@456789@>:@'; then
          :
       else
          AC_MSG_ERROR(
            [[No python 2.4 - 2.9 found, required to create tune-files]])
       fi
   fi

   # This just can't be the right way to find out from libtool where
   # if and how, to find dlopen, undocumented variables and all!
   if test ${lt_cv_dlopen} = no; then
       AC_MSG_ERROR([[No support for dlopen, required to create tune-files]])
   elif test ${lt_cv_dlopen} != dlopen; then
       AC_MSG_ERROR([[Unsupported dlopen flavor, please contact maintainers]])
   fi

   DLLIB=${lt_cv_dlopen_libs}

   # When we need to generate a tune-file, we'll reconfigure once for
   # each candidate, which is just redundant. If no config-file is
   # specified, use one in ${ac_abs_top_builddir}. If one is specified,
   # make sure it has an absolute path. (N.B.: RAPP_TUNING must be
   # handled gracefully.)
   test "x${cache_file}" = x/dev/null && cache_file=
   x=${CONFIG_ARGS_FOR_TUNING}
   case ${cache_file} in
   @<:@\\/@:>@* | ?:@<:@\\/@:>@*) ;;
   *)
         test "x${cache_file}" = x \
          && cache_file=config.cache && rm -f ${cache_file}
         cache_file=`pwd`/${cache_file}
         x="${x} '--cache-file=${cache_file}'"
         AC_MSG_NOTICE([[Using config cache-file '${cache_file}']])
         # Now that we (may) have hacked a cache-file, make
         # sure it's there (really, that it passes "test -w")
         touch ${cache_file}
         ;;
   esac

   # FIXME: remove the following when we start generating
   # releases with a new autoconf version that doesn't nag.
   # Avoid "--host but not --build specified" obnoxiousness,
   # very visible during silent re-tuning phase.  This also
   # requires that we scratch the autoconf cache-file for the
   # initial run, as I can't figure out how to insert a build_alias
   # setting and having a different cached value yields a fatal
   # error message.
   if test "x${build_alias}" = x && test "x${host_alias}" != x; then
       x="${x} --build=${build}"
       cache_file=/dev/null
   fi

   # Don't lose to relative --srcdir options (such as happens
   # for "make distcheck"; just override with the absolute path.
   case ${srcdir} in
   @<:@\\/@:>@* | ?:@<:@\\/@:>@*) ;;
   *)
         x="${x} '--srcdir=`cd ${srcdir} && pwd`'"
         ;;
   esac
   CONFIG_ARGS_FOR_TUNING=${x}])

INSTALLCHECK_CONFIG_OPTIONS=
AC_ARG_WITH([installcheck-config-options],
  [AS_HELP_STRING([[--with-installcheck-config-options=args]],
                  [Pass args to the installcheck configure])],
  [case ${withval} in
   yes|no)
         AC_MSG_ERROR(
             [[Bad value "${withval}" for --with-installcheck-config-options]])
         ;;
   *)
         INSTALLCHECK_CONFIG_OPTIONS=${withval}
         ;;
   esac])

# Identify building in the srcdir.
BUILD_IN_SRCDIR=no
test "`cd ${srcdir} && pwd`" = "`pwd`" && BUILD_IN_SRCDIR=yes

rb_xcandidates=
rb_ncandidates=0
rb_impls="generic swar simd"
rb_unrolls="1 2 4"
RAPP_TUNING_DEPENDENCIES=
for impl in ${rb_impls}; do
     for unroll in ${rb_unrolls}; do
         comb=${impl}_${unroll}
         rb_xcandidates="${rb_xcandidates} ${comb}.x"
         RAPP_TUNING_DEPENDENCIES="${RAPP_TUNING_DEPENDENCIES}
${comb}.cstamp: ${comb}.x
${comb}.bstamp: ${comb}.cstamp
${comb}.istamp: ${comb}.bstamp archive.stamp"
         rb_ncandidates=`expr ${rb_ncandidates} + 1`
     done
done
RB_XCANDIDATES=${rb_xcandidates}
RB_NCANDIDATES=${rb_ncandidates}

# Substitute variables in output files
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([PLATFORM])
AC_SUBST([RAPP_MAJOR_VERSION])
AC_SUBST([RAPP_MINOR_VERSION])
AC_SUBST([RAPP_LIB_CUR])
AC_SUBST([RAPP_LIB_REV])
AC_SUBST([RAPP_LIB_AGE])
AC_SUBST([CONFIG_ARGS_FOR_TUNING])
AC_SUBST([DLLIB])

AC_SUBST([INSTALLCHECK_CONFIG_OPTIONS])
AC_SUBST([BUILD_IN_SRCDIR])
AC_SUBST([RAPP_TUNING_DEPENDENCIES])
AM_SUBST_NOTMAKE([RAPP_TUNING_DEPENDENCIES])
AC_SUBST([RB_XCANDIDATES])
AC_SUBST([RB_NCANDIDATES])

# Output
AC_CONFIG_HEADERS([config.h])

# The directory test/installtest isn't configured until after
# installation, but to make autoreconf update this directory we
# have to mention it here
if false; then
    AC_CONFIG_SUBDIRS([test/installtest])
fi

AM_COND_IF([RAPP_TUNING],
  [],
  # Setup links for out-of-source builds with doxygen
  [AM_COND_IF([HAVE_DOXYGEN],
     [AC_CONFIG_LINKS([COPYING:COPYING
                       COPYING.LESSER:COPYING.LESSER
                       COPYING.FDL:COPYING.FDL])])])

# We always create all files, though we don't visit all when building
AC_CONFIG_FILES([Makefile
                 Doxyfile
                 rapp.pc
                 rapp.spec
                 include/rapp_version.h
                 driver/Makefile
                 doxygen/images/Makefile
                 compute/Makefile
                 compute/Doxyfile
                 compute/include/Makefile
                 compute/common/Makefile
                 compute/backend/Makefile
                 compute/backend/test/Makefile
                 compute/doxygen/images/Makefile
                 compute/generic/Makefile
                 compute/vector/Makefile
                 compute/tune/Makefile
                 compute/tune/benchmark/Makefile
                 compute/tools/Makefile
                 benchmark/Makefile
                 test/Makefile
                 test/reference/Makefile])

# Link the tuned configuration file
AM_COND_IF([RAPP_USE_TUNECACHE],
  [AC_MSG_NOTICE([[Using predefined configuration:]])
   AC_MSG_NOTICE([[${TUNEFILE}]])
   if test ${rapp_do_fallbacks} = no; then
       AC_CONFIG_LINKS([compute/tune/rapptune.h:${TUNEFILE}])
   fi],

  # Not using an existing tunecache:
  # If we're in the process of generating a tuning candidate,
  # just create a dummy, otherwise emit a suitable message.
  # We'll have to wait creating the dummy until after AC_OUTPUT
  # below, when the directories exist.
  [AM_COND_IF([RAPP_TUNING],
     [],
     [if test -f "${TUNEFILE}"; then
           AC_MSG_NOTICE([[Ignoring existing tune-file "${TUNEFILE}".]])
      else
           AC_MSG_NOTICE(
               [[RAPP is not tuned for this platform (${PLATFORM}).]])
      fi])])

# Link the benchmark plot file
PLOTFILE=${srcdir}/benchmark/arch/benchmarkplot-${PLATFORM}.html
if test ${rapp_use_tunecache} = yes && test -f "${PLOTFILE}"; then
    AC_CONFIG_LINKS([benchmark/benchmarkplot.html:${PLOTFILE}])
elif test ${rapp_tune_generation} = no; then
    if test -f "${PLOTFILE}"; then
        AC_MSG_NOTICE([[Ignoring existing benchmark plot-file "${PLOTFILE}".]])
    else
        AC_MSG_NOTICE(
            [[RAPP is not benchmarked on this platform (${PLATFORM}).]])
    fi
fi

AC_OUTPUT

# See AC_OUTPUT comment above.
AM_COND_IF([RAPP_USE_TUNECACHE],
           [AM_COND_IF([RAPP_TUNING],
              [rm -f compute/tune/rapptune.h
               touch compute/tune/rapptune.h],
              [# Is there a need to be able to build out-of-the-box using
               # mismatching tune-files during development?  For now assume
               # no; people that build using development sources must know
               # what to do. For releases yes: the intent of the fallbacks
               # is to avoid (build) failure (reports) for systems without
               # developer access or sufficient interest from developers to
               # re-generate the tune-cache files.
               if test ${rapp_do_fallbacks} = yes; then
                   echo '#include "'${TUNEFILE}'"' > compute/tune/rapptune.h
                   f=compute/tune/rapptune.h
                   ff=${srcdir}/compute/tune/benchmark/rc_benchmark.c
                   for fn in `sed -ne ${RC_BMARK_SED} < $ff`; do
                       echo "#ifndef ${fn}_IMPL" >> $f
                       test ${PLATFORM} != generic \
                        && echo "#warning \"${fn}\" is not tuned" >> $f
                       case ${fn} in
                       rc_bitblt_v*)
                           echo "#define ${fn}_IMPL RC_IMPL_SWAR" >> $f;;
                       *)
                           echo "#define ${fn}_IMPL RC_IMPL_GEN" >> $f;;
                       esac
                       echo "#define ${fn}_UNROLL 1" >> $f
                       echo "#define ${fn}_SCORE 0.0" >> $f
                       echo "#endif" >> $f
                   done
               fi])])
