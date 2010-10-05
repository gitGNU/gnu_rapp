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
#  A progress bar for RAPP autotuning scripts.
#############################################################

update()
{
    # Determine the print mechanism
    if (printf "") 2> /dev/null; then
	# Use printf if available
	PRINT="printf"
	PERC="%%"
	BACK="\b"
    elif test "x`echo -ne`" != x-ne; then
	# Use echo with n (no newline) and e (special characters)
	PRINT="echo -ne"
        PERC="%"
	BACK="\b"
    else
	# Fallback - use plain echo
	PRINT="echo"
	PERC="%"
	BACK=""
    fi

    # Bar style constants
    BEGIN="["
    END="]"
    FILL="="
    NONE="."

    # Set COLUMNS
    test -n "${COLUMNS}" || COLUMNS=80

    # Compute bar properties. We use the portable expr rather than $(()).
    WIDTH=`expr ${COLUMNS} - 12`
    POS=`expr '(' $1 '*' ${WIDTH} + 50 ')' / 100`
    REM=`expr ${WIDTH} - ${POS}`

    # Plot variables
    fill=""
    none=""
    back=${BACK}
    clear=${BACK}${BACK}${BACK}${BACK}

    # Generate the plot strings
    pos=0
    while test ${pos} -lt ${POS}; do
	fill=${fill}${FILL}
	clear=${clear}${BACK}
	pos=`expr ${pos} + 1`
    done

    pos=0
    while test ${pos} -lt ${REM}; do
	none=${none}${NONE}
	back=${back}${BACK}
	pos=`expr ${pos} + 1`
    done

    # Get the percentage number padding
    pad=""
    test $1 -lt 100 && pad=" "
    test $1 -lt 10  && pad="  "

    # Plot the bar
    ${PRINT} "${clear}${back}${pad}$1${PERC} ${BEGIN}${fill}${none}${END}${back}" 1>&2
}

if test $# && test $1 -ge 0 && test $1 -le 100; then
    update $1
else
    echo "Usage: $0 <percentage>"
fi
