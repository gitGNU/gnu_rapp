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
 *  @file   rapp_ref_contour.c
 *  @brief  RAPP contour chain code generation, reference implementation.
 */

#include <string.h>           /* memcmp()                 */
#include <assert.h>           /* assert()                 */
#include "rapp.h"             /* RAPP API                 */
#include "rapp_ref_crop.h"    /* rapp_ref_crop_seek_bin() */
#include "rapp_ref_contour.h" /* Contour API              */


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_ref_contour_driver(unsigned origin[2], char *contour, int len,
                        const uint8_t *buf, int dim,
                        int width, int height,
                        int (*find)(), int (*loop)());

static int
rapp_ref_4conn_find_dir(const uint8_t *buf, const uint8_t *map,
                        int dim, int width, int height,
                        int xpos, int ypos, int cw);

static int
rapp_ref_8conn_find_dir(const uint8_t *buf, const uint8_t *map,
                        int dim, int width, int height,
                        int xpos, int ypos, int cw);

static int
rapp_ref_4conn_loop(char *contour, int len, uint8_t *buf, uint8_t *map,
                    int dim, int width, int height,
                    int xpos, int ypos, int dir);

static int
rapp_ref_8conn_loop(char *contour, int len, uint8_t *buf, uint8_t *map,
                    int dim, int width, int height,
                    int xpos, int ypos, int dir);

static int
rapp_ref_4conn_code(const int conn[4], int code);


static int
rapp_ref_8conn_code(const int conn[8], int code);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
rapp_ref_contour_4conn_bin(unsigned origin[2], char *contour, int len,
                           const uint8_t *buf, int dim,
                           int width, int height)
{
    return rapp_ref_contour_driver(origin, contour, len,
                                   buf, dim, width, height,
                                   &rapp_ref_4conn_find_dir,
                                   &rapp_ref_4conn_loop);
}


int
rapp_ref_contour_8conn_bin(unsigned origin[2], char *contour, int len,
                           const uint8_t *buf, int dim,
                           int width, int height)
{
    return rapp_ref_contour_driver(origin, contour, len, buf, dim,
                                   width, height,
                                   &rapp_ref_8conn_find_dir,
                                   &rapp_ref_8conn_loop);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static int
rapp_ref_contour_driver(unsigned origin[2], char *contour, int len,
                        const uint8_t *buf, int dim,
                        int width, int height,
                        int (*find)(), int (*loop)())
{
    uint8_t *map;
    int      idx = 0;
    int      xpos, ypos, code;

    /* Set up the 'visited' map buffer */
    map = malloc(height*dim);
    assert(map);
    memset(map, 0, height*dim);

    /* Find the start position */
    rapp_ref_crop_seek_bin(buf, dim, width, height, origin);
    xpos = origin[0];
    ypos = origin[1];

    /* Find the first loop start direction */
    code = (*find)(buf, map, dim, width, height, xpos, ypos, 1);
    if (code >= 0) {
        /* Trace the first loop */
        idx = (*loop)(contour, len, buf, map, dim,
                      width, height, xpos, ypos, code);

        /* Find the second loop start direction */
        code = (*find)(buf, map, dim, width, height, xpos, ypos, 0);
        if (code >= 0) {
            /* Trace the second loop */
            idx += (*loop)(&contour[idx], len - idx, buf, map, dim,
                           width, height, xpos, ypos, code);
        }
    }

    /* Add NUL termination */
    if (idx < len) {
        contour[idx] = '\0';
    }

    /* Release the map buffer */
    free(map);

    return idx;
}

static int
rapp_ref_4conn_find_dir(const uint8_t *buf, const uint8_t *map,
                        int dim, int width, int height,
                        int xpos, int ypos, int cw)
{
    const int dx[2] = {1, 0};
    const int dy[2] = {0, 1};
    const int ck[2] = {0, 3};
    int       k;

    for (k = 0; k < 2; k++) {
        int p = cw ? k : 1 - k;
        int x = xpos + dx[p];
        int y = ypos + dy[p];

        if (x >= 0 && x < width &&
            y >= 0 && y < height)
        {
            if (rapp_pixel_get_bin(map, dim, 0, x, y)) {
                /* Visited */
                return -1;
            }
            if (rapp_pixel_get_bin(buf, dim, 0, x, y)) {
                /* Found next start pixel */
                return ck[p];
            }
        }
    }

    return -1;
}

static int
rapp_ref_8conn_find_dir(const uint8_t *buf, const uint8_t *map,
                        int dim, int width, int height,
                        int xpos, int ypos, int cw)
{
    const int dx[4] = {1, 1, 0, -1};
    const int dy[4] = {0, 1, 1,  1};
    const int ck[4] = {0, 7, 6,  5};
    int       k;

    for (k = 0; k < 4; k++) {
        int p = cw ? k : 3 - k;
        int x = xpos + dx[p];
        int y = ypos + dy[p];

        if (x >= 0 && x < width &&
            y >= 0 && y < height)
        {
            if (rapp_pixel_get_bin(map, dim, 0, x, y)) {
                /* Visited */
                return -1;
            }
            if (rapp_pixel_get_bin(buf, dim, 0, x, y)) {
                /* Found next start pixel */
                return ck[p];
            }
        }
    }

    return -1;
}

static int
rapp_ref_4conn_loop(char *contour, int len, uint8_t *buf, uint8_t *map,
                    int dim, int width, int height,
                    int xpos, int ypos, int dir)
{
    const int dx[4] = {1 , 0, -1, 0};
    const int dy[4] = {0, -1,  0, 1};

    int code = dir;
    int x    = xpos + dx[dir];
    int y    = ypos + dy[dir];
    int i    = 1;

    /* Mark the first pixel */
    rapp_pixel_set_bin(map, dim, 0, x, y, 1);

    /* Save the first chain code */
    if (len > 0) {
        contour[0] = '0' + dir;
    }

    while (x != xpos || y != ypos) {
        int conn[4] = {0};
        int k;

        /* Set the neighborhood connectivity map */
        for (k = 0; k < 4; k++) {
            int xx = x + dx[k];
            int yy = y + dy[k];

            if (xx >= 0 && xx < width &&
                yy >= 0 && yy < height)
            {
                conn[k] = rapp_pixel_get_bin(buf, dim, 0, xx, yy);
            }
        }

        /* Compute the new chain code */
        code = rapp_ref_4conn_code(conn, code);

        /* Update position */
        x += dx[code];
        y += dy[code];

        /* Mark this pixel as visited */
        rapp_pixel_set_bin(map, dim, 0, x, y, 1);

        /* Save the chain code */
        if (i < len) {
            contour[i] = '0' + code;
        }

        /* One more chain code */
        i++;
    }

    return i;
}

static int
rapp_ref_8conn_loop(char *contour, int len, uint8_t *buf, uint8_t *map,
                    int dim, int width, int height,
                    int xpos, int ypos, int dir)
{
    const int dx[8] = {1 , 1,  0, -1, -1, -1, 0, 1};
    const int dy[8] = {0, -1, -1, -1,  0,  1, 1, 1};

    int code = dir;
    int x    = xpos + dx[dir];
    int y    = ypos + dy[dir];
    int i    = 1;

    /* Mark the first pixel */
    rapp_pixel_set_bin(map, dim, 0, x, y, 1);

    /* Save the first chain code */
    if (len > 0) {
        contour[0] = '0' + dir;
    }

    while (x != xpos || y != ypos) {
        int conn[8] = {0};
        int k;

        /* Set the neighborhood connectivity map */
        for (k = 0; k < 8; k++) {
            int xx = x + dx[k];
            int yy = y + dy[k];

            if (xx >= 0 && xx < width &&
                yy >= 0 && yy < height)
            {
                conn[k] = rapp_pixel_get_bin(buf, dim, 0, xx, yy);
            }
        }

        /* Compute the new chain code */
        code = rapp_ref_8conn_code(conn, code);

        /* Update position */
        x += dx[code];
        y += dy[code];

        /* Mark this pixel as visited */
        rapp_pixel_set_bin(map, dim, 0, x, y, 1);

        /* Save the chain code */
        if (i < len) {
            contour[i] = '0' + code;
        }

        /* One more chain code */
        i++;
    }

    return i;
}

static int
rapp_ref_4conn_code(const int conn[4], int code)
{
    const int cand[4][4] =
        {{1, 0, 3, 2},
         {2, 1, 0, 3},
         {3, 2, 1, 0},
         {0, 3, 2, 1}};

    int k;

    for (k = 0; k < 4; k++) {
        int next = cand[code][k];
        if (conn[next]) {
            return next;
        }
    }

    assert(0);
    return -1;
}

static int
rapp_ref_8conn_code(const int conn[8], int code)
{
    const int cand[8][7] =
        {{1, 0, 7, 6, 5, 4, 5},
         {3, 2, 1, 0, 7, 6, 5},
         {3, 2, 1, 0, 7, 6, 7},
         {5, 4, 3, 2, 1, 0, 7},
         {5, 4, 3, 2, 1, 0, 1},
         {7, 6, 5, 4, 3, 2, 1},
         {7, 6, 5, 4, 3, 2, 3},
         {1, 0, 7, 6, 5, 4, 3}};

    int k;

    for (k = 0; k < 7; k++) {
        int next = cand[code][k];
        if (conn[next]) {
            return next;
        }
    }

    assert(0);
    return -1;
}
