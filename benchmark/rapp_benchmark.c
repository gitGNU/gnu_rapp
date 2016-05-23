/*  Copyright (C) 2005-2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_benchmark.c
 *  @brief  RAPP benchmark application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "rapp.h"

/*
 * -------------------------------------------------------------
 *  Constants macros
 * -------------------------------------------------------------
 */

/**
 *  Standard MAX().
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 *  The name of the output data file.
 */
#define RAPP_BMARK_OUTFILE "benchmarkdata.py"

/**
 *  The vertical (left/right) buffer padding value in bytes.
 */
#define RAPP_BMARK_VPAD 2

/**
 *  The horizontal (top/bottom) buffer padding value in pixels.
 */
#define RAPP_BMARK_HPAD 16

/**
 *  The maximum number of rows in the rapp_gather_u8() test.
 */
#define RAPP_BMARK_ROWS 5

/**
 *  Platform specific defines
 */
#ifdef __CYGWIN__
#define RAPP_BMARK_SIGNUM SIGALRM
#define RAPP_BMARK_ITIMER ITIMER_REAL
#else
#define RAPP_BMARK_SIGNUM SIGPROF
#define RAPP_BMARK_ITIMER ITIMER_PROF
#endif

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  The test suite table entry macro.
 */
#define RAPP_BMARK_ENTRY(func, desc, exec, arg1, arg2) \
    {"rapp_" #func,                                    \
     (desc) ? desc : "", rapp_ ## func,                \
     rapp_bmark_exec_ ## exec,                         \
     {arg1, arg2}}


/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

/**
 *  The data object holding buffers etc.
 */
typedef struct rapp_bmark_data_st {
    uint8_t *dst;     /**< Destination buffer                        */
    uint8_t *set;     /**< Source buffer with all bits set           */
    uint8_t *pad;     /**< Source buffer, set and padded with zeros  */
    uint8_t *clear;   /**< Source buffer with all bits cleared       */
    uint8_t *checker; /**< Source buffer with checker bit pattern    */
    uint8_t *aux;     /**< Auxiliary buffer, for LUTs etc            */
    uint8_t *src[5];  /**< A table of pointers to the 5 src buffers  */
    int      dim_u8;  /**< 8-bit image buffer dimension              */
    int      dim_bin; /**< Binary image buffer dimension             */
    int      rot_u8;  /**< Rotated 8-bit image buffer dimension      */
    int      rot_bin; /**< Rotated binary image buffer dimension     */
    int      pad_u8;  /**< 8-bit buffer padding dimension increment  */
    int      pad_bin; /**< Binary buffer padding dimension increment */
    int      width;   /**< Image width in pixels                     */
    int      height;  /**< Image height in pixels                    */
    int      offset;  /**< Offset to pointer to free                 */
} rapp_bmark_data_t;

/**
 *  The test suite table object.
 */
typedef struct rapp_bmark_table_st {
    const char  *name;
    const char  *desc;
    int        (*func)();
    void       (*exec)(int (*func)(), const int*);
    int          args[2];
} rapp_bmark_table_t;


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rapp_bmark_setup(int width, int height);

static void
rapp_bmark_cleanup(void);

static void
rapp_bmark_sighandler(int signum);

static void
rapp_bmark_print(float value);

static void
rapp_bmark_exec_bin(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_p(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_bin(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_bin_off(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_bin_ip(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_bin_iip(int (*func)(), const int *args);

static void
rapp_bmark_exec_u8_bin(int (*func)(), const int *args);

static void
rapp_bmark_exec_bin_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_u8_p(int (*func)(), const int *args);

static void
rapp_bmark_exec_u8_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_u8_u8_p(int (*func)(), const int *args);

static void
rapp_bmark_exec_thresh_pixel(int (*func)(), const int *args);

static void
rapp_bmark_exec_expand(int (*func)(), const int *args);

static void
rapp_bmark_exec_contour(int (*func)(), const int *args);

static void
rapp_bmark_exec_rotate_bin(int (*func)(), const int *args);

static void
rapp_bmark_exec_rotate_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_cond_set_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_cond_copy_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_gather_u8(int (*func)(), const int *args);

static void
rapp_bmark_exec_gather_bin(int (*func)(), const int *args);

static void
rapp_bmark_exec_scatter(int (*func)(), const int *args);

static void
rapp_bmark_exec_scatter_bin(int (*func)(), const int *args);


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static volatile int rapp_bmark_done = 0;

static rapp_bmark_data_t rapp_bmark_data;

static const rapp_bmark_table_t rapp_bmark_suite[] = {
    /* rapp_bitblt_bin functions */
    RAPP_BMARK_ENTRY(bitblt_copy_bin, "aligned",      bin_bin_off, 0, 0),
    RAPP_BMARK_ENTRY(bitblt_copy_bin, "byte-aligned", bin_bin_off, 1, 0),
    RAPP_BMARK_ENTRY(bitblt_copy_bin, "misaligned",   bin_bin_off, 1, 3),
    RAPP_BMARK_ENTRY(bitblt_and_bin,  "aligned",      bin_bin_off, 0, 0),
    RAPP_BMARK_ENTRY(bitblt_and_bin,  "byte-aligned", bin_bin_off, 1, 0),
    RAPP_BMARK_ENTRY(bitblt_and_bin,  "misaligned",   bin_bin_off, 1, 3),
    RAPP_BMARK_ENTRY(bitblt_nand_bin, "aligned",      bin_bin_off, 0, 0),
    RAPP_BMARK_ENTRY(bitblt_nand_bin, "byte-aligned", bin_bin_off, 1, 0),
    RAPP_BMARK_ENTRY(bitblt_nand_bin, "misaligned",   bin_bin_off, 1, 3),
    /* rapp_pixop functions */
    RAPP_BMARK_ENTRY(pixop_set_u8,    NULL,  u8,    0, 0),
    RAPP_BMARK_ENTRY(pixop_not_u8,    NULL,  u8,    0, 0),
    RAPP_BMARK_ENTRY(pixop_flip_u8,   NULL,  u8,    0, 0),
    RAPP_BMARK_ENTRY(pixop_lut_u8,    NULL,  u8_p,  0, 0),
    RAPP_BMARK_ENTRY(pixop_abs_u8,    NULL,  u8,    0, 0),
    RAPP_BMARK_ENTRY(pixop_addc_u8,   NULL,  u8,    7, 0),
    RAPP_BMARK_ENTRY(pixop_lerpc_u8,  NULL,  u8,    7, 9),
    RAPP_BMARK_ENTRY(pixop_lerpnc_u8, NULL,  u8,    7, 9),
    RAPP_BMARK_ENTRY(pixop_copy_u8,   NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_add_u8,    NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_avg_u8,    NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_sub_u8,    NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_subh_u8,   NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_suba_u8,   NULL,  u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(pixop_lerp_u8,   NULL,  u8_u8, 7, 0),
    RAPP_BMARK_ENTRY(pixop_lerpn_u8,  NULL,  u8_u8, 7, 0),
    RAPP_BMARK_ENTRY(pixop_lerpi_u8,  NULL,  u8_u8, 7, 0),
    RAPP_BMARK_ENTRY(pixop_norm_u8,   NULL,  u8_u8, 0, 0),

    /* rapp_type functions */
    RAPP_BMARK_ENTRY(type_u8_to_bin,  NULL,  u8_bin, 0, 0),
    RAPP_BMARK_ENTRY(type_bin_to_u8,  NULL,  bin_u8, 0, 0),
    /* rapp_thresh functions */
    RAPP_BMARK_ENTRY(thresh_gt_u8,   NULL, u8_bin, 7, 0),
    RAPP_BMARK_ENTRY(thresh_lt_u8,   NULL, u8_bin, 7, 0),
    RAPP_BMARK_ENTRY(thresh_gtlt_u8, NULL, u8_bin, 7, 9),
    RAPP_BMARK_ENTRY(thresh_ltgt_u8, NULL, u8_bin, 7, 9),
    RAPP_BMARK_ENTRY(thresh_gt_pixel_u8,   NULL, thresh_pixel, 1, 0),
    RAPP_BMARK_ENTRY(thresh_lt_pixel_u8,   NULL, thresh_pixel, 1, 0),
    RAPP_BMARK_ENTRY(thresh_gtlt_pixel_u8, NULL, thresh_pixel, 2, 0),
    RAPP_BMARK_ENTRY(thresh_ltgt_pixel_u8, NULL, thresh_pixel, 2, 0),
    /* rapp_reduce functions */
    RAPP_BMARK_ENTRY(reduce_1x2_u8, NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x1_u8, NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x2_u8, NULL, u8_u8, 0, 0),
    /* rapp_reduce_bin functions */
    RAPP_BMARK_ENTRY(reduce_1x2_rk1_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_1x2_rk2_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x1_rk1_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x1_rk2_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x2_rk1_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x2_rk2_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x2_rk3_bin, NULL, bin_bin, 0, 0),
    RAPP_BMARK_ENTRY(reduce_2x2_rk4_bin, NULL, bin_bin, 0, 0),
    /* rapp_expand_bin functions */
    RAPP_BMARK_ENTRY(expand_1x2_bin, NULL, expand, 0, 0),
    RAPP_BMARK_ENTRY(expand_2x2_bin, NULL, expand, 0, 0),
    RAPP_BMARK_ENTRY(expand_2x2_bin, NULL, expand, 0, 0),
    /* rapp_rotate_u8 functions */
    RAPP_BMARK_ENTRY(rotate_cw_u8,  NULL, rotate_u8, 0, 0),
    RAPP_BMARK_ENTRY(rotate_ccw_u8, NULL, rotate_u8, 0, 0),
    /* rapp_rotate_bin functions */
    RAPP_BMARK_ENTRY(rotate_cw_bin,  "empty", rotate_bin, 2, 0),
    RAPP_BMARK_ENTRY(rotate_cw_bin,  "full",  rotate_bin, 0, 0),
    RAPP_BMARK_ENTRY(rotate_ccw_bin, "empty", rotate_bin, 2, 0),
    RAPP_BMARK_ENTRY(rotate_ccw_bin, "full",  rotate_bin, 0, 0),
    /* rapp_stat functions */
    RAPP_BMARK_ENTRY(stat_sum_bin, NULL, bin,     0, 0),
    RAPP_BMARK_ENTRY(stat_sum_u8,  NULL, u8,      0, 0),
    RAPP_BMARK_ENTRY(stat_sum2_u8, NULL, u8_p,    0, 0),
    RAPP_BMARK_ENTRY(stat_xsum_u8, NULL, u8_u8_p, 0, 0),
    RAPP_BMARK_ENTRY(stat_min_bin, NULL, bin,     0, 0),
    RAPP_BMARK_ENTRY(stat_max_bin, NULL, bin,     0, 0),
    RAPP_BMARK_ENTRY(stat_min_u8,  NULL, u8,      0, 0),
    RAPP_BMARK_ENTRY(stat_max_u8,  NULL, u8,      0, 0),
    /* rapp_moment_bin functions */
    RAPP_BMARK_ENTRY(moment_order1_bin, "empty",   bin_p, 2, 0),
    RAPP_BMARK_ENTRY(moment_order1_bin, "full",    bin_p, 0, 0),
    RAPP_BMARK_ENTRY(moment_order1_bin, "checker", bin_p, 3, 0),
    RAPP_BMARK_ENTRY(moment_order2_bin, "empty",   bin_p, 2, 0),
    RAPP_BMARK_ENTRY(moment_order2_bin, "full",    bin_p, 0, 0),
    RAPP_BMARK_ENTRY(moment_order2_bin, "checker", bin_p, 3, 0),
    /* rapp_conv functions */
    RAPP_BMARK_ENTRY(filter_diff_1x2_horz_u8,      NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_diff_1x2_horz_abs_u8,  NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_diff_2x1_vert_u8,      NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_diff_2x1_vert_abs_u8,  NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_diff_2x2_magn_u8,      NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_sobel_3x3_horz_u8,     NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_sobel_3x3_horz_abs_u8, NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_sobel_3x3_vert_u8,     NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_sobel_3x3_vert_abs_u8, NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_sobel_3x3_magn_u8,     NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_gauss_3x3_u8,          NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_laplace_3x3_u8,        NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_laplace_3x3_abs_u8,    NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_highpass_3x3_u8,       NULL, u8_u8, 0, 0),
    RAPP_BMARK_ENTRY(filter_highpass_3x3_abs_u8,   NULL, u8_u8, 0, 0),
    /* rapp_morph_bin functions */
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "2x2",   bin_bin_iip,  2,  2),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "3x3",   bin_bin_iip,  3,  3),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "5x5",   bin_bin_iip,  5,  5),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "7x7",   bin_bin_iip,  7,  7),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "15x15", bin_bin_iip, 15, 15),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "31x31", bin_bin_iip, 31, 31),
    RAPP_BMARK_ENTRY(morph_erode_rect_bin, "63x63", bin_bin_iip, 63, 63),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "3x3",   bin_bin_ip,   2,  0),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "5x5",   bin_bin_ip,   3,  0),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "7x7",   bin_bin_ip,   4,  0),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "15x15", bin_bin_ip,   8,  0),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "31x31", bin_bin_ip,  16,  0),
    RAPP_BMARK_ENTRY(morph_erode_diam_bin, "63x63", bin_bin_ip,  32,  0),
    RAPP_BMARK_ENTRY(morph_erode_oct_bin,  "5x5",   bin_bin_ip,   3,  0),
    RAPP_BMARK_ENTRY(morph_erode_oct_bin,  "7x7",   bin_bin_ip,   4,  0),
    RAPP_BMARK_ENTRY(morph_erode_oct_bin,  "15x15", bin_bin_ip,   8,  0),
    RAPP_BMARK_ENTRY(morph_erode_oct_bin,  "31x31", bin_bin_ip,  16,  0),
    RAPP_BMARK_ENTRY(morph_erode_oct_bin,  "63x63", bin_bin_ip,  32,  0),
    RAPP_BMARK_ENTRY(morph_erode_disc_bin, "7x7",   bin_bin_ip,   4,  0),
    RAPP_BMARK_ENTRY(morph_erode_disc_bin, "15x15", bin_bin_ip,   8,  0),
    RAPP_BMARK_ENTRY(morph_erode_disc_bin, "31x31", bin_bin_ip,  16,  0),
    RAPP_BMARK_ENTRY(morph_erode_disc_bin, "63x63", bin_bin_ip,  32,  0),
    /* rapp_fill functions */
    RAPP_BMARK_ENTRY(fill_4conn_bin, "full", bin_bin, 0,  0),
    RAPP_BMARK_ENTRY(fill_8conn_bin, "full", bin_bin, 0,  0),
    /* rapp_contour functions */
    RAPP_BMARK_ENTRY(contour_4conn_bin, "full", contour, 0,  0),
    RAPP_BMARK_ENTRY(contour_8conn_bin, "full", contour, 0,  0),
    /* rapp_cond functions */
    RAPP_BMARK_ENTRY(cond_set_u8,  "empty",   cond_set_u8,  2,  0),
    RAPP_BMARK_ENTRY(cond_set_u8,  "full",    cond_set_u8,  0,  0),
    RAPP_BMARK_ENTRY(cond_set_u8,  "checker", cond_set_u8,  3,  0),
    RAPP_BMARK_ENTRY(cond_copy_u8, "empty",   cond_copy_u8, 2,  0),
    RAPP_BMARK_ENTRY(cond_copy_u8, "full",    cond_copy_u8, 0,  0),
    RAPP_BMARK_ENTRY(cond_copy_u8, "checker", cond_copy_u8, 3,  0),
    /* rapp_gather functions */
    RAPP_BMARK_ENTRY(gather_u8, "empty, 1 row",    gather_u8, 2,  1),
    RAPP_BMARK_ENTRY(gather_u8, "full, 1 row",     gather_u8, 0,  1),
    RAPP_BMARK_ENTRY(gather_u8, "checker, 1 row",  gather_u8, 3,  1),
    RAPP_BMARK_ENTRY(gather_u8, "empty, 2 rows",   gather_u8, 2,  2),
    RAPP_BMARK_ENTRY(gather_u8, "full, 2 rows",    gather_u8, 0,  2),
    RAPP_BMARK_ENTRY(gather_u8, "checker, 2 rows", gather_u8, 3,  2),
    RAPP_BMARK_ENTRY(gather_u8, "empty, 3 rows",   gather_u8, 2,  3),
    RAPP_BMARK_ENTRY(gather_u8, "full, 3 rows",    gather_u8, 0,  3),
    RAPP_BMARK_ENTRY(gather_u8, "checker, 3 rows", gather_u8, 3,  3),
    RAPP_BMARK_ENTRY(gather_u8, "empty, 5 rows",   gather_u8, 2,  5),
    RAPP_BMARK_ENTRY(gather_u8, "full, 5 rows",    gather_u8, 0,  5),
    RAPP_BMARK_ENTRY(gather_u8, "checker, 5 rows", gather_u8, 3,  5),
    /* rapp_gather_bin functions */
    RAPP_BMARK_ENTRY(gather_bin, "empty",   gather_bin, 2,  0),
    RAPP_BMARK_ENTRY(gather_bin, "full",    gather_bin, 0,  0),
    RAPP_BMARK_ENTRY(gather_bin, "checker", gather_bin, 3,  0),
    /* rapp_scatter functions */
    RAPP_BMARK_ENTRY(scatter_u8, "empty",   scatter, 2,  0),
    RAPP_BMARK_ENTRY(scatter_u8, "full",    scatter, 0,  0),
    RAPP_BMARK_ENTRY(scatter_u8, "checker", scatter, 3,  0),
    /* rapp_scatter_bin functions */
    RAPP_BMARK_ENTRY(scatter_bin, "empty",   scatter_bin, 2,  0),
    RAPP_BMARK_ENTRY(scatter_bin, "full",    scatter_bin, 0,  0),
    RAPP_BMARK_ENTRY(scatter_bin, "checker", scatter_bin, 3,  0)
};


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int
main(int argc, char **argv)
{
    struct itimerval itm = {{0, 0}, {0, 0}};
    struct sigaction act;
    FILE            *outfile;
    int              width  = 256;
    int              height = 256;
    int              msecs  = 1000;
    int              ch, k;

    /* Get options */
    while ((ch = getopt(argc, argv, "w:h:m:")) != -1) {
        switch (ch) {
            case 'w':
                width = atoi(optarg);
                break;

            case 'h':
                height = atoi(optarg);
                break;

            case 'm':
                msecs = atoi(optarg);
                break;

            case '?':
                fprintf(stderr,
                        "Usage %s [options]\n"
                        "Options: -w <width>    Image width in pixels\n"
                        "         -h <height>   Image height in pixels\n"
                        "         -m <msecs>    Timer in milliseconds\n",
                        argv[0]);
                return EXIT_SUCCESS;
        }
    }

    if (msecs <= 0) {
        fprintf(stderr, "Invalid benchmark time %d ms\n", msecs);
        return EXIT_FAILURE;
    }

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Invalid image size %dx%d pixels\n", width, height);
        return EXIT_FAILURE;
    }

    /* Open the output data file */
    outfile = fopen(RAPP_BMARK_OUTFILE, "wb");
    if(!outfile) {
        fprintf(stderr, "Failed to open output file %s\n",
                RAPP_BMARK_OUTFILE);
        return EXIT_FAILURE;
    }

    /* Setup timer */
    itm.it_value.tv_sec  =  msecs/1000;
    itm.it_value.tv_usec = (msecs*1000) % 1000000;

    memset(&act, 0, sizeof act);
    act.sa_handler = &rapp_bmark_sighandler;
    sigaction(RAPP_BMARK_SIGNUM, &act, NULL);

    rapp_initialize();

    /* Initialize the data */
    rapp_bmark_setup(width, height);

    /* Print the header */
    printf("%-35s  %-20s%s\n", "function", "param", "pix/sec");
    printf("--------------------------------"
           "--------------------------------\n");

    /* Print data file header */
    fprintf(outfile, "build='%s'\n"
                     "size=[%d, %d]\n"
                     "data=[\n", rapp_info, width, height);

    /* Run the benchmark suite */
    for (k = 0; k < (int)(sizeof rapp_bmark_suite /
                          sizeof rapp_bmark_suite[0]); k++)
    {
        const rapp_bmark_table_t *entry = &rapp_bmark_suite[k];
        float                     cnt;
        int                       iter;

        /* Print formatting data */
        printf("%-35s  %-20s", entry->name, entry->desc);
        fflush(stdout);

        /* Run benchmark test */
        rapp_bmark_done = 0;
        setitimer(RAPP_BMARK_ITIMER, &itm, NULL);

        for (iter = 0; !rapp_bmark_done; iter++) {
            entry->exec(entry->func, entry->args);
        }

        cnt = (float)width*height*iter / (msecs/1000.0f);
        rapp_bmark_print(cnt);
        printf("\n");

        /* Print data file entry */
        fprintf(outfile, "['%s', '%s', %e],\n",
                entry->name, entry->desc, cnt);
    }
    fprintf(outfile, "]");

    /* Clean up */
    rapp_bmark_cleanup();
    fclose(outfile);

    rapp_terminate();

    return EXIT_SUCCESS;
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rapp_bmark_setup(int width, int height)
{
    int dim_u8  = rapp_align(width);
    int dim_bin = rapp_align((width + 7) / 8);
    int rot_u8  = rapp_align(height);
    int rot_bin = rapp_align((height + 7) / 8);
    int pad_u8  = rapp_align(RAPP_BMARK_VPAD);
    int pad_bin = rapp_align((RAPP_BMARK_VPAD + 7) / 8);
    int offset  = RAPP_BMARK_HPAD*dim_u8 + pad_u8;
    int size;

    /**
     *  We perform an extra alignment of "offset", since it adjusts
     *  pointers to types that might require alignment larger than
     *  RAPP_ALIGNMEMT, e.g. uintmax_t being 64 bits and requiring
     *  64-bit alignments on a target with 32-bit pointers and
     *  RC_ALIGNMENT 4, such as sparc64 with the 32-bit ABI.
     *  We assume that doubling the alignment is sufficient.
     */
    offset = 2*rapp_align(offset / 2 + 1);
    size = offset + 256 +
           RAPP_BMARK_ROWS*MAX(dim_u8*height + 2*RAPP_BMARK_HPAD,
                               rot_u8*width  + 2*RAPP_BMARK_HPAD);

    rapp_bmark_data.dst     = rapp_malloc(size, 0);
    rapp_bmark_data.set     = rapp_malloc(size, 0);
    rapp_bmark_data.pad     = rapp_malloc(size, 0);
    rapp_bmark_data.clear   = rapp_malloc(size, 0);
    rapp_bmark_data.checker = rapp_malloc(size, 0);
    rapp_bmark_data.aux     = rapp_malloc(size, 0);

    rapp_bmark_data.dim_u8  = dim_u8;
    rapp_bmark_data.dim_bin = dim_bin;
    rapp_bmark_data.rot_u8  = rot_u8;
    rapp_bmark_data.rot_bin = rot_bin;
    rapp_bmark_data.pad_u8  = 2*pad_u8;
    rapp_bmark_data.pad_bin = 2*pad_bin;
    rapp_bmark_data.width   = width;
    rapp_bmark_data.height  = height;
    rapp_bmark_data.offset  = offset;

    memset(rapp_bmark_data.dst,     0xff, size);
    memset(rapp_bmark_data.set,     0xff, size);
    memset(rapp_bmark_data.pad,     0xff, size);
    memset(rapp_bmark_data.clear,   0,    size);
    memset(rapp_bmark_data.checker, 0x55, size);
    memset(rapp_bmark_data.aux,     0xff, size);

    rapp_bmark_data.dst     += offset;
    rapp_bmark_data.set     += offset;
    rapp_bmark_data.pad     += offset;
    rapp_bmark_data.clear   += offset;
    rapp_bmark_data.checker += offset;
    rapp_bmark_data.aux     += offset;

    rapp_bmark_data.src[0] = rapp_bmark_data.set;
    rapp_bmark_data.src[1] = rapp_bmark_data.pad;
    rapp_bmark_data.src[2] = rapp_bmark_data.clear;
    rapp_bmark_data.src[3] = rapp_bmark_data.checker;
    rapp_bmark_data.src[4] = rapp_bmark_data.aux;

    // rapp_pixel_set_bin(rapp_bmark_data.set, 0, 0, 0, 0, 0);
    rapp_pad_const_bin(rapp_bmark_data.pad, dim_bin + 2*pad_bin,
                       0, width, height, 1, 0);
}

static void
rapp_bmark_cleanup(void)
{
    rapp_free(&rapp_bmark_data.dst    [-rapp_bmark_data.offset]);
    rapp_free(&rapp_bmark_data.set    [-rapp_bmark_data.offset]);
    rapp_free(&rapp_bmark_data.pad    [-rapp_bmark_data.offset]);
    rapp_free(&rapp_bmark_data.clear  [-rapp_bmark_data.offset]);
    rapp_free(&rapp_bmark_data.checker[-rapp_bmark_data.offset]);
    rapp_free(&rapp_bmark_data.aux    [-rapp_bmark_data.offset]);
}

static void
rapp_bmark_sighandler(int signum)
{
    (void)signum;
    rapp_bmark_done = 1;
}

static void
rapp_bmark_print(float value)
{
    float aval = fabs(value);
    char  suff = ' ';

    if (aval < 1.0e3f) {
        suff = ' ';
    }
    else if (aval < 1.0e6f) {
        value *= 1.0e-3f;
        suff   = 'k';
    }
    else if (aval < 1.0e9f) {
        value *= 1.0e-6f;
        suff   = 'M';
    }
    else if (aval < 1.0e12f) {
        value *= 1.0e-9f;
        suff   = 'G';
    }
    else {
        value *= 1.0e-12f;
        suff   = 'T';
    }

    printf("%6.2f%c", value, suff);
}

static void
rapp_bmark_exec_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst,   data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_bin_p(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    int idx = args[0];
    (*func)(data->src[idx], data->dim_bin,
            data->width,    data->height,
            data->aux);
}

static void
rapp_bmark_exec_bin_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,   data->dim_bin,
            data->set,   data->dim_bin,
            data->width, data->height,
            args[0], args[1]);
}

static void
rapp_bmark_exec_bin_bin_off(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst, data->dim_bin, 0, &data->set[args[0]],
            data->dim_bin + (args[0] || args[1])*rapp_alignment,
            args[1], data->width, data->height);
}

static void
rapp_bmark_exec_bin_bin_ip(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst, data->dim_bin,
            data->set, data->dim_bin + data->pad_bin,
            data->width, data->height,
            args[0], data->aux);
}

static void
rapp_bmark_exec_bin_bin_iip(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst, data->dim_bin,
            data->set, data->dim_bin  + data->pad_bin,
            data->width, data->height,
            args[0], args[1], data->aux);
}

static void
rapp_bmark_exec_bin_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst,   data->dim_u8,
            data->set,   data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_u8_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,   data->dim_bin,
            data->set,   data->dim_u8,
            data->width, data->height, args[0]);
}

static void
rapp_bmark_exec_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,   data->dim_u8,
            data->width, data->height,
            (int)args[0], (int)args[1]);
}

static void
rapp_bmark_exec_u8_p(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst, data->dim_u8, data->width, data->height, data->aux);
}

static void
rapp_bmark_exec_u8_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst, data->dim_u8,
            data->set, data->dim_u8 + data->pad_u8,
            data->width, data->height,
            args[0], args[1]);
}

static void
rapp_bmark_exec_u8_u8_p(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst, data->dim_u8,
            data->set, data->dim_u8,
            data->width, data->height,
            data->aux);
}

static void
rapp_bmark_exec_thresh_pixel(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;

    const int num_thresholds = args[0];
    if (num_thresholds == 2) {
        /* The speed is not dependent of the content or calculation results
         * so the aux buffer is reused for both high and low thresholds.
         * This minimize changes of the entire benchmark test,
           i.e. only require a single aux buffer. */
        (*func)(data->dst, data->dim_bin,
                data->set, data->dim_u8,
                data->aux, data->dim_u8,
                data->aux, data->dim_u8,
                data->width, data->height);
    }
    else {
        (*func)(data->dst, data->dim_bin,
                data->set, data->dim_u8,
                data->aux, data->dim_u8,
                data->width, data->height);
    }
}

static void
rapp_bmark_exec_expand(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst, 2*data->dim_bin,
            data->set, data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_contour(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->aux, NULL, 0,
            data->pad, data->dim_bin + data->pad_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_rotate_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,          data->rot_bin,
            data->src[args[0]], data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_rotate_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (void)args;
    (*func)(data->dst, data->rot_u8,
            data->set, data->dim_u8,
            data->width, data->height);
}

static void
rapp_bmark_exec_cond_set_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    int                      idx  = args[0];
    (*func)(data->dst, data->dim_u8,
            data->src[idx], data->dim_bin,
            data->width, data->height, 0);
}

static void
rapp_bmark_exec_cond_copy_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    int                      idx  = args[0];
    (*func)(data->dst, data->dim_u8,
            data->set, data->dim_u8,
            data->src[idx], data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_gather_u8(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    int                      idx  = args[0];
    int                      rows = args[1];
    (*func)(data->dst,      data->height*data->dim_u8,
            data->clear,    data->dim_u8,
            data->src[idx], data->dim_bin,
            data->width, data->height, rows);
}

static void
rapp_bmark_exec_gather_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,
            data->clear, data->dim_bin,
            data->src[args[0]], data->dim_bin,
            data->width, data->height);
}

static void
rapp_bmark_exec_scatter(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,          data->dim_u8,
            data->src[args[0]], data->dim_bin,
            data->clear, data->width, data->height);
}

static void
rapp_bmark_exec_scatter_bin(int (*func)(), const int *args)
{
    const rapp_bmark_data_t *data = &rapp_bmark_data;
    (*func)(data->dst,          data->dim_bin,
            data->src[args[0]], data->dim_bin,
            data->clear, data->width, data->height);
}
