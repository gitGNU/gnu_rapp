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


################################################################
#  RAPP Tuner performance measure script for automatic tuning.
################################################################

# The library module base name
MODULE=rappcompute_tune

# The benchmark application name
BENCHMARK=benchmark

# The progress bar command
PROGRESS=./progress.sh

# The output data file
OUTFILE=tunedata.py

# The implementation names
IMPL="generic swar simd"

# The unroll factors
UNROLL="1 2 4"

# The test case image resolutions
RES="256x256 512x512 16384x2"

# The default measurement time in milliseconds
MSECS=100

# The total number of cases
nres=`echo ${RES} | wc -w`
nimpl=`echo ${IMPL} | wc -w`
nunroll=`echo ${UNROLL} | wc -w`

# N.B.: $(()) is not portable, but expr is
TOTAL=`expr '(' ${nres} '*' ${nimpl} '*' ${nunroll} ')'`

# Set argument measurement time
test $# -gt 0 &&  MSECS=$1

# Display initial info and progress
echo
echo "Running benchmark suites..."
./${PROGRESS} 0

# Write the header
echo "# RAPP tuning data" >  ${OUTFILE}
echo "# `uname -a`"       >> ${OUTFILE}

# Write platform info
echo arch=\'`uname -a`\'  >> ${OUTFILE}
echo compilerversion=\'`./${BENCHMARK} -c`\' >> ${OUTFILE}

# Write data object name
echo "data=[" >> ${OUTFILE}

# Execute all performance tests
count=0
for impl in ${IMPL}; do
    for unroll in ${UNROLL}; do
	for res in ${RES}; do
	    # Parse the resolution string
	    width=` echo ${res} | sed 's/x[0-9]*//'`
	    height=`echo ${res} | sed 's/[0-9]*x//'`

            # Write test meta data
	    echo "{'impl'  : '${impl}'," >> ${OUTFILE}
	    echo " 'unroll': ${unroll}," >> ${OUTFILE}
	    echo " 'width' : ${width},"  >> ${OUTFILE}
	    echo " 'height': ${height}," >> ${OUTFILE}
	    echo " 'data'  : \\"         >> ${OUTFILE}

	    # Create the library name
	    name=${MODULE}_${impl}_${unroll}.*

	    # Write measurement data
	    ./${BENCHMARK} -l ./${name} -w ${width} -h ${height} \
		           -m ${MSECS} -p >> ${OUTFILE}

	    # Check for errors
            test $? -gt 0 && exit 1

	    # Write data entry termination character
	    echo "}," >> ${OUTFILE}

	    # Update and display progress
	    count=`expr ${count} + 1`
	    ./${PROGRESS} `expr '(' 100 '*' ${count} ')' / ${TOTAL}`
	done
    done
done

# Write data object termination character
echo "]" >> ${OUTFILE}
echo
