/*  Copyright (C) 2005-2010, Axis Communications AB, LUND, SWEDEN
 *
 *  This file is part of RAPP.
 *
 *  RAPP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  You can use the comments under either the terms of the GNU Lesser General
 *  Public License version 3 as published by the Free Software Foundation,
 *  either version 3 of the License or (at your option) any later version, or
 *  the GNU Free Documentation License version 1.3 or any later version
 *  published by the Free Software Foundation; with no Invariant Sections, no
 *  Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the documentation section entitled
 *  "GNU Free Documentation License".
 *
 *  RAPP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License and a copy of the GNU Free Documentation License along
 *  with RAPP. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file   rc_rasterize.c
 *  @brief  RAPP Compute layer chain code line rasterization.
 */

#include <stdlib.h>       /* abs()        */
#include <assert.h>       /* assert()     */
#include "rc_util.h"      /* MIN(), MAX() */
#include "rc_rasterize.h" /* Exported API */


/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Swap integers a and b.
 */
#define RC_RASTERIZE_SWAP(a, b) \
do {                            \
    int tmp_ = (a);             \
    (a) = (b);                  \
    (b) = tmp_;                 \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Rasterize a 4-connectivity chain code.
 */
int
rc_rasterize_4conn(char *line, int x0, int y0, int x1, int y1)
{
    int dx  = abs(x1 - x0);    /* Absolute x diff            */
    int dy  = abs(y1 - y0);    /* Absolute y diff            */
    int len = dx + dy;         /* Chain code length          */

    int pos   = 0;             /* Buffer position            */
    int err   = 0;             /* Accumulated error          */
    int step  = 1;             /* Step in x = (+/-)1         */

    int ahead = 0;             /* Ahead direction chain code */
    int side  = 0;             /* Side direction chain code  */
    int dir   = -1;

    int x;

    /* Swap x and y */
    if (dy > dx) {
        RC_RASTERIZE_SWAP(x0, y0); /* Swap start point coords    */
        RC_RASTERIZE_SWAP(x1, y1); /* Swap end point coords      */
        RC_RASTERIZE_SWAP(dx, dy); /* Swap lengths               */
        ahead = 3;                 /* Ahead is now down          */
        dir   = 1;                 /* Reverse orientation        */
    }

    /* Reverse x */
    if (x0 > x1) {
        RC_RASTERIZE_SWAP(x0, x1); /* Swap x coordinates         */
        RC_RASTERIZE_SWAP(y0, y1); /* Swap y coordindats         */
        step  = -1;                /* Decreasing x coordinate    */
        pos   = len - 1;           /* Start at the buffer end    */
        ahead = (ahead + 2) & 3;   /* Rotate ahead direction     */
    }

    /* Reverse y */
    if (y0 > y1) {
        dir = dir < 0 ? 1 : -1;    /* Flip orientation           */
    }

    /* Compute the side step chain code */
    side = (ahead + dir) & 3;

    /* Adjust chain codes to ASCII characters */
    ahead += '0';
    side  += '0';

    /* Rasterize the chain code */
    for (x = x0, err = -dx; x < x1; x++) {

        assert(pos >= 0 && pos < len);

        /* Update the accumulated error */
        err += dy;

        /* Check for side step */
        if (err >= 0) {
            err -= dx;         /* Decrement error */
            line[pos] = side;  /* Side step       */
            pos += step;       /* Update position */
        }

        /* Always one step straight ahead  */
        line[pos] = ahead;

        /* Update the buffer position */
        pos += step;
    }

    /* Add NUL termination */
    line[len] = '\0';

    return len;
}

/**
 *  Rasterize an 8-connectivity chain code.
 */
int
rc_rasterize_8conn(char *line, int x0, int y0, int x1, int y1)
{
    int dx  = abs(x1 - x0);    /* Absolute x diff            */
    int dy  = abs(y1 - y0);    /* Absolute y diff            */
    int len = MAX(dx, dy);     /* Chain code length          */

    int pos   = 0;             /* Buffer position            */
    int err   = 0;             /* Accumulated error          */
    int step  = 1;             /* Step in x = (+/-)1         */

    int ahead = 0;             /* Ahead direction chain code */
    int side  = 7;             /* Side direction chain code  */
    int dir   = -1;

    int x;

    /* Swap x and y */
    if (dy > dx) {
        RC_RASTERIZE_SWAP(x0, y0); /* Swap start point coords    */
        RC_RASTERIZE_SWAP(x1, y1); /* Swap end point coords      */
        RC_RASTERIZE_SWAP(dx, dy); /* Swap lengths               */
        ahead = 6;                 /* Ahead is now down          */
        dir   = 1;                 /* Reverse direction          */
    }

    /* Reverse x */
    if (x0 > x1) {
        RC_RASTERIZE_SWAP(x0, x1); /* Swap x coordinates         */
        RC_RASTERIZE_SWAP(y0, y1); /* Swap y coordindats         */
        step  = -1;                /* Decreasing x coordinate    */
        pos   = len - 1;           /* Start at the buffer end    */
        ahead = (ahead + 4) & 7;   /* Rotate ahead direction     */
    }

    /* Reverse y */
    if (y0 > y1) {
        dir = dir < 0 ? 1 : -1;         /* Reverse direction          */
    }

    /* Compute the side step chain code */
    side = (ahead + dir) & 7;

    /* Adjust chain codes to ASCII characters */
    ahead += '0';
    side  += '0';

    /* Rasterize the chain code */
    for (x = x0, err = -(dx + 1)/2; x < x1; x++) {
        assert(pos >= 0 && pos < len);

        /* Update the accumulated error */
        err += dy;

        /* Update the line chain code */
        if (err < 0) {
            line[pos] = ahead; /* Straight ahead  */
        }
        else {
            err -= dx;         /* Decrement error */
            line[pos] = side;  /* Side step       */
        }

        /* Update the buffer position */
        pos += step;
    }

    /* Add NUL termination */
    line[len] = '\0';

    return len;
}
