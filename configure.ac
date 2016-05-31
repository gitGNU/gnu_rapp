#  Autoconf file for the RAPP library.

#  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
m4_define([rapp_minor_version], [8])
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

# Allow (downstream) users to use --disable-maintainer-mode to avoid
# autotools-version-related breakages when timestamps differ (almost
# always with sources imported into a VC system).  This line by itself
# only means that --disable-maintainer-mode *can* be used.
AM_MAINTAINER_MODE([enable])

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
RAPP_LIB_CUR=2

# Interface revision
RAPP_LIB_REV=0

# Interface age
RAPP_LIB_AGE=2

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
AC_CHECK_HEADERS([stdint.h limits.h], [],
  [AC_MSG_ERROR([[Required header file ${ac_header} missing]])])
AC_CHECK_HEADERS([malloc.h])
have_standard_memalign=unknown
AC_CHECK_FUNCS([memalign posix_memalign], [have_standard_memalign=yes])
# Fallbacks provided for historic reasons and lack of testing.
# FIXME: We shouldn't emit an error when malloc is sufficient,
# but we can't check RC_ALIGNMENT here.
if test ${have_standard_memalign} != yes; then
    AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(, [[
#if !defined(__APPLE__) && !defined(__WIN32__)
choke me
#endif]])],
      [],
      [AC_MSG_FAILURE([[Missing or unknown memalign equivalent]])])
fi

# Check C compiler vendor
AX_COMPILER_VENDOR

# Find out the native size.  We use the sizeof a pointer-containing int.
# Perhaps we could use UINTPTR_MAX, but this is easier.
AC_TYPE_UINTPTR_T
AC_CHECK_SIZEOF([uintptr_t])
case ${ac_cv_sizeof_uintptr_t} in
2|4|8) ;;
*)
    # More changes are needed than updating the match above.
    AC_MSG_ERROR([[Unsupported native word size: ${ac_cv_sizeof_uintptr_t}]])
    ;;
esac
AC_DEFINE_UNQUOTED([RC_COMPUTED_NATIVE_SIZE], [${ac_cv_sizeof_uintptr_t}],
                   [Define to the default unit access size; one of 2 4 and 8])
rc_bits=`expr ${ac_cv_sizeof_uintptr_t} '*' 8`

# Check for the CHECK unit test framework
AC_ARG_VAR([CHECK], [the CHECK unit test framework])
# Need to do this rather than the automatic AC_PATH_TOOL
# invocation from the PKG_CHECK_MODULES usage, in order to avoid
# nagging that a cross-version wasn't found (over and over again
# at tuning; PKG_CONFIG being declared precious apparently has
# no effect).
AC_PATH_PROGS([PKG_CONFIG], [${host}-pkg-config pkg-config])

# We use --with-check/--without-check mainly because
# --disable-check sounds a little bit more like we're disabling
# the testsuite or some runtime-checks.
rapp_try_check=yes
AC_ARG_WITH([check],
  [AS_HELP_STRING([[--with-check]],
                  [Use the CHECK test framework if present])
   AS_HELP_STRING(,[(default yes)])],
  [rapp_try_check=${withval}
   case ${withval} in
   yes|no) ;;
   *)
       AC_MSG_ERROR([[Bad value "${withval}" for --with-check option]])
       ;;
   esac])

have_check=no
if test ${rapp_try_check} = yes; then
    # Don't regenerate configure without the pkg-config package;
    # it's required to expand the PKG_CHECK_MODULES below.
    PKG_CHECK_MODULES(
      [CHECK],
      [check >= 0.9.4],
      [AC_MSG_CHECKING([[whether CHECK as installed can be linked in]])
       saved_CFLAGS="$CFLAGS"
       CFLAGS="$CFLAGS $AM_CFLAGS $CHECK_CFLAGS"
       saved_LIBS="$LIBS"
       LIBS="$LIBS $CHECK_LIBS"
       AC_LINK_IFELSE([AC_LANG_PROGRAM(
                         [[#include <stddef.h>
                           #include <check.h>]],
                         [[SRunner *sr = srunner_create(NULL);]])],
                      [[have_check=yes]],
                      [[CHECK_CFLAGS= CHECK_LIBS=]])
       CFLAGS="$saved_CFLAGS"
       LIBS="$saved_LIBS"
       AC_MSG_RESULT([[$have_check]])],
       # Just a dummy to make CHECK optional, allowing failure here.
       [:])
fi
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

# Determine the vector backend from predefined macros and other observations.
AC_MSG_NOTICE([[Scanning default present vector support]])
AC_CHECK_DEFINED([__MMX__], [OPT_BACKEND=mmx])
AC_CHECK_DEFINED([__SSE__], [OPT_BACKEND=sse])
AC_CHECK_DEFINED([__mips_loongson_vector_rev], [OPT_BACKEND=loongson2ef])
AC_CHECK_DEFINED([__ARM_NEON__], [OPT_BACKEND=neon])
# For SPARC VIS and GCC, there are no macros or headers and we
# can't induce VIS from other macros as it's not "standard SPARC".
# To see if VIS is on by default, we have to try to compile
# something that'll fail if a VIS-specific builtin isn't defined.
# Even then, don't enable it; it fails silently on e.g. Niagara T1.
if test ${rapp_tune_generation} = no; then
    AC_MSG_CHECKING([[for intrinsics support for VIS]])
    have_vis=no
    AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
         [],
         [[typedef unsigned char v8qi __attribute__((__vector_size__(8)));
           v8qi x,y,z;
           void f(void){x=__builtin_vis_faligndatav8qi(y,z);}]])],
      # Adjust text and prepend "OPT_BACKEND=vis; " to enable by default.
      [have_vis="yes, configure with --enable-backend=vis to enable"]);
    AC_MSG_RESULT([[$have_vis]])
fi
AC_CHECK_DEFINED([__SSE2__], [OPT_BACKEND=sse2])
AC_CHECK_DEFINED([__SSSE3__], [OPT_BACKEND=ssse3])
AC_CHECK_DEFINED([__VEC__], [OPT_BACKEND=altivec])
AC_CHECK_DEFINED([__ALTIVEC__], [OPT_BACKEND=altivec])
AC_CHECK_DEFINED([__VADMX__], [OPT_BACKEND=vadmx])
AC_CHECK_DEFINED([__VADMX2__], [OPT_BACKEND=vadmx2])

# Set the PLATFORM_BASE string to be the AC 'host_cpu' plus
# the compiler vendor and PLATFORM appending a suffix built from
# native bit-size as defined by uintptr_t, if different than the
# first encountered (hopefully the most common) configuration,
# if there's more than one known.
PLATFORM_BASE=${host_cpu}-${ax_cv_c_compiler_vendor}

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
asserted_returns=0

# Due to using AC_CONFIG_SUBDIRS, we get no warnings for
# incorrect options (up-to-including-at-least autoconf-2.68).
# Add your favorite gotchas here until there'a a better
# solution.
AC_ARG_ENABLE([debugging],
  [AS_HELP_STRING([[--enable-debugging]],
                  [Incorrect spelling of --enable-debug])],
  [AC_MSG_ERROR([[Use --enable-debug, not --enable-debugging]])])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([[--enable-debug@<:@=no@:>@]], [Enable debugging:])
   AS_HELP_STRING(,[no:               Disable debugging])
   AS_HELP_STRING(,[yes:              Enable debugging])
   AS_HELP_STRING(,[asserted-returns: For erroneous parameters,]
                   [ call abort() instead of returning])],
  [OPT_DEBUG=${enableval}
   case ${enableval} in
   yes|no) ;;
   asserted-returns)
       OPT_DEBUG=yes
       asserted_returns=1
       ;;
   *)
       AC_MSG_ERROR([[Bad value "${enableval}" for --enable-debug option]])
       ;;
   esac])
AC_DEFINE_UNQUOTED([RC_ASSERTED_RETURNS], [${asserted_returns}],
                   [Define to 1 to not return but call abort()]
                   [ for erroneous parameters])

# Check for vector backend option.
# Unfortunately, the second field in the help-strings will
# not be vertically aligned as they are below.
backend_info_name=
AC_ARG_ENABLE([backend],
  [AS_HELP_STRING([[--enable-backend@<:@=auto@:>@]],
                  [Use vector accelerator backend:])
   AS_HELP_STRING(,[auto:        Select backend automatically])
   AS_HELP_STRING(,[nonvector:   Disable all vector operations])
   AS_HELP_STRING(,[none:        Disable all vector operations and tuned operations])
   AS_HELP_STRING(,[mmx:         64-bit MMX vector backend])
   AS_HELP_STRING(,[sse:         64-bit SSE vector backend])
   AS_HELP_STRING(,[sse2:        128-bit SSE2 vector backend])
   AS_HELP_STRING(,[ssse3:       128-bit SSSE3 vector backend])
   AS_HELP_STRING(,[altivec:     128-bit AltiVec vector backend])
   AS_HELP_STRING(,[vadmx:       64-bit VADMX vector backend])
   AS_HELP_STRING(,[vadmx2:      128-bit VADMX2 vector backend])
   AS_HELP_STRING(,[loongson2e:  64-bit Loongson-2E vector backend])
   AS_HELP_STRING(,[loongson2f:  64-bit Loongson-2F vector backend])
   AS_HELP_STRING(,[neon:        64-bit ARM NEON vector backend])
   AS_HELP_STRING(,[vis:         64-bit SPARC VIS 1 vector backend])],
  [case ${enableval} in
   yes|auto)
       ;;
   nonvector)
       OPT_BACKEND=
       ;;
   none)
       OPT_BACKEND=
       PLATFORM_BASE=generic
       ;;
   mmx | sse | sse2 | ssse3 | altivec | vadmx | vadmx2 | vis)
       OPT_BACKEND=${enableval}
       AM_CFLAGS="${AM_CFLAGS} -m${OPT_BACKEND}"
       ;;
   loongson2e | loongson2f)
       OPT_BACKEND=loongson2ef
       # The difference is in the object code, not the generated assembly code.
       AM_CFLAGS="${AM_CFLAGS} -march=${enableval}"
       ;;
   neon)
       OPT_BACKEND=${enableval}

       AC_CACHE_CHECK([for compiler options to enable NEON], [ac_cv_neon_opts],
         [neon_ok=no
          # If we already have __ARM_NEON__ defined, do nothing.
          # Otherwise, try adding the obvious option to enable it.
          (unset ac_cv_defined___ARM_NEON__) >/dev/null 2>&1 && unset ac_cv_defined___ARM_NEON__
          AC_CHECK_DEFINED([__ARM_NEON__], [neon_ok=yes])
          if test ${neon_ok} = yes; then
              ac_cv_neon_opts=""
          else
              neonsaved_CFLAGS=${CFLAGS}
              neon_opt="-mfpu=neon"
              AC_MSG_NOTICE([[Trying to trivially enable NEON by adding ${neon_opt}]])
              CFLAGS="${neonsaved_CFLAGS} ${neon_opt}"

              # GCC may ignore the -mfpu=neon if there's an ABI mismatch, so
              # if it *still* isn't defined, add -mfloat-abi=softfp, but
              # before the enable-neon option.
              # Also, we have to uncache the saved value for the test; we
              # want to re-evaluate it after updating CFLAGS, and
              # AC_CHECK_DEFINED doesn't conditionalize the caching on
              # unchanged CFLAGS as it should.
              (unset ac_cv_defined___ARM_NEON__) >/dev/null 2>&1 && unset ac_cv_defined___ARM_NEON__
              AC_CHECK_DEFINED([__ARM_NEON__],
                [ac_cv_neon_opts="${neon_opt}"],
                [neon_abiadd="-mfloat-abi=softfp"
                 AC_MSG_NOTICE([[Also adding ${neon_abiadd} to enable NEON]])
                 ac_cv_neon_opts="${neon_abiadd} ${neon_opt}"])
              CFLAGS=${neonsaved_CFLAGS}
          fi])
       AM_CFLAGS="${AM_CFLAGS} ${ac_cv_neon_opts}"
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
          AC_MSG_CHECKING([[for working vector initializer syntax]])
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
          AC_MSG_RESULT([[$initializer]])
          ;;
   vis)
          backend_info_name="VIS 1"

          # Need to provide a macro to show explicitly when we
          # want to use this support, as there is no indicator
          # other than that the code compiles.
          AC_DEFINE_UNQUOTED(
            [RC_VEC_BACKEND_VIS],
            [1],
            [Define to 1 when using the RAPP VIS vector back-end])
          ;;

   loongson2ef)
          backend_info_name="Loongson-2E and -2F"
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
       if test ${PLATFORM_BASE} != generic; then
           OPT_TUNECACHE=no
       else # Avoid special-casing below as much as possible.
           OPT_TUNECACHE=yes
       fi
       ;;
   *)
       AC_MSG_ERROR([[Bad value "${enableval}" for --enable-tune-cache option]])
       ;;
   esac])

# Update the PLATFORM_BASE identifier.
if test -n "${OPT_BACKEND}"; then
    AC_MSG_NOTICE([[Using vector backend: ${OPT_BACKEND}]])
    PLATFORM_BASE=${PLATFORM_BASE}-${OPT_BACKEND}
    enable_simd=1
    rapp_vector_backend_header='"rc_vec_'${OPT_BACKEND}'.h"'
else
    AC_MSG_NOTICE([[No vector backend will be used]])
    rapp_vector_backend_header='"no-such-backend-header-exists.h"'
    enable_simd=0
fi
# Form PLATFORM from PLATFORM_BASE, different for platforms
# where multiple natural-word-length ABIs exist.
PLATFORM=${PLATFORM_BASE}
case ${host_cpu} in
x86_64)
    test ${rc_bits} != 64 && PLATFORM="${PLATFORM_BASE}-${rc_bits}"
    ;;
sparc*|mips*|powerpc*)
    test ${rc_bits} != 32 && PLATFORM="${PLATFORM_BASE}-${rc_bits}"
    ;;
esac

AC_DEFINE_UNQUOTED([RAPP_ENABLE_SIMD], [${enable_simd}],
                   [Define to 1 when a SIMD back-end exists and is enabled])
AC_DEFINE_UNQUOTED([RAPP_VECTOR_BACKEND_HEADER],
                   [${rapp_vector_backend_header}],
                   [Name of SIMD back-end header file])
AC_DEFINE_UNQUOTED([RAPP_INFO_SIMD], ["${backend_info_name}"],
                   [Name of SIMD back-end])

# Add the BUILD flag
AM_CPPFLAGS="${AM_CPPFLAGS} -DRAPP_BUILD"

# Default to -Werror iff this is not a final release. Don't set
# the variable "enable_werror" here, as that will force the setting.
rapp_enable_werror=no
test ${rapp_finalized_release} = no && rapp_enable_werror=yes
AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([[--enable-werror]],
                  [Treat compiler warnings as errors, with GCC])],
  [rapp_enable_werror=${enableval}
   case ${enableval} in
   yes|no) ;;
   *) AC_MSG_ERROR([[Bad value "${enableval}" for --enable-werror option]]);;
   esac])
if test ${rapp_enable_werror} = yes; then
    AX_CFLAGS_GCC_OPTION([-Werror], [AM_CFLAGS])
    AX_CFLAGS_GCC_OPTION([-Wno-error=cpp], [HASHWARN_CFLAGS])
    case " ${HASHWARN_CFLAGS} " in
    *" -Wno-error=cpp "*) ;;
    *) AX_CFLAGS_GCC_OPTION([-Wno-error], [HASHWARN_CFLAGS]) ;;
    esac
fi

# Set the default compiler flags. See m4/ax_compiler_vendor.m4
# for more "vendors".
rapp_gflag=
case ${ax_cv_c_compiler_vendor} in
gnu)
    # -Wextra is an alias for -W but it's not in old releases.
    AM_CFLAGS="${AM_CFLAGS} -W -Wall"
    # The type of errors caught by -Wshadow can happen easily with
    # the layers of macros we use.  It's also popular among
    # downstream users.
    AM_CFLAGS="${AM_CFLAGS} -Wshadow"
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

rapp_enable_logging=no
AC_ARG_ENABLE([logging],
  [AS_HELP_STRING([[--enable-logging@<:@=no@:>@]],
                  [Log calls to the RAPP API functions. See docs for details.])],
  [rapp_enable_logging=${enableval}
   case ${enableval} in
   yes|no) ;;
   *) AC_MSG_ERROR([[Bad parameter "${enableval}" for --enable-logging option]]);;
   esac])
if test ${rapp_enable_logging} = yes; then
    AM_CPPFLAGS="${AM_CPPFLAGS} -DRAPP_LOGGING"
fi
AM_CONDITIONAL([RAPP_LOGGING], [test ${rapp_enable_logging} = yes])

# Check if we will use a tune-cache.
if test ${PLATFORM_BASE} = generic; then
    TUNEFILE=${srcdir}/compute/tune/arch/rapptune-generic.h
else
    TUNEFILE=${srcdir}/compute/tune/arch/rapptune-${PLATFORM}.h
fi
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
        if test ${PLATFORM_BASE} = generic; then
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

   AM_COND_IF([RAPP_LOGGING],
     [AC_MSG_ERROR([[Tuning with --enable-logging refused: would fill ${TMPDIR-/tmp}]])])

   # Besides the known working versions, accept future 2.x series
   if test ${PYTHON} = python; then
       if python -V 2>&1 | ${EGREP} 'Python 2.@<:@456789@:>@'; then
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
AC_SUBST([HASHWARN_CFLAGS])
AC_SUBST([PLATFORM])
AC_SUBST([PLATFORM_BASE])
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
# have to mention it in a AC_ CONFIG_SUBDIRS call here, one
# which doesn't really have to execute; it can be in an "if
# false" conditional. Unfortunately, calling that macro turns
# off warnings for unknown configure options (just to be safe,
# we also slightly obfuscate its spelling in this comment). Fool
# autoreconf into thinking we called AC_ CONFIG_SUBDIRS here by
# temporily suppressing its definition. If that stops working in
# a future autoconf release, ignore the configure options issue
# and use the "if false" construct.
m4_pushdef([AC_CONFIG_SUBDIRS], [])
AC_CONFIG_SUBDIRS([test/installtest])
m4_popdef([AC_CONFIG_SUBDIRS])

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
if test ${PLATFORM_BASE} = generic; then
    PLOTFILE=${srcdir}/benchmark/arch/benchmarkplot-generic.html
else
    PLOTFILE=${srcdir}/benchmark/arch/benchmarkplot-${PLATFORM}.html
fi
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
                       test ${PLATFORM_BASE} != generic \
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
