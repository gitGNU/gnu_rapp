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
 *  @file   rc_filter.c
 *  @brief  RAPP Compute layer fixed filters, vector implementation.
 *
 *  ALGORITHM
 *  ---------
 *  The vectorized filter implementation processes one vector of destination
 *  pixels at once. The vectorization is achieved by processing many pixels
 *  for each filter point rather than many filter points for each pixels.
 *  This makes the vector size independent of the size of the filter.
 *
 *  The technique is in many ways similar to the scalar implementation,
 *  but now we read vectors instead of individual pixels. If we for
 *  a particular position in the source image read the vector B, and
 *  also read the vector A above and the vector C below, we will have
 *  all the vector elements aligned vectically. This means that we can
 *  compute the vectorized 3x1 vectical filter response by using
 *  element-wise arithmetic operations, producing the result vector D.
 *
 *  a1 a2 a3 a4
 *  b1 b2 b3 b4 -> d1 d2 d3 d4
 *  c1 c2 c3 c4
 *
 *  The horizontal case is a little bit more complicated. Assume that
 *  we have three consecutive vectors in the horizontal direction,
 *  A, B and C. By shifting and ORing the vectors A, B and B, C,
 *  we can produce three vectors that are aligned as in the vertical
 *  case. We can now again use element-wise arithmetic operations on
 *  these vectors, producing the horizontal 1x3 filter response D.
 *
 *                                             a4 b1 b2 b3
 *  a1 a2 a3 a4 | b1 b2 b3 b3 | c1 c2 c3 c4 -> b1 b2 b3 b4 -> d1 d2 d3 d4
 *                                             b2 b3 b4 c1
 *
 *  The vector operations always keep data in 8 fields. This gives a
 *  significant speedup compared to the traditional approach of expanding
 *  to 16 bits, performing twice as many operations and the reduce to
 *  8 bits. There are however some drawbacks:
 *
 *   - The result is not always correctly rounded
 *   - Only some filters can be implemented
 *
 *  The operations performed are variants of the average (AVGT/AVGR),
 *  half subtraction (SUBHT/SUBHR) and absolute-value subtraction (SUBA).
 *  By mixing the truncating and rounding versions, we can center the
 *  error around zero for most operations.
 *
 *  Example: The horizontal Sobel filter
 *  ------------------------------------
 *  The horizontal Sobel filter kernel is
 *
 *            [1 0 -1]
 *    S = 1/8 [2 0 -2].
 *            [1 0 -1]
 *
 *  It is separable into a vertical and a horizontal component,
 *
 *    Sx = 1/2 [1 0 -1]
 *
 *             [1]
 *    Sy = 1/4 [2]
 *             [1]
 *
 *  Using the average and the half-subtraction operations we can
 *  compute the components as
 *
 *    Sx = subh(c, a)
 *
 *  and
 *
 *    Sy = avg(avg(a, c), b),
 *
 *  where [a b c] are pixels in the image. For the 3x3 neighborhood
 *
 *    [p11 p12 p13]
 *    [p21 p22 p23]
 *    [p31 p32 p33]
 *
 *  can compute the sliding vertical filter response
 *
 *    vk = avg(avg(p1k, p3k), p2k)
 *
 *  and then the total filter response
 *
 *    s = subh(v3, v1).
 *
 *  All this requires the use of exact arthimetics. When rounding to
 *  8 bits, we will get roundoff errors in each operation. However,
 *  by using both truncated (rounded down), and rounded (up) operations,
 *  we can minimize the error and center it around zero.
 *
 *  It turns out that we need to use both AVGT and AVGR for the vertical
 *  component. Furthermore, we need to compute the two vertical intermediate
 *  values v3 and v1 using DIFFERENT combinations of AVGT and AVGR.
 *  This essentially makes the operation non-separable. For the vertical
 *  operations, we compute
 *
 *    vk = AVGT(AVGT(p1k, p3k), p2k)
 *    uk = AVGR(AVGT(p1k, p3k), p2k)
 *
 *  and for the horizontal one we compute
 *
 *    s = SUBHR(u3, v1).
 *
 *  In this case we need to compute two sliding vertical intermediate
 *  values instead of just one in the separable case. Filters with this
 *  property are called PARTLY separable in this implementation.
 *
 *  IMPLEMENTATION
 *  --------------
 *  There are template macros for traversing the image. These accept
 *  macros as arguments for performing the actual vector operations
 *  in the inner loop.
 *
 *  The 3x3 template is used for all 3x3 operations. It supports separable,
 *  partly separable and non-separable filters. The inner loop is unrolled
 *  three times. This makes it possible to store the intermediate vectors
 *  for separable filters in CPU registers.
 *
 *  First the vertical operation is performed. The input is three vectors
 *  for the three rows. The output is also three vectors. These vectors
 *  are cached by the template, keeping three consecutive sets output
 *  vectors. These 3x3 vectors are then fed to horizontal operation macro.
 *
 *  A fully separable operation will only set the first output vector in
 *  the vertical operation. The horizontal operation will then only use
 *  the 3 of the 9 sliding vectors. A non-separable operations will
 *  just forward all the input vectors of the vertical operations so
 *  that the horizontal operation can use all 9 vectors.
 *
 *  The 3x3 vector operation macros are suffixed with V for the vertical
 *  component and H for the horizontal component. In those cases where it
 *  is possible to use different combinations of AVGT/AVGR and SUBHT/SUBHR,
 *  the implementation will be determined by the RC_VEC_HINT_* mechanism
 *  of the vector interface.
 */

#include "rc_impl_cfg.h" /* Implementation cfg */
#include "rc_util.h"     /* RC_DIV_CEIL()      */
#include "rc_vector.h"   /* Vector API         */
#include "rc_filter.h"   /* Fixed-filter API   */

/*
 * -------------------------------------------------------------
 *  2x2 template macros
 * -------------------------------------------------------------
 */

/**
 *  The 1x2 convolution template.
 *  The unroll factor is a compile-time constant.
 */
#ifdef RC_VEC_ALIGNC
#define RC_FILTER_1X2_TEMPLATE(dst, dst_dim, src, src_dim,         \
                               width, height, vecop, unroll)       \
do {                                                               \
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);                     \
    int len = tot / MAX(unroll, 2);                                \
    int rem = tot % MAX(unroll, 2);                                \
    int y;                                                         \
                                                                   \
    RC_VEC_DECLARE();                                              \
    for (y = 0; y < (height); y++) {                               \
        rc_vec_t v1, v2;                                           \
        int      j = y*(dst_dim);                                  \
        int      i = y*(src_dim);                                  \
        int      x;                                                \
                                                                   \
        /* Load the first vector */                                \
        RC_VEC_LOAD(v1, &(src)[i - RC_VEC_SIZE]);                  \
                                                                   \
        /* Handle all full periods */                              \
        for (x = 0; x < len; x++) {                                \
            RC_FILTER_1X2_ITER(dst, src, j, i, v1, v2, vecop);     \
            RC_FILTER_1X2_ITER(dst, src, j, i, v2, v1, vecop);     \
                                                                   \
            if ((unroll) == 4) {                                   \
                RC_FILTER_1X2_ITER(dst, src, j, i, v1, v2, vecop); \
                RC_FILTER_1X2_ITER(dst, src, j, i, v2, v1, vecop); \
            }                                                      \
        }                                                          \
                                                                   \
        /* Handle the partial period */                            \
        if (rem > 1) {                                             \
            RC_FILTER_1X2_ITER(dst, src, j, i, v1, v2, vecop);     \
            RC_FILTER_1X2_ITER(dst, src, j, i, v2, v1, vecop);     \
        }                                                          \
        if (rem & 1) {                                             \
            RC_FILTER_1X2_ITER(dst, src, j, i, v1, v2, vecop);     \
        }                                                          \
    }                                                              \
    RC_VEC_CLEANUP();                                              \
} while (0)

/**
 *  A 1x2 iteration step.
 */
#define RC_FILTER_1X2_ITER(dst, src, j, i, v1, v2, vecop) \
do {                                                      \
    rc_vec_t v12;                                         \
    RC_VEC_LOAD(v2, &(src)[i]);                           \
    RC_VEC_ALIGNC(v12, v1, v2, RC_VEC_SIZE - 1);          \
    vecop(v12, v2, v12);                                  \
    RC_VEC_STORE(&(dst)[j], v12);                         \
    (i) += RC_VEC_SIZE, (j) += RC_VEC_SIZE;               \
} while (0)
#endif /* RC_VEC_ALIGNC */

/**
 *  The 1x2 convolution template.
 *  The unroll factor is a compile-time constant.
 */
#define RC_FILTER_2X1_TEMPLATE(dst, dst_dim, src, src_dim,      \
                               width, height, vecop, unroll)    \
do {                                                            \
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);                  \
    int len = tot / (unroll);                                   \
    int rem = tot % (unroll);                                   \
    int y;                                                      \
                                                                \
    RC_VEC_DECLARE();                                           \
    for (y = 0; y < (height); y++) {                            \
        int j  = y*(dst_dim);                                   \
        int i2 = y*(src_dim);                                   \
        int i1 = i2 - (src_dim);                                \
        int x;                                                  \
                                                                \
        /* Handle all full periods */                           \
        for (x = 0; x < len; x++) {                             \
            RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop);     \
            if ((unroll) >= 2) {                                \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop); \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop); \
                RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop); \
            }                                                   \
        }                                                       \
                                                                \
        /* Handle the partial period */                         \
        for (x = 0; x < rem; x++) {                             \
            RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop);     \
        }                                                       \
    }                                                           \
    RC_VEC_CLEANUP();                                           \
} while (0)

/**
 *  A 2x1 iteration step.
 */
#define RC_FILTER_2X1_ITER(dst, src, j, i1, i2, vecop) \
do {                                                   \
    rc_vec_t v1, v2;                                   \
    RC_VEC_LOAD(v1, &(src)[i1]);                       \
    RC_VEC_LOAD(v2, &(src)[i2]);                       \
    vecop(v1, v2, v1);                                 \
    RC_VEC_STORE(&(dst)[j], v1);                       \
    (i1) += RC_VEC_SIZE;                               \
    (i2) += RC_VEC_SIZE;                               \
    (j)  += RC_VEC_SIZE;                               \
} while (0)

/**
 *  A 2x2 difference magnitude iteration step.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGR && \
    defined RC_VEC_SUBA
#define RC_FILTER_2X2_ITER(dst, src, j, i1, i2,   \
                           v11, v12, v21, v22)    \
do {                                              \
    rc_vec_t u1, u2;                              \
    RC_VEC_LOAD(v12, &(src)[i1]);                 \
    RC_VEC_LOAD(v22, &(src)[i2]);                 \
    RC_VEC_ALIGNC(u1, v21, v22, RC_VEC_SIZE - 1); \
    RC_VEC_SUBA(u1, u1,  v22);                    \
    RC_VEC_SUBA(u2, v12, v22);                    \
    RC_VEC_AVGR(u1, u1, u2);                      \
    RC_VEC_STORE(&(dst)[j], u1);                  \
    (i1) += RC_VEC_SIZE;                          \
    (i2) += RC_VEC_SIZE;                          \
    (j)  += RC_VEC_SIZE;                          \
} while (0)
#endif


/*
 * -------------------------------------------------------------
 *  3x3 operation macros
 * -------------------------------------------------------------
 */

/**
 *  Horizontal Sobel operator, vertical component.
 *  Partly separable. We store the result of column-wise
 *  gaussian filter, computed using AVGT + AVGT and AVGR + AVGT
 *  in the first and second output values, respectivly.
 */
#if defined RC_VEC_AVGT && defined RC_VEC_AVGR
#define RC_FILTER_SOBEL_HORZ_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                      \
    RC_VEC_AVGT(v1x, p1,  p3);                            \
    RC_VEC_AVGR(v2x, v1x, p2);                            \
    RC_VEC_AVGT(v1x, v1x, p2);                            \
} while (0)
#endif

/**
 *  Horizontal Sobel operator, horizontal component.
 *  Partly separable. We use the first and second rows,
 *  containing the gaussian result from the vertical operation.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_SUBHR
#define RC_FILTER_SOBEL_HORZ_H(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23;                                  \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                    \
    RC_VEC_SUBHR(sum, u23, u12);                        \
} while (0)
#endif

/**
 *  Vertical Sobel operator, vertical component.
 *  Partly separable. We store the result of column-wise
 *  difference filter, computed using SUBHT and SUBHR in
 *  the first and second output values, respectivly.
 */
#if defined RC_VEC_SUBHT && defined RC_VEC_SUBHR
#define RC_FILTER_SOBEL_VERT_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                      \
    RC_VEC_SUBHT(v1x, p3, p1);                            \
    RC_VEC_SUBHR(v2x, p3, p1);                            \
} while (0)
#endif

/**
 *  Vertical Sobel operator, horizontal component.
 *  Partly separable. We use the first and second rows,
 *  containing the difference result from the vertical operation.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGR
#define RC_FILTER_SOBEL_VERT_H(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23;                                  \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                    \
    RC_VEC_AVGR(sum, u12, u23);                         \
    RC_VEC_AVGR(sum, sum, v22);                         \
} while (0)
#endif

/**
 *  Horizontal absolute-value Sobel operator, vertical component.
 *  Fully separable. We store the vertical gaussian result in the
 *  first output value.
 */
#ifdef RC_VEC_HINT_AVGT
#define RC_FILTER_SOBEL_HORZ_ABS_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                          \
    RC_VEC_AVGT(v1x, p1,  p3);                                \
    RC_VEC_AVGT(v1x, v1x, p2);                                \
} while (0)
#elif defined RC_VEC_AVGR
#define RC_FILTER_SOBEL_HORZ_ABS_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                          \
    RC_VEC_AVGR(v1x, p1,  p3);                                \
    RC_VEC_AVGR(v1x, v1x, p2);                                \
} while (0)
#endif

/**
 *  Horizontal absolute-value Sobel operator, horizontal component.
 *  Fully separable. We use the first row, containing the gaussian
 *  result from the vertical operation.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_SUBA
#define RC_FILTER_SOBEL_HORZ_ABS_H(sum, v11, v12, v13, v21, \
                                   v22, v23, v31, v32, v33) \
do {                                                        \
    rc_vec_t u12, u23;                                      \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);          \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                        \
    RC_VEC_SUBA(sum, u12, u23);                             \
} while (0)
#endif

/**
 *  Vertical absolute-value Sobel operator, vertical component.
 *  Partly separable. We store the vertical difference result from
 *  SUBHT and SUBHR in the first and second variables, respectivly.
 */
#if defined RC_VEC_SUBHT && defined RC_VEC_SUBHR
#define RC_FILTER_SOBEL_VERT_ABS_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                          \
    RC_VEC_SUBHT(v1x, p3, p1);                                \
    RC_VEC_SUBHR(v2x, p3, p1);                                \
} while (0)
#endif

/**
 *  Vertical absolute-value Sobel operator, horizontal component.
 *  Partly separable. We use the first two rows, containing the
 *  difference results from the vertical operation.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_NOT && defined RC_VEC_SUBA
#ifdef  RC_VEC_HINT_AVGT
#define RC_FILTER_SOBEL_VERT_ABS_H(sum, v11, v12, v13, v21, \
                                   v22, v23, v31, v32, v33) \
do {                                                        \
    rc_vec_t u12, u23, n2;                                  \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);          \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                        \
    RC_VEC_NOT(n2, v22);                                    \
    RC_VEC_AVGT(sum, u12, u23);                             \
    RC_VEC_SUBA(sum, sum, n2);                              \
} while (0)
#elif defined RC_VEC_AVGR
#define RC_FILTER_SOBEL_VERT_ABS_H(sum, v11, v12, v13, v21, \
                                   v22, v23, v31, v32, v33) \
do {                                                        \
    rc_vec_t u12, u23, n2;                                  \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);          \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                        \
    RC_VEC_NOT(n2, v12);                                    \
    RC_VEC_AVGR(sum, u12, u23);                             \
    RC_VEC_SUBA(sum, sum, n2);                              \
} while (0)
#endif
#endif /* RC_VEC_ALIGNC && RC_VEC_NOT && RC_VEC_SUBA */

/**
 *  Sobel magnitude operator, vertical component.
 *  Fully separable.
 */
#ifdef  RC_VEC_HINT_AVGT
#define RC_FILTER_SOBEL_MAGN_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                      \
    RC_VEC_AVGT(v1x, p1,  p3);                            \
    RC_VEC_AVGT(v1x, v1x, p2);                            \
    RC_VEC_SUBHT(v2x, p1, p3);                            \
} while (0)
#elif defined RC_VEC_AVGR && defined RC_VEC_SUBHR
#define RC_FILTER_SOBEL_MAGN_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                      \
    RC_VEC_AVGR(v1x, p1,  p3);                            \
    RC_VEC_AVGR(v1x, v1x, p2);                            \
    RC_VEC_SUBHR(v2x, p1, p3);                            \
} while (0)
#endif

/**
 *  Sobel magnitude operator, vertical component.
 *  Fully separable.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_NOT && \
    defined RC_VEC_SUBA   && defined RC_VEC_AVGT && \
    defined RC_VEC_AVGR
/* Not #ifdef RC_VEC_HINT_AVGR; balances with RC_FILTER_SOBEL_MAGN_V. */
#ifdef RC_VEC_HINT_AVGT
#define RC_FILTER_SOBEL_MAGN_H(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23, h12, h23, n2;                    \
    /* Horizontal filter absolute value */              \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                    \
    RC_VEC_SUBA(u12, u12, u23);                         \
    /* Vertical filter absolute value */                \
    RC_VEC_ALIGNC(h12, v21, v22, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(h23, v22, v23, 1);                    \
    RC_VEC_NOT(n2, v22);                                \
    RC_VEC_AVGR(h12, h12, h23);                         \
    RC_VEC_SUBA(h12, h12, n2);                          \
    /* Combine results */                               \
    RC_VEC_AVGT(sum, u12, h12);                         \
} while (0)
#else
#define RC_FILTER_SOBEL_MAGN_H(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23, h12, h23, n2;                    \
    /* Horizontal filter absolute value */              \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                    \
    RC_VEC_SUBA(u12, u12, u23);                         \
    /* Vertical filter absolute value */                \
    RC_VEC_ALIGNC(h12, v21, v22, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(h23, v22, v23, 1);                    \
    RC_VEC_NOT(n2, v22);                                \
    RC_VEC_AVGT(h12, h12, h23);                         \
    RC_VEC_SUBA(h12, h12, n2);                          \
    /* Combine results */                               \
    RC_VEC_AVGT(sum, u12, h12);                         \
} while (0)
#endif
#endif /* RC_VEC_ALIGNC && RC_VEC_NOT && RC_VEC_SUBA && RC_VEC_AVGT */

/**
 *  Gaussian operator, vertical component.
 *  Fully separable. We store the result of the vertical
 *  component in the first output value.
 */
#ifdef RC_VEC_AVGT
#define RC_FILTER_GAUSS_V(v1x, v2x, v3x, p1, p2, p3) \
do {                                                 \
    RC_VEC_AVGT(v1x, p1,  p3);                       \
    RC_VEC_AVGT(v1x, v1x, p2);                       \
} while (0)
#endif

/**
 *  Gaussian operator, horizontal component.
 *  Fully separable. We use the vertical result in the first row.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGR
#define RC_FILTER_GAUSS_H(sum, v11, v12, v13, v21, \
                          v22, v23, v31, v32, v33) \
do {                                               \
    rc_vec_t u12, u23;                             \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1); \
    RC_VEC_ALIGNC(u23, v12, v13, 1);               \
    RC_VEC_AVGR(sum, u12, u23);                    \
    RC_VEC_AVGR(sum, sum, v12);                    \
} while (0)
#endif

/**
 *  Vertical operator for non-separable filters,
 *  forwarding p1, p2 and p3 to all output values.
 */
#define RC_FILTER_FORWARD(v1x, v2x, v3x, p1, p2, p3) \
    ((v1x) = (p1), (v2x) = (p2), (v3x) = (p3))

/**
 *  Laplacian operator. Non-separable.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGT && defined RC_VEC_SUBHR
#define RC_FILTER_LAPLACE(sum, v11, v12, v13, v21,  \
                          v22, v23, v31, v32, v33)  \
do {                                                \
    rc_vec_t u12, u23;                              \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);  \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                \
    RC_VEC_AVGT(u12, u12, u23);                     \
    RC_VEC_AVGT(sum, v12, v32);                     \
    RC_VEC_AVGT(sum, sum, u12);                     \
    RC_VEC_SUBHR(sum, v22, sum);                    \
} while (0)
#endif

/**
 *  Absolute-value laplacian operator. Non-separable.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGT && \
    defined RC_VEC_AVGR   && defined RC_VEC_SUBA
#ifdef  RC_VEC_HINT_AVGT
#define RC_FILTER_LAPLACE_ABS(sum, v11, v12, v13, v21, \
                              v22, v23, v31, v32, v33) \
do {                                                   \
    rc_vec_t u12, u23;                                 \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);     \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                   \
    RC_VEC_AVGT(u12, u12, u23);                        \
    RC_VEC_AVGT(sum, v12, v32);                        \
    RC_VEC_AVGR(sum, sum, u12);                        \
    RC_VEC_SUBA(sum, v22, sum);                        \
} while (0)
#else
#define RC_FILTER_LAPLACE_ABS(sum, v11, v12, v13, v21, \
                              v22, v23, v31, v32, v33) \
do {                                                   \
    rc_vec_t u12, u23;                                 \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);     \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                   \
    RC_VEC_AVGR(u12, u12, u23);                        \
    RC_VEC_AVGR(sum, v12, v32);                        \
    RC_VEC_AVGT(sum, sum, u12);                        \
    RC_VEC_SUBA(sum, v22, sum);                        \
} while (0)
#endif
#endif /* RC_VEC_ALIGNC && RC_VEC_AVGT && RC_VEC_AVGR && RC_VEC_SUBA */

/**
 *  Highpass operator. Non-separable.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGT && \
    defined RC_VEC_AVGR   && defined RC_VEC_SUBHR
#define RC_FILTER_HIGHPASS(sum, v11, v12, v13, v21, \
                           v22, v23, v31, v32, v33) \
do {                                                \
    rc_vec_t u12, u23, acc1, acc2, acc3;            \
    /* Combine 11 and 13 to acc1 */                 \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);  \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                \
    RC_VEC_AVGT(acc1, u12, u23);                    \
    /* Combine 21 and 23 to acc2 */                 \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);  \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                \
    RC_VEC_AVGT(acc2, u12, u23);                    \
    /* Combine acc1 and acc2 to acc1 */             \
    RC_VEC_AVGR(acc1, acc1, acc2);                  \
    /* Combine 31 and 33 to acc2 */                 \
    RC_VEC_ALIGNC(u12, v31, v32, RC_VEC_SIZE - 1);  \
    RC_VEC_ALIGNC(u23, v32, v33, 1);                \
    RC_VEC_AVGT(acc2, u12, u23);                    \
    /* Combine 12 and 32 to acc3 */                 \
    RC_VEC_AVGT(acc3, v12, v32);                    \
    /* Combine acc2 and acc3 to acc2 */             \
    RC_VEC_AVGT(acc2, acc2, acc3);                  \
    /* Combine acc1 and acc2 to acc1 */             \
    RC_VEC_AVGT(acc1, acc1, acc2);                  \
    /* Subtract from the center 22 */               \
    RC_VEC_SUBHR(sum, v22, acc1);                   \
} while (0)
#endif

/**
 *  Absolute-value highpass operator. Non-separable.
 */
#if defined RC_VEC_ALIGNC && defined RC_VEC_AVGT && \
    defined RC_VEC_AVGR   && defined RC_VEC_SUBA
#ifdef  RC_VEC_HINT_AVGT
#define RC_FILTER_HIGHPASS_ABS(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23, acc1, acc2, acc3;                \
    /* Combine 11 and 13 to acc1 */                     \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                    \
    RC_VEC_AVGT(acc1, u12, u23);                        \
    /* Combine 21 and 23 to acc2 */                     \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                    \
    RC_VEC_AVGT(acc2, u12, u23);                        \
    /* Combine acc1 and acc2 to acc1 */                 \
    RC_VEC_AVGR(acc1, acc1, acc2);                      \
    /* Combine 31 and 33 to acc2 */                     \
    RC_VEC_ALIGNC(u12, v31, v32, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v32, v33, 1);                    \
    RC_VEC_AVGT(acc2, u12, u23);                        \
    /* Combine 12 and 32 to acc3 */                     \
    RC_VEC_AVGT(acc3, v12, v32);                        \
    /* Combine acc2 and acc3 to acc2 */                 \
    RC_VEC_AVGT(acc2, acc2, acc3);                      \
    /* Combine acc1 and acc2 to acc1 */                 \
    RC_VEC_AVGR(acc1, acc1, acc2);                      \
    /* Subtract from the center 22 */                   \
    RC_VEC_SUBA(sum, v22, acc1);                        \
} while (0)
#else
#define RC_FILTER_HIGHPASS_ABS(sum, v11, v12, v13, v21, \
                               v22, v23, v31, v32, v33) \
do {                                                    \
    rc_vec_t u12, u23, acc1, acc2, acc3;                \
    /* Combine 11 and 13 to acc1 */                     \
    RC_VEC_ALIGNC(u12, v11, v12, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v12, v13, 1);                    \
    RC_VEC_AVGR(acc1, u12, u23);                        \
    /* Combine 21 and 23 to acc2 */                     \
    RC_VEC_ALIGNC(u12, v21, v22, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v22, v23, 1);                    \
    RC_VEC_AVGR(acc2, u12, u23);                        \
    /* Combine acc1 and acc2 to acc1 */                 \
    RC_VEC_AVGR(acc1, acc1, acc2);                      \
    /* Combine 31 and 33 to acc2 */                     \
    RC_VEC_ALIGNC(u12, v31, v32, RC_VEC_SIZE - 1);      \
    RC_VEC_ALIGNC(u23, v32, v33, 1);                    \
    RC_VEC_AVGR(acc2, u12, u23);                        \
    /* Combine 12 and 32 to acc3 */                     \
    RC_VEC_AVGR(acc3, v12, v32);                        \
    /* Combine acc2 and acc3 to acc2 */                 \
    RC_VEC_AVGT(acc2, acc2, acc3);                      \
    /* Combine acc1 and acc2 to acc1 */                 \
    RC_VEC_AVGT(acc1, acc1, acc2);                      \
    /* Subtract from the center 22 */                   \
    RC_VEC_SUBA(sum, v22, acc1);                        \
} while (0)
#endif
#endif /* RC_VEC_ALIGNC && RC_VEC_AVGT && RC_VEC_AVGR && RC_VEC_SUBA */


/*
 * -------------------------------------------------------------
 *  3x3 template macros
 * -------------------------------------------------------------
 */

/**
 *  The 3x3 separable/non-separable convolution template.
 */
#ifdef RC_VEC_ZERO
#define RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim,               \
                               width, height, horz, vert)                \
do {                                                                     \
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);   /* Total num vectors */ \
    int len = tot / 3;                           /* Full 3-periods    */ \
    int rem = tot % 3;                           /* Partial period    */ \
    int y;                                                               \
                                                                         \
    RC_VEC_DECLARE();                                                    \
    for (y = 0; y < (height); y++) {                                     \
        rc_vec_t v11, v12, v13;                                          \
        rc_vec_t v21, v22, v23;                                          \
        rc_vec_t v31, v32, v33;                                          \
        rc_vec_t s1, s2, s3;                                             \
        int      j  = y*(dst_dim);               /* Dst row index     */ \
        int      i2 = y*(src_dim) + RC_VEC_SIZE; /* Cur src row idx   */ \
        int      i1 = i2 - (src_dim);            /* Above src row idx */ \
        int      i3 = i2 + (src_dim);            /* Below src row idx */ \
        int      x;                                                      \
                                                                         \
        /* Set up the first column */                                    \
        RC_VEC_LOAD(s1, &(src)[i1 - 2*RC_VEC_SIZE]);                     \
        RC_VEC_LOAD(s2, &(src)[i2 - 2*RC_VEC_SIZE]);                     \
        RC_VEC_LOAD(s3, &(src)[i3 - 2*RC_VEC_SIZE]);                     \
        vert(v11, v21, v31, s1, s2, s3);                                 \
                                                                         \
        /* Set up the second column */                                   \
        RC_VEC_LOAD(s1, &(src)[i1 - RC_VEC_SIZE]);                       \
        RC_VEC_LOAD(s2, &(src)[i2 - RC_VEC_SIZE]);                       \
        RC_VEC_LOAD(s3, &(src)[i3 - RC_VEC_SIZE]);                       \
        vert(v12, v22, v32, s1, s2, s3);                                 \
                                                                         \
        /* Clear the third column */                                     \
        RC_VEC_ZERO(v13);                                                \
        RC_VEC_ZERO(v23);                                                \
        RC_VEC_ZERO(v33);                                                \
                                                                         \
        /* Kill compiler warnings */                                     \
        (void)v21, (void)v22;                                            \
        (void)v31, (void)v32;                                            \
                                                                         \
        /* Handle all full periods */                                    \
        for (x = 0; x < len; x++) {                                      \
            RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,                  \
                               v11, v12, v13,                            \
                               v21, v22, v23,                            \
                               v31, v32, v33,                            \
                               vert, horz);                              \
            RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,                  \
                               v12, v13, v11,                            \
                               v22, v23, v21,                            \
                               v32, v33, v31,                            \
                               vert, horz);                              \
            RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,                  \
                               v13, v11, v12,                            \
                               v23, v21, v22,                            \
                               v33, v31, v32,                            \
                               vert, horz);                              \
        }                                                                \
                                                                         \
        /* Handle the partial period */                                  \
        if (rem > 0) {                                                   \
            RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,                  \
                               v11, v12, v13,                            \
                               v21, v22, v23,                            \
                               v31, v32, v33,                            \
                               vert, horz);                              \
        }                                                                \
        if (rem > 1) {                                                   \
            RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,                  \
                               v12, v13, v11,                            \
                               v22, v23, v21,                            \
                               v32, v33, v31,                            \
                               vert, horz);                              \
        }                                                                \
    }                                                                    \
    RC_VEC_CLEANUP();                                                    \
} while (0)
#endif

/**
 *  A separable/non-separable 3x3 iteration step.
 */
#define RC_FILTER_3X3_ITER(dst, src, j, i1, i2, i3,         \
                           v11, v12, v13,                   \
                           v21, v22, v23,                   \
                           v31, v32, v33,                   \
                           vert, horz)                      \
do {                                                        \
    rc_vec_t p1, p2, p3, sum;                               \
    RC_VEC_LOAD(p1, &(src)[i1]);                            \
    RC_VEC_LOAD(p2, &(src)[i2]);                            \
    RC_VEC_LOAD(p3, &(src)[i3]);                            \
    vert(v13, v23, v33, p1, p2, p3);                        \
    horz(sum, v11, v12, v13, v21, v22, v23, v31, v32, v33); \
    RC_VEC_STORE(&(dst)[j], sum);                           \
    (i1) += RC_VEC_SIZE;                                    \
    (i2) += RC_VEC_SIZE;                                    \
    (i3) += RC_VEC_SIZE;                                    \
    (j)  += RC_VEC_SIZE;                                    \
} while (0)


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  1x2 horizontal difference.
 *  The error is within [-0.5, 0] ulp.
 */
#if RC_IMPL(rc_filter_diff_1x2_horz_u8, 1)
#if defined RC_FILTER_1X2_TEMPLATE && defined RC_VEC_SUBHR
void
rc_filter_diff_1x2_horz_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    RC_FILTER_1X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_VEC_SUBHR,
                           RC_UNROLL(rc_filter_diff_1x2_horz_u8));
}
#endif
#endif

/**
 *  1x2 horizontal difference, absolute value.
 *  The result is exact.
 */
#if RC_IMPL(rc_filter_diff_1x2_horz_abs_u8, 1)
#if defined RC_FILTER_1X2_TEMPLATE && defined RC_VEC_SUBA
void
rc_filter_diff_1x2_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height)
{
    RC_FILTER_1X2_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_VEC_SUBA,
                           RC_UNROLL(rc_filter_diff_1x2_horz_abs_u8));
}
#endif
#endif

/**
 *  2x1 vertical difference.
 *  The error is within [-0.5, 0] ulp.
 */
#if RC_IMPL(rc_filter_diff_2x1_vert_u8, 1)
#if defined RC_FILTER_2X1_TEMPLATE && defined RC_VEC_SUBHR
void
rc_filter_diff_2x1_vert_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    RC_FILTER_2X1_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_VEC_SUBHR,
                           RC_UNROLL(rc_filter_diff_2x1_vert_u8));
}
#endif
#endif

/**
 *  2x1 vertical difference, absolute value.
 *  The result is exact.
 */
#if RC_IMPL(rc_filter_diff_2x1_vert_abs_u8, 1)
#if defined RC_FILTER_2X1_TEMPLATE && defined RC_VEC_SUBA
void
rc_filter_diff_2x1_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                               const uint8_t *restrict src, int src_dim,
                               int width, int height)
{
    RC_FILTER_2X1_TEMPLATE(dst, dst_dim, src, src_dim,
                           width, height, RC_VEC_SUBA,
                           RC_UNROLL(rc_filter_diff_2x1_vert_abs_u8));
}
#endif
#endif

/**
 *  2x2 difference magnitude.
 *  The error is within [-0.5, 0.5] ulp.
 */
#if RC_IMPL(rc_filter_diff_2x2_magn_u8, 1)
#ifdef RC_FILTER_2X2_ITER
void
rc_filter_diff_2x2_magn_u8(uint8_t *restrict dst, int dst_dim,
                           const uint8_t *restrict src, int src_dim,
                           int width, int height)
{
    int tot = RC_DIV_CEIL(width, RC_VEC_SIZE);
    int len = tot / MAX(2, RC_UNROLL(rc_filter_diff_2x2_magn_u8));
    int rem = tot % MAX(2, RC_UNROLL(rc_filter_diff_2x2_magn_u8));
    int y;

    for (y = 0; y < height; y++) {
        rc_vec_t v11, v12;
        rc_vec_t v21, v22;
        int      j  = y*dst_dim;
        int      i2 = y*src_dim;
        int      i1 = i2 - src_dim;
        int      x;

        /* Load the first column */
        RC_VEC_LOAD(v11, &src[i1 - RC_VEC_SIZE]);
        RC_VEC_LOAD(v21, &src[i2 - RC_VEC_SIZE]);

        /* Handle all full periods */
        for (x = 0; x < len; x++) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v11, v12, v21, v22);
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v12, v11, v22, v21);

            if (RC_UNROLL(rc_filter_diff_2x2_magn_u8) == 4) {
                 RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v11, v12, v21, v22);
                 RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v12, v11, v22, v21);
            }
        }

        /* Handle the partial period */
        if (rem > 1) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v11, v12, v21, v22);
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v12, v11, v22, v21);
        }
        if (rem & 1) {
            RC_FILTER_2X2_ITER(dst, src, j, i1, i2, v11, v12, v21, v22);
        }
    }

    RC_VEC_CLEANUP();
}
#endif
#endif

/**
 *  3x3 horizontal Sobel gradient.
 *  The error is within [-0.625, 0.625] ulp.
 */
#if RC_IMPL(rc_filter_sobel_3x3_horz_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_SOBEL_HORZ_H && \
    defined RC_FILTER_SOBEL_HORZ_V
void
rc_filter_sobel_3x3_horz_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SOBEL_HORZ_H, RC_FILTER_SOBEL_HORZ_V);
}
#endif
#endif

/**
 *  3x3 horizontal Sobel gradient, absolute value.
 *  The error is within [-0.75, 0.75] ulp.
 */
#if RC_IMPL(rc_filter_sobel_3x3_horz_abs_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_SOBEL_HORZ_ABS_H && \
    defined RC_FILTER_SOBEL_HORZ_ABS_V
void
rc_filter_sobel_3x3_horz_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SOBEL_HORZ_ABS_H,
                           RC_FILTER_SOBEL_HORZ_ABS_V);
}
#endif
#endif

/**
 *  3x3 vertical Sobel gradient.
 *  The error is within [-0.625, 0.625] ulp.
 */
#if RC_IMPL(rc_filter_sobel_3x3_vert_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_SOBEL_VERT_H && \
    defined RC_FILTER_SOBEL_VERT_V
void
rc_filter_sobel_3x3_vert_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SOBEL_VERT_H, RC_FILTER_SOBEL_VERT_V);
}
#endif
#endif

/**
 *  3x3 vertical Sobel gradient, absolute value.
 *  The error is within [-0.75, 0.75] ulp.
 */
#if RC_IMPL(rc_filter_sobel_3x3_vert_abs_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_SOBEL_VERT_ABS_H && \
    defined RC_FILTER_SOBEL_VERT_ABS_V
void
rc_filter_sobel_3x3_vert_abs_u8(uint8_t *restrict dst, int dst_dim,
                                const uint8_t *restrict src, int src_dim,
                                int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SOBEL_VERT_ABS_H,
                           RC_FILTER_SOBEL_VERT_ABS_V);
}
#endif
#endif

/**
 *  3x3 Sobel gradient magnitude.
 *  The error is within [-1.25, 0.75] ulp.
 */
#if RC_IMPL(rc_filter_sobel_3x3_magn_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_SOBEL_MAGN_H && \
    defined RC_FILTER_SOBEL_MAGN_V
void
rc_filter_sobel_3x3_magn_u8(uint8_t *restrict dst, int dst_dim,
                            const uint8_t *restrict src, int src_dim,
                            int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_SOBEL_MAGN_H, RC_FILTER_SOBEL_MAGN_V);
}
#endif
#endif

/**
 *  3x3 gaussian.
 *  The error is within [-0.75, 0.75] ulp.
 */
#if RC_IMPL(rc_filter_gauss_3x3_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_GAUSS_H && \
    defined RC_FILTER_GAUSS_V
void
rc_filter_gauss_3x3_u8(uint8_t *restrict dst, int dst_dim,
                       const uint8_t *restrict src, int src_dim,
                       int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_GAUSS_H, RC_FILTER_GAUSS_V);
}
#endif
#endif

/**
 *  3x3 laplacian.
 *  The error is within [-0.5, 0.5] ulp.
 */
#if RC_IMPL(rc_filter_laplace_3x3_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_LAPLACE && \
    defined RC_FILTER_FORWARD
void
rc_filter_laplace_3x3_u8(uint8_t *restrict dst, int dst_dim,
                         const uint8_t *restrict src, int src_dim,
                         int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_LAPLACE, RC_FILTER_FORWARD);
}
#endif
#endif

/**
 *  3x3 laplacian, absolute value.
 *  The error is within [-0.5, 0.5] ulp.
 */
#if RC_IMPL(rc_filter_laplace_3x3_abs_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_LAPLACE_ABS && \
    defined RC_FILTER_FORWARD
void
rc_filter_laplace_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                             const uint8_t *restrict src, int src_dim,
                             int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_LAPLACE_ABS, RC_FILTER_FORWARD);
}
#endif
#endif

/**
 *  3x3 highpass filter.
 *  The error is within [-0.625, 0.625] ulp.
 */
#if RC_IMPL(rc_filter_highpass_3x3_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_HIGHPASS && \
    defined RC_FILTER_FORWARD
void
rc_filter_highpass_3x3_u8(uint8_t *restrict dst, int dst_dim,
                          const uint8_t *restrict src, int src_dim,
                          int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_HIGHPASS, RC_FILTER_FORWARD);
}
#endif
#endif

/**
 *  3x3 highpass filter, absolute value.
 *  The error is within [-0.75, 0.75] ulp.
 */
#if RC_IMPL(rc_filter_highpass_3x3_abs_u8, 0)
#if defined RC_FILTER_3X3_TEMPLATE && defined RC_FILTER_HIGHPASS_ABS && \
    defined RC_FILTER_FORWARD
void
rc_filter_highpass_3x3_abs_u8(uint8_t *restrict dst, int dst_dim,
                              const uint8_t *restrict src, int src_dim,
                              int width, int height)
{
    RC_FILTER_3X3_TEMPLATE(dst, dst_dim, src, src_dim, width, height,
                           RC_FILTER_HIGHPASS_ABS, RC_FILTER_FORWARD);
}
#endif
#endif
