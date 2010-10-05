#!/bin/sh

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


#############################################################
# Script for creating self-extracting shell scripts.
#############################################################

# Check arguments
if test $# -lt 2; then
    echo "Create a self-extracting shell script." >&2
    echo "Usage: $0 <script> <payload-dir>" >&2
    exit 1
fi

# Create a temporary directory the archive
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

# Set up the payload and bootstrap in TMPDIR
mkdir ${TMPDIR}/payload || exit 1
ln -s `pwd`/$2/* ${TMPDIR}/payload/ || exit 1
cp $1 ${TMPDIR}/bootstrap.sh || exit 1
chmod a+x ${TMPDIR}/bootstrap.sh || exit 1

# Create the archive
(cd ${TMPDIR} && tar -chf - bootstrap.sh payload | gzip > ${TMPDIR}/selfextract.tgz) || exit 1

# Add the self-extracting script header
cat selfheader.sh ${TMPDIR}/selfextract.tgz || exit 1

# Clean up
rm -rf ${TMPDIR}
