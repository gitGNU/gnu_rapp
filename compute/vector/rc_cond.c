/*  Copyright (C) 2016, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_cond.c
 *  @brief  RAPP Compute layer conditional operations,
 *          vector implementation.
 */

#include "rc_impl_cfg.h" /* Tuning. */
#include "rc_vector.h" /* Vector operations. */
#include "rc_util.h" /* RC_DIV_CEIL(). */
#include "rc_cond.h" /* Pixel operation API. */


/*
 * -------------------------------------------------------------
 *  Pixel operation macros.
 * -------------------------------------------------------------
 */

#define RC_PIXOP_COPY(vec1, vec2) ((vec1) = (vec2))

#ifdef RC_VEC_ADDS
#define RC_PIXOP_ADDS(vec1, vec2) RC_VEC_ADDS(vec1, vec1, vec2)
#endif

#ifdef RC_VEC_SUBS
#define RC_PIXOP_SUBS(vec1, vec2) RC_VEC_SUBS(vec1, vec1, vec2)
#endif

/*
 * -------------------------------------------------------------
 *  Template macros.
 * -------------------------------------------------------------
 */

/**
 *  Compute the number of set bits in the map vector, possibly overshooting.
 */
#if defined RC_VEC_ZERO && defined RC_VEC_CNTV && defined RC_VEC_CNTR
#define RC_COND_COUNT(cnt, mapv) \
do {                             \
    (cnt) = 0;                   \
    rc_vec_t countv_;            \
    RC_VEC_ZERO(countv_);        \
    RC_VEC_CNTV(countv_, mapv);  \
    RC_VEC_CNTR(cnt, countv_);   \
} while(0)
#else  /* No bit-count instructions, so assume all bits are set. */
#define RC_COND_COUNT(cnt, mapv) \
do {                             \
    (cnt) = 8 * RC_VEC_SIZE;     \
    (void)mapv;                  \
} while(0)
#endif

/**
 *  Single-operand operation iteration.
 */
#define RC_PIXOP_ITER(buf, pos, vec_, cvec_, pixop, arg1)       \
do {                                                            \
    RC_VEC_LOAD(vec_, &(buf)[(pos)]);                           \
    (cvec_) = (vec_);                                           \
    pixop(vec_, arg1);                                          \
} while (0)

#if defined RC_VEC_SETMASKV && defined RC_VEC_SHLC && defined RC_VEC_ANDNOT \
    && defined RC_VEC_AND && defined RC_VEC_OR && defined RC_VEC_SHLC
/**
 *  Single-operand iteration for a destination operand vector.
 */
#define RC_CONDOP_ITER(buf, mapv, di, pixop, arg1)              \
do {                                                            \
            rc_vec_t dv_, sv_, tv_;                             \
            rc_vec_t exp_mv_, cdv_, cv1_, cv2_;                 \
                                                                \
            /* Run standard pixop. */                           \
            RC_PIXOP_ITER(buf, di, sv_, cdv_, pixop, arg1);     \
                                                                \
            /* Conditional part. */                             \
            RC_VEC_SETMASKV(exp_mv_, mapv);                     \
            RC_VEC_ANDNOT(cv1_, cdv_, exp_mv_);                 \
            RC_VEC_AND(cv2_, sv_, exp_mv_);                     \
            RC_VEC_OR(dv_, cv1_, cv2_);                         \
            RC_VEC_SHLC(tv_, mapv, RC_VEC_SIZE / 8);            \
            mapv = tv_;                                         \
            RC_VEC_STORE(&(buf)[(di)], dv_);                    \
           (di) += RC_VEC_SIZE;                                 \
} while (0)

/**
 *  Single-operand iteration for a map vector.
 *  This is the innermost loop, so this is where we do manual
 *  unrolling.  Max is the constant 8 in the common case, and variable
 *  for the remainder.
 */
#define RC_COND_SINGLE_MAPV_ITER_MAX(max, unroll, buf, map,     \
                                     di, mi, pixop, arg1)       \
do {                                                            \
    rc_vec_t mv_;                                               \
    int k_, cnt_;                                               \
    RC_VEC_LOAD(mv_, &(map)[(mi)]);                             \
    RC_COND_COUNT(cnt_, mv_);                                   \
    if (cnt_ > 0) {                                             \
        int mm_ = (max) / (unroll);                             \
        int mr_ = (max) % (unroll);                             \
        for (k_ = 0; k_ < mm_; k_++) {                          \
            RC_CONDOP_ITER(buf, mv_, di, pixop, arg1);          \
            if ((unroll) >= 2) {                                \
                RC_CONDOP_ITER(buf, mv_, di, pixop, arg1);      \
            }                                                   \
            if ((unroll) == 4) {                                \
                RC_CONDOP_ITER(buf, mv_, di, pixop, arg1);      \
                RC_CONDOP_ITER(buf, mv_, di, pixop, arg1);      \
            }                                                   \
        }                                                       \
                                                                \
        for (k_ = 0; k_ < mr_; k_++) {                          \
            RC_CONDOP_ITER(buf, mv_, di, pixop, arg1);          \
        }                                                       \
    }                                                           \
    else {                                                      \
        (di) += (max) * RC_VEC_SIZE;                            \
    }                                                           \
    (mi) += RC_VEC_SIZE;                                        \
} while (0)

#define RC_COND_SINGLE_MAPV_ITER(unroll, dst, map, di, mi, pixop, arg1) \
    RC_COND_SINGLE_MAPV_ITER_MAX(8, unroll, dst, map, di, mi, pixop, arg1)

/**
 *  Single-operand template.
 *  The caller needs to wrap this in a
 *  RC_VEC_DECLARE() / RC_VEC_CLEANUP() pair.
 */
#define RC_COND_PIXOP_TEMPLATE(dst, dst_dim, map, map_dim,              \
                               width, height, pixop, arg1,              \
                               unroll)                                  \
do {                                                                    \
    /* We use the total number of destination vectors as the base. */   \
    int tot_ = RC_DIV_CEIL((width), RC_VEC_SIZE * 8 / 8);               \
                                                                        \
    /* Split in full and partial map vectors. */                        \
    int len_ = tot_ / 8;                                                \
    int rem_ = tot_ % 8;                                                \
    int y_;                                                             \
                                                                        \
    /* Process all rows. */                                             \
    for (y_ = 0; y_ < (height); y_++) {                                 \
        int i_ = y_ * (map_dim);                                        \
        int j_ = y_ * (dst_dim);                                        \
        int x_;                                                         \
                                                                        \
        /* Process full map vectors. */                                 \
        for (x_ = 0; x_ < len_; x_++) {                                 \
            RC_COND_SINGLE_MAPV_ITER(unroll, dst, map, j_, i_,          \
                                     pixop, arg1);                      \
        }                                                               \
                                                                        \
        /* Handle the remainder. */                                     \
        if (rem_) {                                                     \
            RC_COND_SINGLE_MAPV_ITER_MAX(rem_, unroll, dst, map,        \
                                         j_, i_, pixop, arg1);          \
        }                                                               \
    }                                                                   \
} while(0)

/**
 *  Double-operand iteration.
 */
#define RC_PIXOP_ITER2(dst, src, j, i,          \
                       dv_, cdv_, sv, pixop)    \
do {                                            \
    RC_VEC_LOAD(sv_, &(src)[(i)]);              \
    RC_VEC_LOAD(dv_, &(dst)[(j)]);              \
    (cdv_) = (dv_);                             \
    pixop(cdv_, sv_);                           \
} while (0)

#define RC_CONDOP_ITER2(dst, src, mapv, di, si, pixop, arg1)    \
do {                                                            \
    rc_vec_t dv_, sv_, tv_;                                     \
    rc_vec_t exp_mv_, cdv_, cv1_, cv2_;                         \
                                                                \
    /* Run standard pixop. */                                   \
    RC_PIXOP_ITER2(dst, src, di, si, dv_, cdv_, sv_, pixop);    \
                                                                \
    /* Conditional part. */                                     \
    RC_VEC_SETMASKV(exp_mv_, mapv);                             \
    RC_VEC_ANDNOT(cv1_, dv_, exp_mv_);                          \
    RC_VEC_AND(cv2_, cdv_, exp_mv_);                            \
    RC_VEC_OR(dv_, cv1_, cv2_);                                 \
    RC_VEC_SHLC(tv_, mapv, RC_VEC_SIZE / 8);                    \
    mapv = tv_;                                                 \
    RC_VEC_STORE(&(dst)[(di)], dv_);                            \
    (si) += RC_VEC_SIZE;                                        \
    (di) += RC_VEC_SIZE;                                        \
} while (0)

#define RC_COND_DOUBLE_MAPV_ITER_MAX(max, unroll, dst, src, map,             \
                               j, i, m, pixop)                          \
do {                                                                    \
    rc_vec_t mv_;                                                       \
    int k_, cnt_;                                                       \
    RC_VEC_LOAD(mv_, &(map)[(m)]);                                      \
    RC_COND_COUNT(cnt_, mv_);                                           \
    if (cnt_ > 0) {                                                     \
        int mm_ = (max) / (unroll);                                     \
        int mr_ = (max) % (unroll);                                     \
        for (k_ = 0; k_ < mm_; k_++) {                                  \
            RC_CONDOP_ITER2(dst, src, mv_, j, i, pixop, arg1);          \
            if ((unroll) >= 2) {                                        \
                RC_CONDOP_ITER2(dst, src, mv_, j, i, pixop, arg1);      \
            }                                                           \
            if ((unroll) == 4) {                                        \
                RC_CONDOP_ITER2(dst, src, mv_, j, i, pixop, arg1);      \
                RC_CONDOP_ITER2(dst, src, mv_, j, i, pixop, arg1);      \
            }                                                           \
        }                                                               \
                                                                        \
        for (k_ = 0; k_ < mr_; k_++) {                                  \
            RC_CONDOP_ITER2(dst, src, mv_, j, i, pixop, arg1);          \
        }                                                               \
    }                                                                   \
    else {                                                              \
        (i) += (max) * RC_VEC_SIZE;                                     \
        (j) += (max) * RC_VEC_SIZE;                                     \
    }                                                                   \
    (m) += RC_VEC_SIZE;                                                 \
} while (0)

#define RC_COND_DOUBLE_MAPV_ITER(unroll, dst, src, map, j, i, m, pixop) \
    RC_COND_DOUBLE_MAPV_ITER_MAX(8, unroll, dst, src, map, j, i, m, pixop)

/**
 *  Double-operand template.
 */
#define RC_COND_PIXOP_TEMPLATE2(dst, dst_dim, map, map_dim, src,        \
                               src_dim, width, height, pixop, unroll)   \
do {                                                                    \
    /* We use the total number of destination vectors as the base. */   \
    int tot_ = RC_DIV_CEIL((width), RC_VEC_SIZE * 8 / 8);               \
                                                                        \
    /* Split in full and partial map vectors. */                        \
    int len_ = tot_ / 8;                                                \
    int rem_ = tot_ % 8;                                                \
    int y_;                                                             \
                                                                        \
    RC_VEC_DECLARE();                                                   \
                                                                        \
    /* Process all rows. */                                             \
    for (y_ = 0; y_ < (height); y_++) {                                 \
        int i_ = y_ * (src_dim);                                        \
        int j_ = y_ * (dst_dim);                                        \
        int m_ = y_ * (map_dim);                                        \
        int x_;                                                         \
                                                                        \
        /* Process full map vectors. */                                 \
        for (x_ = 0; x_ < len_; x_++) {                                 \
            RC_COND_DOUBLE_MAPV_ITER(unroll, dst, src, map, j_, i_, m_,      \
                                pixop);                                 \
        }                                                               \
                                                                        \
        /* Handle the remainder. */                                     \
        if (rem_) {                                                     \
            RC_COND_DOUBLE_MAPV_ITER_MAX(rem_, unroll, dst, src, map,        \
                                    j_, i_, m_, pixop);                 \
        }                                                               \
    }                                                                   \
                                                                        \
    RC_VEC_CLEANUP();                                                   \
} while (0)
#endif /* (All required vector operations.) */

/*
 * -------------------------------------------------------------
 *  Single-operand functions
 * -------------------------------------------------------------
 */

/**
 *  Conditionally set pixels to a constant value.
 */
#if RC_IMPL(rc_cond_set_u8, 1)
#if defined RC_COND_PIXOP_TEMPLATE && defined RC_VEC_SPLAT
void
rc_cond_set_u8(uint8_t *restrict dst, int dst_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height, unsigned value)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    RC_VEC_SPLAT(vec, value);
    RC_COND_PIXOP_TEMPLATE(dst, dst_dim, map, map_dim,
                           width, height, RC_PIXOP_COPY, vec,
                           RC_UNROLL(rc_cond_set_u8));
    RC_VEC_CLEANUP();
}
#endif
#endif

/**
 *  Conditionally add unsigned constant.
 */
#if RC_IMPL(rc_cond_addc_u8, 1)
#if defined RC_COND_PIXOP_TEMPLATE && defined RC_PIXOP_ADDS \
    && defined RC_VEC_SPLAT
void
rc_cond_addc_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height, unsigned value)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    RC_VEC_SPLAT(vec, value);
    RC_COND_PIXOP_TEMPLATE(dst, dst_dim, map, map_dim,
                           width, height, RC_PIXOP_ADDS, vec,
                           RC_UNROLL(rc_cond_addc_u8));
    RC_VEC_CLEANUP();
}
#endif
#endif

/**
 *  Conditionally subtract unsigned constant.
 */
#if RC_IMPL(rc_cond_subc_u8, 1)
#if defined RC_COND_PIXOP_TEMPLATE && defined RC_PIXOP_SUBS \
    && defined RC_VEC_SPLAT
void
rc_cond_subc_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height, unsigned value)
{
    rc_vec_t vec;
    RC_VEC_DECLARE();

    RC_VEC_SPLAT(vec, value);
    RC_COND_PIXOP_TEMPLATE(dst, dst_dim, map, map_dim,
                           width, height, RC_PIXOP_SUBS, vec,
                           RC_UNROLL(rc_cond_subc_u8));
    RC_VEC_CLEANUP();
}
#endif
#endif

/*
 * -------------------------------------------------------------
 *  Double-operand functions.
 * -------------------------------------------------------------
 */

/**
 *  Conditionally copy pixels.
 */
#if RC_IMPL(rc_cond_copy_u8, 1)
#if defined RC_COND_PIXOP_TEMPLATE2
void
rc_cond_copy_u8(uint8_t *restrict dst, int dst_dim,
                const uint8_t *restrict src, int src_dim,
                const uint8_t *restrict map, int map_dim,
                int width, int height)
{
    RC_COND_PIXOP_TEMPLATE2(dst, dst_dim, map, map_dim, src, src_dim,
                            width, height, RC_PIXOP_COPY,
                            RC_UNROLL(rc_cond_copy_u8));
}
#endif
#endif

/**
 *  Conditionally add pixels.
 */
#if RC_IMPL(rc_cond_add_u8, 1)
#if defined RC_COND_PIXOP_TEMPLATE && defined RC_PIXOP_ADDS
void
rc_cond_add_u8(uint8_t *restrict dst, int dst_dim,
               const uint8_t *restrict src, int src_dim,
               const uint8_t *restrict map, int map_dim,
               int width, int height)
{
    RC_COND_PIXOP_TEMPLATE2(dst, dst_dim, map, map_dim, src, src_dim,
                            width, height, RC_PIXOP_ADDS,
                            RC_UNROLL(rc_cond_add_u8));
}
#endif
#endif
