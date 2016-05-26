#!/bin/sh

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


##############################################################
#  Self-extracting script.
##############################################################

# Get the line offset in a file for a given pattern
getoffset()
{
    # Use awk if available
    if x=`(/usr/bin/awk 'BEGIN{print "yes";}' < /dev/null) 2> /dev/null` \
        && test "$x" = yes; then
        awk "/$2/ {print NR + 1; exit 0; }" $1
    else
        # Use grep -n or grep -na
        GREPOPT=-n
        if (echo "" | grep -a "") > /dev/null 2>&1; then
            GREPOPT=-na
        fi
        # The tr in busybox 0.52 requires [] around the set
        offs=`grep ${GREPOPT} "$2" $1 | tr -dc '[0-9]'`
	# N.B.: $(()) is not portable, but expr is
        expr '(' ${offs} + 1 ')'
    fi
}

test -n "${RAPP_SCRIPT_TRACE}" && set -x

# Create a temporary directory for extracted files
# As mktemp has no -d in busybox 0.52 we go for the autoconf-documented method
: ${TMPDIR=/tmp}
{
  tmp=`
    (umask 077 && mktemp -d "$TMPDIR/selfheader.XXXXXX") 2>/dev/null
  ` &&
  test -n "$tmp" && test -d "$tmp"
} || {
  tmp=$TMPDIR/selfheader.$$-$RANDOM
  (umask 077 && mkdir "$tmp")
} || exit $?
TMPDIR=$tmp
CURDIR=`pwd`

# Get the line number where the archive starts
OFFSET=`getoffset $0 '^# -- ARCHIVE BOUNDARY --'`

# Extract the archive data
tail -n +${OFFSET} $0 | (cd ${TMPDIR} && gunzip | tar -xf -)
test $? -ne 0 && {
  echo
  echo "$0: Maybe the error is tar failing to read uncompressed, retrying..."
  rm -rf ${TMPDIR}/payload ${TMPDIR}/bootstrap.sh
  # The broken tar ("tar (GNU tar) 1.15") seems to read stdin wrongly for
  # the raw tar file, probably the very bug fixed in 1.15.1. A "tar xf -
  # < file" works, but "cat file | tar xf -" does not. It does have "z"
  # capability, working around the issue, so we try to use that instead of
  # going through gunzip. Don't copy the tarfile instead of the pipe: we
  # may not have room. Don't use "z" always: tar might not support it.
  tail -n +${OFFSET} $0 | (cd ${TMPDIR} && tar -zxf -)
  test $? -ne 0 && echo "$0: Nope, that didn't fix it. Sorry!" && exit 1
}

# Stop now, if so requested
test -n "${RAPP_STOP_AFTER_SELFEXTRACT}" &&
  echo "$0: extract at ${TMPDIR} stopped: RAPP_STOP_AFTER_SELFEXTRACT set." &&
  exit 0

# Run the bootstrap script
cd ${TMPDIR}/payload && ../bootstrap.sh ${CURDIR}
cd ${CURDIR}

# Clean up
test -n "${RAPP_KEEP_TMPDIR}" || rm -rf ${TMPDIR}

exit 0

# -- ARCHIVE BOUNDARY --
