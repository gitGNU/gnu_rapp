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
 *  @file   rc_benchmark.c
 *  @brief  RAPP Compute layer benchmark application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/time.h>

/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  The buffer padding value in pixels.
 */
#define RC_BMARK_PADDING 16

/**
 *  Platform specific defines
 */
#ifdef __CYGWIN__
#define RC_BMARK_SIGNUM SIGALRM
#define RC_BMARK_ITIMER ITIMER_REAL
#else
#define RC_BMARK_SIGNUM SIGPROF
#define RC_BMARK_ITIMER ITIMER_PROF
#endif

/**
 *  Note that __GNUC__ is defined by more than gcc. Tests for such
 *  compilers go here, forming an #elif chain with the __GNUC__ test
 *  last.
 */
#ifdef __GNUC__
/**
 *  Do *not* include __GNUC_PATCHLEVEL__ here. It's not defined for
 *  old-enough versions and it's not (supposed to be) significant for
 *  tuning.
 */
#define RC_COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
/**
 *  Append tests for non-__GNUC__ compilers here. The value 0 is a
 *  fallback. Mask off unimportant parts, produce an integer with
 *  trivial ordering.
 */
#define RC_COMPILER_VERSION 0
#endif

/*
 * -------------------------------------------------------------
 *  Macros
 * -------------------------------------------------------------
 */

/**
 *  Standard MAX.
 */
#undef  MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 *  The test suite table entry macro.
 */
#define RC_BMARK_ENTRY(func, exec, arg1, arg2) \
    {#func, rc_bmark_exec_ ## exec, {arg1, arg2}}


/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

/**
 *  The data object holding buffers etc.
 */
typedef struct rc_bmark_data_st {
    uint8_t *dst;     /* Destination buffer large enough for all images */
    uint8_t *src;     /* Source buffer large enough for all images      */
    uint8_t *aux;     /* Auxiliary buffer large enough for all images  */
    uint8_t *aux2;    /* Second similar auxiliary buffer */
    uint8_t *map;     /* Buffer for binary mapping.                     */
    int      dim_bin; /* Binary row dimension, with padding             */
    int      dim_u8;  /* 8-bit row dimension, with padding              */
    int      rot_u8;  /* 8-bit rotatated row dimension, no padding      */
    int      width;   /* Test image width in pixels                     */
    int      height;  /* Test image height in pixels                    */
    int      offset;  /* Offset to allocated pointer                    */
    void*   (*alloc)(size_t);  /* Library memory allocator              */
    void    (*release)(void*); /* Library memory release                */
} rc_bmark_data_t;

/**
 *  The test suite table object.
 */
typedef struct rc_bmark_table_st {
    const char  *name;
    void       (*exec)(int (*)(), const int*);
    int          args[2];
} rc_bmark_table_t;


/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static void
rc_bmark_setup(void *lib, int width, int height);

static void
rc_bmark_cleanup(void);

static void
rc_bmark_sighandler(int signum);

static void
rc_bmark_print(float value);

static void
rc_bmark_exec_bin(int (*func)(), const int *args);

static void
rc_bmark_exec_p_bin(int (*func)(), const int *args);

static void
rc_bmark_exec_bin_bin(int (*func)(), const int *args);

static void
rc_bmark_exec_bin_bin_m(int (*func)(), const int *args);

static void
rc_bmark_exec_bin_u8(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_bin(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_bin_c(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_bin_u8_c(int (*func)(), const int *args);

static void
rc_bmark_exec_u8(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_p(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_u8(int (*func)(), const int *args);

static void
rc_bmark_exec_u8_u8_p(int (*func)(), const int *args);

static void
rc_bmark_exec_rotate(int (*func)(), const int *args);

static void
rc_bmark_exec_thresh_pixel(int (*func)(), const int *args);


/*
 * -------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------
 */

static volatile int rc_bmark_done = 0;

static rc_bmark_data_t rc_bmark_data;

static const rc_bmark_table_t rc_bmark_suite[] = {
  /**
   *  This file is also parsed using sed so entries must contain the
   *  exact same prefix: "RC_BMARK_ENTRY(rc_" and no space before the
   *  first comma.
   */
    /* Word-misaligned bitblt */
    RC_BMARK_ENTRY(rc_bitblt_wm_copy_bin,                 bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_not_bin,                  bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_and_bin,                  bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_or_bin,                   bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_xor_bin,                  bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_nand_bin,                 bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_nor_bin,                  bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_xnor_bin,                 bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_andn_bin,                 bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_orn_bin,                  bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_nandn_bin,                bin_bin_m, 3, 0),
    RC_BMARK_ENTRY(rc_bitblt_wm_norn_bin,                 bin_bin_m, 3, 0),
    /* Word-aligned bitblt */
    RC_BMARK_ENTRY(rc_bitblt_wa_copy_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_not_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_and_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_or_bin,                   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_xor_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_nand_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_nor_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_xnor_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_andn_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_orn_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_nandn_bin,                bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_wa_norn_bin,                 bin_bin,   0, 0),
    /* Vector-misaligned bitblt */
    RC_BMARK_ENTRY(rc_bitblt_vm_copy_bin,                 bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_not_bin,                  bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_and_bin,                  bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_or_bin,                   bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_xor_bin,                  bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_nand_bin,                 bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_nor_bin,                  bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_xnor_bin,                 bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_andn_bin,                 bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_orn_bin,                  bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_nandn_bin,                bin_bin_m, 0, 0),
    RC_BMARK_ENTRY(rc_bitblt_vm_norn_bin,                 bin_bin_m, 0, 0),
    /* Vector-aligned bitblt */
    RC_BMARK_ENTRY(rc_bitblt_va_copy_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_not_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_and_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_or_bin,                   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_xor_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_nand_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_nor_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_xnor_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_andn_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_orn_bin,                  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_nandn_bin,                bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_bitblt_va_norn_bin,                 bin_bin,   0, 0),
    /* Pixelwise operations */
    RC_BMARK_ENTRY(rc_pixop_set_u8,                       u8,        0, 0),
    RC_BMARK_ENTRY(rc_pixop_not_u8,                       u8,        0, 0),
    RC_BMARK_ENTRY(rc_pixop_flip_u8,                      u8,        0, 0),
    RC_BMARK_ENTRY(rc_pixop_lut_u8,                       u8_p,      0, 0),
    RC_BMARK_ENTRY(rc_pixop_abs_u8,                       u8,        0, 0),
    RC_BMARK_ENTRY(rc_pixop_addc_u8,                      u8,        7, 0),
    RC_BMARK_ENTRY(rc_pixop_lerpc_u8,                     u8,       12, 0x80),
    RC_BMARK_ENTRY(rc_pixop_lerpnc_u8,                    u8,       12, 0x80),
    RC_BMARK_ENTRY(rc_pixop_add_u8,                       u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_pixop_avg_u8,                       u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_pixop_sub_u8,                       u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_pixop_subh_u8,                      u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_pixop_suba_u8,                      u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_pixop_lerp_u8,                      u8_u8,    12, 0),
    RC_BMARK_ENTRY(rc_pixop_lerpn_u8,                     u8_u8,    12, 0),
    RC_BMARK_ENTRY(rc_pixop_lerpi_u8,                     u8_u8,    12, 0),
    RC_BMARK_ENTRY(rc_pixop_norm_u8,                      u8_u8,     0, 0),
    /* Type conversions */
    RC_BMARK_ENTRY(rc_type_u8_to_bin,                     u8_bin,    0, 0),
    RC_BMARK_ENTRY(rc_type_bin_to_u8,                     bin_u8,    0, 0),
    /* Thresholding */
    RC_BMARK_ENTRY(rc_thresh_gt_u8,                       u8_bin,    0, 0),
    RC_BMARK_ENTRY(rc_thresh_lt_u8,                       u8_bin,    0, 0),
    RC_BMARK_ENTRY(rc_thresh_gtlt_u8,                     u8_bin,    0, 0),
    RC_BMARK_ENTRY(rc_thresh_ltgt_u8,                     u8_bin,    0, 0),
    RC_BMARK_ENTRY(rc_thresh_gt_pixel_u8,                 thresh_pixel, 1, 0),
    RC_BMARK_ENTRY(rc_thresh_lt_pixel_u8,                 thresh_pixel, 1, 0),
    RC_BMARK_ENTRY(rc_thresh_gtlt_pixel_u8,               thresh_pixel, 2, 0),
    RC_BMARK_ENTRY(rc_thresh_ltgt_pixel_u8,               thresh_pixel, 2, 0),
    /* Statistics */
    RC_BMARK_ENTRY(rc_stat_sum_bin,                       bin,       0, 0),
    RC_BMARK_ENTRY(rc_stat_sum_u8,                        u8,        0, 0),
    RC_BMARK_ENTRY(rc_stat_sum2_u8,                       u8_p,      0, 0),
    RC_BMARK_ENTRY(rc_stat_xsum_u8,                       u8_u8_p,   0, 0),
    RC_BMARK_ENTRY(rc_stat_min_bin,                       bin,       0, 0),
    RC_BMARK_ENTRY(rc_stat_max_bin,                       bin,       0, 0),
    RC_BMARK_ENTRY(rc_stat_min_u8,                        u8,        0, 0),
    RC_BMARK_ENTRY(rc_stat_max_u8,                        u8,        0, 0),
    /* 8-bit 2x reductions */
    RC_BMARK_ENTRY(rc_reduce_1x2_u8,                      u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x1_u8,                      u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x2_u8,                      u8_u8,     0, 0),
    /* Binary 2x reductions */
    RC_BMARK_ENTRY(rc_reduce_1x2_rk1_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_1x2_rk2_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x1_rk1_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x1_rk2_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x2_rk1_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x2_rk2_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x2_rk3_bin,                 bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_reduce_2x2_rk4_bin,                 bin_bin,   0, 0),
    /* Binary 2x expansions */
    RC_BMARK_ENTRY(rc_expand_1x2_bin,                     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_expand_2x1_bin,                     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_expand_2x2_bin,                     bin_bin,   0, 0),
    /* 8-bit rotation */
    RC_BMARK_ENTRY(rc_rotate_cw_u8,                       rotate,    0, 0),
    RC_BMARK_ENTRY(rc_rotate_ccw_u8,                      rotate,    0, 0),
    /* Fixed-filter convolutions */
    RC_BMARK_ENTRY(rc_filter_diff_1x2_horz_u8,            u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_diff_1x2_horz_abs_u8,        u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_diff_2x1_vert_u8,            u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_diff_2x1_vert_abs_u8,        u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_diff_2x2_magn_u8,            u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_sobel_3x3_horz_u8,           u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_sobel_3x3_horz_abs_u8,       u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_sobel_3x3_vert_u8,           u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_sobel_3x3_vert_abs_u8,       u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_sobel_3x3_magn_u8,           u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_gauss_3x3_u8,                u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_laplace_3x3_u8,              u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_laplace_3x3_abs_u8,          u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_highpass_3x3_u8,             u8_u8,     0, 0),
    RC_BMARK_ENTRY(rc_filter_highpass_3x3_abs_u8,         u8_u8,     0, 0),
    /* Binary morphology */
    RC_BMARK_ENTRY(rc_morph_erode_line_1x2_bin,           bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x2_bin,          bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x3_bin,           bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x3_bin,          bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x3_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x3_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x5_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x5_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x7_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x7_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x9_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x9_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x13_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x13_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x15_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x15_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x17_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x17_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x25_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x25_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x29_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x29_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_1x31_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_1x31_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_2x1_bin,           bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_2x1_bin,          bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_3x1_bin,           bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_3x1_bin,          bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_3x1_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_3x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_5x1_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_5x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_7x1_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_7x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_9x1_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_9x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_13x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_13x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_15x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_15x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_17x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_17x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_25x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_25x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_29x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_29x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_line_31x1_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_line_31x1_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_square_2x2_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_square_2x2_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_square_3x3_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_square_3x3_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_square_3x3_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_square_3x3_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_3x3_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_3x3_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_3x3_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_3x3_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_5x5_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_5x5_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_7x7_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_7x7_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_9x9_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_9x9_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_13x13_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_13x13_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_15x15_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_15x15_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_17x17_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_17x17_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_25x25_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_25x25_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_29x29_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_29x29_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_diamond_31x31_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_diamond_31x31_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_5x5_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_5x5_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_7x7_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_7x7_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_9x9_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_9x9_p_bin,     bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_13x13_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_13x13_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_15x15_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_15x15_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_17x17_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_17x17_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_25x25_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_25x25_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_29x29_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_29x29_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_octagon_31x31_p_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_octagon_31x31_p_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_7x7_bin,           bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_7x7_bin,          bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_7x7_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_7x7_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_9x9_p_bin,         bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_9x9_p_bin,        bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_11x11_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_11x11_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_13x13_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_13x13_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_15x15_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_15x15_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_17x17_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_17x17_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_19x19_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_19x19_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_erode_disc_25x25_p_bin,       bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_dilate_disc_25x25_p_bin,      bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c48_r0_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c48_r90_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c48_r180_bin, bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c48_r270_bin, bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c4_r45_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c4_r135_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c4_r225_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c4_r315_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c8_r45_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c8_r135_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c8_r225_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_l_3x3_c8_r315_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c4_r0_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c4_r90_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c4_r180_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c4_r270_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r0_bin,    bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r90_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r180_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r270_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r45_bin,   bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r135_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r225_bin,  bin_bin,   0, 0),
    RC_BMARK_ENTRY(rc_morph_hmt_golay_e_3x3_c8_r315_bin,  bin_bin,   0, 0),
    /* Binary logical margins */
    RC_BMARK_ENTRY(rc_margin_horz_bin,                    p_bin,     0, 0),
    RC_BMARK_ENTRY(rc_margin_vert_bin,                    p_bin,        0, 0),
    /* Conditional operations */
    RC_BMARK_ENTRY(rc_cond_set_u8,                        u8_bin_c,     1, 0),
    RC_BMARK_ENTRY(rc_cond_addc_u8,                       u8_bin_c,     1, 0),
    RC_BMARK_ENTRY(rc_cond_subc_u8,                       u8_bin_c,     1, 0),
    RC_BMARK_ENTRY(rc_cond_copy_u8,                       u8_bin_u8_c,  0, 0),
    RC_BMARK_ENTRY(rc_cond_add_u8,                        u8_bin_u8_c,  0, 0)
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
    void            *lib;
    const char      *path   = "librappcompute.so";
    int              python = 0;
    int              width  = 256;
    int              height = 256;
    int              msecs  = 1000;
    int              ch, k;

    /* Get options */
    while ((ch = getopt(argc, argv, "l:w:h:m:pc")) != -1) {
        switch (ch) {
            case 'l':
                path = optarg;
                break;

            case 'w':
                width = atoi(optarg);
                break;

            case 'h':
                height = atoi(optarg);
                break;

            case 'm':
                msecs = atoi(optarg);
                break;

            case 'p':
                python = 1;
                break;

            case 'c':
                printf("%d\n", RC_COMPILER_VERSION);
                return EXIT_SUCCESS;
                break;

            case '?':
                fprintf(stderr,
                        "Usage %s [options]\n"
                        "Options: -l <library>  Path to RAPP Compute library\n"
                        "         -w <width>    Image width in pixels\n"
                        "         -h <height>   Image height in pixels\n"
                        "         -m <msecs>    Timer in milliseconds\n"
                        "         -c            Output compiler version\n"
                        "                       (0 if unknown)\n"
                        "         -p            Output data in Python format",
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

    /* Open the library */
    /**
     *  Using RTLD_NOW, not so much to avoid having dynamic name lookup
     *  affecting run-time, but rather to catch calls to tuned functions
     *  that don't have a separate rc_X__internal reference function,
     *  though we could use RTLD_LAZY for releases if need be.  See
     *  rc_stat_max_bin__internal.  Libraries dlopened with RTLD_NOW
     *  fail already at the dlopen if there are any undefined references.
     */
    lib = dlopen(path, RTLD_NOW);
    if (!lib) {
        fprintf(stderr, "\n%s: *** Could not load library \"%s\":\n %s\n",
                argv[0] == NULL ? __FILE__ : argv[0], path, dlerror());
        return EXIT_FAILURE;
    }

    /* Setup the data object */
    rc_bmark_setup(lib, width, height);

    /* Setup timer */
    itm.it_value.tv_sec  =  msecs/1000;
    itm.it_value.tv_usec = (msecs*1000) % 1000000;
    memset(&act, 0, sizeof act);
    act.sa_handler = &rc_bmark_sighandler;
    sigaction(RC_BMARK_SIGNUM, &act, NULL);

    if (python) {
        printf("[");
    }

    /* Run the benchmark suite */
    for (k = 0; k < (int)(sizeof rc_bmark_suite /
                          sizeof rc_bmark_suite[0]); k++)
    {
        const rc_bmark_table_t *entry   = &rc_bmark_suite[k];
        int                   (*func)() = dlsym(lib, entry->name);

        /* Print formatting data */
        if (python && k > 0) {
            printf(",\n ");
        }
        if (python) {
            printf("['%s', ", entry->name);
        }
        else {
            printf("%-50s  ", entry->name);
        }
        fflush(stdout);

        /* Run benchmark test */
        if (func) {
            float cnt;
            int   iter;

            rc_bmark_done = 0;
            setitimer(RC_BMARK_ITIMER, &itm, NULL);

            for (iter = 0; !rc_bmark_done; iter++) {
                entry->exec(func, entry->args);
            }

            cnt = (float)width*height*iter / (msecs/1000.0f);

#ifdef isfinite
            if (!isfinite(cnt)) {
                fprintf(stderr,
                        "Non-finite measurement - forgot EMMS cleanup?\n");
                exit(EXIT_FAILURE);
            }
#endif
            if (python) {
                printf("%.3e", cnt);
            }
            else {
                rc_bmark_print(cnt);
            }
        }
        else {
            printf("      %c", python ? '0' : '-');
        }
        if (python) {
            printf("]");
        }
        else {
            printf("\n");
        }
    }

    printf("%c\n", python ? ']' : ' ');

    rc_bmark_cleanup();
    dlclose(lib);

    return EXIT_SUCCESS;
}

/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

static void
rc_bmark_setup(void *lib, int width, int height)
{
    size_t (*align)(size_t);
    void*  (*alloc)(size_t);
    int      dim_bin;
    int      dim_u8;
    int      rot_u8;
    int      offset;
    int      size;
    int      pad;

    align = dlsym(lib, "rc_align");
    alloc = dlsym(lib, "rc_malloc");
    pad   = align(RC_BMARK_PADDING);

    dim_u8  = (*align)(width) + 2*pad;
    dim_bin = (*align)((width + 7) / 8) + 2*pad;
    rot_u8  = (*align)(height);
    offset  = RC_BMARK_PADDING*dim_u8 + pad;

    /**
     *  We perform an extra alignment of "offset", since it adjusts
     *  pointers to types that might require alignment larger than
     *  RAPP_ALIGNMEMT, e.g. uintmax_t being 64 bits and requiring
     *  64-bit alignments on a target with 32-bit pointers and
     *  RC_ALIGNMENT 4, such as sparc64 with the 32-bit ABI.
     *  We assume that doubling the alignment is sufficient.
     */
    offset  = 2*align(offset / 2 + 1);
    size    = MAX(dim_u8*(height + RC_BMARK_PADDING), rot_u8*width) + offset;

    rc_bmark_data.dst     = (*alloc)(size);
    rc_bmark_data.src     = (*alloc)(size);
    rc_bmark_data.aux     = (*alloc)(size);
    rc_bmark_data.aux2    = (*alloc)(size);
    rc_bmark_data.map     = (*alloc)(size);
    rc_bmark_data.dim_bin = dim_bin;
    rc_bmark_data.dim_u8  = dim_u8;
    rc_bmark_data.rot_u8  = rot_u8;
    rc_bmark_data.width   = width;
    rc_bmark_data.height  = height;
    rc_bmark_data.offset  = offset;
    rc_bmark_data.alloc   = alloc;
    rc_bmark_data.release = dlsym(lib, "rc_free");

    memset(rc_bmark_data.dst, 0, size);
    memset(rc_bmark_data.src, 0, size);
    memset(rc_bmark_data.aux, 0, size);
    memset(rc_bmark_data.aux2, 0, size);

    memset(&rc_bmark_data.map[0], 0, size/3);
    memset(&rc_bmark_data.map[dim_bin * (height / 3)], 0xff, size/3);
    memset(&rc_bmark_data.map[dim_bin * (2 * height / 3)], 0x55, size/3);

    rc_bmark_data.dst += offset;
    rc_bmark_data.src += offset;
    rc_bmark_data.aux += offset;
    rc_bmark_data.aux2 += offset;
    rc_bmark_data.map += offset;
}

static void
rc_bmark_cleanup(void)
{
    (*rc_bmark_data.release)(&rc_bmark_data.src[-rc_bmark_data.offset]);
    (*rc_bmark_data.release)(&rc_bmark_data.dst[-rc_bmark_data.offset]);
    (*rc_bmark_data.release)(&rc_bmark_data.aux[-rc_bmark_data.offset]);
    (*rc_bmark_data.release)(&rc_bmark_data.aux2[-rc_bmark_data.offset]);
    (*rc_bmark_data.release)(&rc_bmark_data.map[-rc_bmark_data.offset]);
}

static void
rc_bmark_sighandler(int signum)
{
    (void)signum;
    rc_bmark_done = 1;
}

static void
rc_bmark_print(float value)
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
rc_bmark_exec_bin(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_p_bin(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.aux,
            rc_bmark_data.src,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_bin_bin(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_bin,
            rc_bmark_data.src,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_bin_bin_m(int (*func)(), const int *args)
{
    (*func)(rc_bmark_data.dst,     rc_bmark_data.dim_bin,
            rc_bmark_data.src + 1, rc_bmark_data.dim_bin,
            rc_bmark_data.width,   rc_bmark_data.height,
            args[0]);
}

static void
rc_bmark_exec_bin_u8(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.src,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_u8_bin_c(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.map,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height,
            (int)args[0]);
}

static void
rc_bmark_exec_u8_bin(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_bin,
            rc_bmark_data.src,   rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_u8(int (*func)(), const int *args)
{
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height,
            (int)args[0], (int)args[1]);
}

static void
rc_bmark_exec_u8_bin_u8_c(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.src,   rc_bmark_data.dim_u8,
            rc_bmark_data.map,   rc_bmark_data.dim_bin,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_u8_p(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height,
            rc_bmark_data.aux);
}

static void
rc_bmark_exec_u8_u8(int (*func)(), const int *args)
{
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.src,   rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height,
            (int)args[0], (int)args[1]);
}

static void
rc_bmark_exec_u8_u8_p(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_u8,
            rc_bmark_data.src,   rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height,
            rc_bmark_data.aux);
}

static void
rc_bmark_exec_rotate(int (*func)(), const int *args)
{
    (void)args;
    (*func)(rc_bmark_data.dst, rc_bmark_data.rot_u8,
            rc_bmark_data.src, rc_bmark_data.dim_u8,
            rc_bmark_data.width, rc_bmark_data.height);
}

static void
rc_bmark_exec_thresh_pixel(int (*func)(), const int *args)
{
    const int num_thresholds = args[0];
    if (num_thresholds == 2) {
        (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_bin,
                rc_bmark_data.src,   rc_bmark_data.dim_u8,
                rc_bmark_data.aux,   rc_bmark_data.dim_u8,
                rc_bmark_data.aux2,  rc_bmark_data.dim_u8,
                rc_bmark_data.width, rc_bmark_data.height);
    }
    else {
        (*func)(rc_bmark_data.dst,   rc_bmark_data.dim_bin,
                rc_bmark_data.src,   rc_bmark_data.dim_u8,
                rc_bmark_data.aux,   rc_bmark_data.dim_u8,
                rc_bmark_data.width, rc_bmark_data.height);
    }
}
