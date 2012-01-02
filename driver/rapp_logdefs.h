/*  Copyright (C) 2012, Axis Communications AB, LUND, SWEDEN
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
 *  GENERATED FILE -- ANY CHANGES WILL BE OVERWRITTEN.
 *  See gen-rapp-logmacros.pl.
 */

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_initialize(x)
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_initialize(x)
#define RAPP_LOG_ARGLIST_rapp_initialize
#define RAPP_LOG_ARGSFORMAT_rapp_initialize ""
#define RAPP_LOG_RETFORMAT_rapp_initialize ""

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_terminate(x)
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_terminate(x)
#define RAPP_LOG_ARGLIST_rapp_terminate
#define RAPP_LOG_ARGSFORMAT_rapp_terminate ""
#define RAPP_LOG_RETFORMAT_rapp_terminate ""

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_error(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_error(x) x
#define RAPP_LOG_ARGLIST_rapp_error code
#define RAPP_LOG_ARGSFORMAT_rapp_error "%d"
#define RAPP_LOG_RETFORMAT_rapp_error "%p"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_align(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_align(x) x
#define RAPP_LOG_ARGLIST_rapp_align size
#define RAPP_LOG_ARGSFORMAT_rapp_align "%zu"
#define RAPP_LOG_RETFORMAT_rapp_align "%zu"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_malloc(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_malloc(x) x
#define RAPP_LOG_ARGLIST_rapp_malloc size, hint
#define RAPP_LOG_ARGSFORMAT_rapp_malloc "%zu, %u"
#define RAPP_LOG_RETFORMAT_rapp_malloc "%p"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_free(x)
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_free(x) x
#define RAPP_LOG_ARGLIST_rapp_free ptr
#define RAPP_LOG_ARGSFORMAT_rapp_free "%p"
#define RAPP_LOG_RETFORMAT_rapp_free ""

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixel_get_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixel_get_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_pixel_get_bin buf, dim, off, x, y
#define RAPP_LOG_ARGSFORMAT_rapp_pixel_get_bin "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixel_get_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixel_set_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixel_set_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_pixel_set_bin buf, dim, off, x, y, value
#define RAPP_LOG_ARGSFORMAT_rapp_pixel_set_bin "%p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixel_set_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixel_get_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixel_get_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixel_get_u8 buf, dim, x, y
#define RAPP_LOG_ARGSFORMAT_rapp_pixel_get_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixel_get_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixel_set_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixel_set_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixel_set_u8 buf, dim, x, y, value
#define RAPP_LOG_ARGSFORMAT_rapp_pixel_set_u8 "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixel_set_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_copy_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_copy_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_copy_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_copy_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_copy_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_not_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_not_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_not_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_not_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_not_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_and_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_and_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_and_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_and_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_and_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_or_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_or_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_or_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_or_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_or_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_xor_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_xor_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_xor_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_xor_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_xor_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_nand_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_nand_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_nand_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_nand_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_nand_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_nor_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_nor_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_nor_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_nor_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_nor_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_xnor_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_xnor_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_xnor_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_xnor_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_xnor_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_andn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_andn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_andn_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_andn_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_andn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_orn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_orn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_orn_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_orn_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_orn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_nandn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_nandn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_nandn_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_nandn_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_nandn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_bitblt_norn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_bitblt_norn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_bitblt_norn_bin dst, dst_dim, dst_off, src, src_dim, src_off, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_bitblt_norn_bin "%p, %d, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_bitblt_norn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_set_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_set_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_set_u8 buf, dim, width, height, value
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_set_u8 "%p, %d, %d, %d, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_set_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_not_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_not_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_not_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_not_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_not_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_flip_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_flip_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_flip_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_flip_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_flip_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lut_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lut_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lut_u8 buf, dim, width, height, lut
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lut_u8 "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lut_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_abs_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_abs_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_addc_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_addc_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_addc_u8 buf, dim, width, height, value
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_addc_u8 "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_addc_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lerpc_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lerpc_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lerpc_u8 buf, dim, width, height, value, alpha8
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lerpc_u8 "%p, %d, %d, %d, %u, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lerpc_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lerpnc_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lerpnc_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lerpnc_u8 buf, dim, width, height, value, alpha8
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lerpnc_u8 "%p, %d, %d, %d, %u, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lerpnc_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_copy_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_copy_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_copy_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_copy_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_copy_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_add_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_add_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_add_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_add_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_add_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_avg_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_avg_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_avg_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_avg_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_avg_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_sub_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_sub_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_sub_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_sub_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_sub_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_subh_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_subh_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_subh_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_subh_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_subh_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_suba_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_suba_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_suba_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_suba_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_suba_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lerp_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lerp_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lerp_u8 dst, dst_dim, src, src_dim, width, height, alpha8
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lerp_u8 "%p, %d, %p, %d, %d, %d, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lerp_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lerpn_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lerpn_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lerpn_u8 dst, dst_dim, src, src_dim, width, height, alpha8
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lerpn_u8 "%p, %d, %p, %d, %d, %d, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lerpn_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_lerpi_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_lerpi_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_lerpi_u8 dst, dst_dim, src, src_dim, width, height, alpha8
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_lerpi_u8 "%p, %d, %p, %d, %d, %d, %u"
#define RAPP_LOG_RETFORMAT_rapp_pixop_lerpi_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pixop_norm_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pixop_norm_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pixop_norm_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_pixop_norm_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pixop_norm_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_type_u8_to_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_type_u8_to_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_type_u8_to_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_type_u8_to_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_type_u8_to_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_type_bin_to_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_type_bin_to_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_type_bin_to_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_type_bin_to_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_type_bin_to_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_thresh_gt_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_thresh_gt_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_thresh_gt_u8 dst, dst_dim, src, src_dim, width, height, thresh
#define RAPP_LOG_ARGSFORMAT_rapp_thresh_gt_u8 "%p, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_thresh_gt_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_thresh_lt_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_thresh_lt_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_thresh_lt_u8 dst, dst_dim, src, src_dim, width, height, thresh
#define RAPP_LOG_ARGSFORMAT_rapp_thresh_lt_u8 "%p, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_thresh_lt_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_thresh_gtlt_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_thresh_gtlt_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_thresh_gtlt_u8 dst, dst_dim, src, src_dim, width, height, low, high
#define RAPP_LOG_ARGSFORMAT_rapp_thresh_gtlt_u8 "%p, %d, %p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_thresh_gtlt_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_thresh_ltgt_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_thresh_ltgt_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_thresh_ltgt_u8 dst, dst_dim, src, src_dim, width, height, low, high
#define RAPP_LOG_ARGSFORMAT_rapp_thresh_ltgt_u8 "%p, %d, %p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_thresh_ltgt_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_1x2_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_1x2_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_1x2_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_1x2_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_1x2_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x1_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x1_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x1_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x1_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x1_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x2_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x2_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x2_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x2_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x2_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_1x2_rk1_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_1x2_rk1_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_1x2_rk1_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_1x2_rk1_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_1x2_rk1_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_1x2_rk2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_1x2_rk2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_1x2_rk2_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_1x2_rk2_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_1x2_rk2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x1_rk1_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x1_rk1_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x1_rk1_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x1_rk1_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x1_rk1_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x1_rk2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x1_rk2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x1_rk2_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x1_rk2_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x1_rk2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x2_rk1_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x2_rk1_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x2_rk1_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x2_rk1_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x2_rk1_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x2_rk2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x2_rk2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x2_rk2_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x2_rk2_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x2_rk2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x2_rk3_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x2_rk3_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x2_rk3_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x2_rk3_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x2_rk3_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_reduce_2x2_rk4_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_reduce_2x2_rk4_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_reduce_2x2_rk4_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_reduce_2x2_rk4_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_reduce_2x2_rk4_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_expand_1x2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_expand_1x2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_expand_1x2_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_expand_1x2_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_expand_1x2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_expand_2x1_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_expand_2x1_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_expand_2x1_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_expand_2x1_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_expand_2x1_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_expand_2x2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_expand_2x2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_expand_2x2_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_expand_2x2_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_expand_2x2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rotate_cw_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rotate_cw_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_rotate_cw_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_rotate_cw_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rotate_cw_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rotate_ccw_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rotate_ccw_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_rotate_ccw_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_rotate_ccw_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rotate_ccw_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rotate_cw_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rotate_cw_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_rotate_cw_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_rotate_cw_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rotate_cw_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rotate_ccw_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rotate_ccw_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_rotate_ccw_bin dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_rotate_ccw_bin "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rotate_ccw_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_diff_1x2_horz_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_diff_1x2_horz_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_diff_1x2_horz_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_diff_1x2_horz_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_diff_1x2_horz_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_diff_1x2_horz_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_diff_1x2_horz_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_diff_1x2_horz_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_diff_1x2_horz_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_diff_1x2_horz_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_diff_2x1_vert_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_diff_2x1_vert_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_diff_2x1_vert_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_diff_2x1_vert_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_diff_2x1_vert_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_diff_2x1_vert_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_diff_2x1_vert_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_diff_2x1_vert_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_diff_2x1_vert_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_diff_2x1_vert_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_diff_2x2_magn_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_diff_2x2_magn_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_diff_2x2_magn_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_diff_2x2_magn_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_diff_2x2_magn_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_sobel_3x3_horz_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_sobel_3x3_horz_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_sobel_3x3_horz_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_sobel_3x3_horz_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_sobel_3x3_horz_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_sobel_3x3_horz_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_sobel_3x3_horz_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_sobel_3x3_horz_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_sobel_3x3_horz_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_sobel_3x3_horz_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_sobel_3x3_vert_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_sobel_3x3_vert_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_sobel_3x3_vert_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_sobel_3x3_vert_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_sobel_3x3_vert_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_sobel_3x3_vert_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_sobel_3x3_vert_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_sobel_3x3_vert_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_sobel_3x3_vert_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_sobel_3x3_vert_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_sobel_3x3_magn_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_sobel_3x3_magn_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_sobel_3x3_magn_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_sobel_3x3_magn_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_sobel_3x3_magn_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_gauss_3x3_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_gauss_3x3_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_gauss_3x3_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_gauss_3x3_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_gauss_3x3_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_laplace_3x3_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_laplace_3x3_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_laplace_3x3_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_laplace_3x3_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_laplace_3x3_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_laplace_3x3_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_laplace_3x3_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_laplace_3x3_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_laplace_3x3_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_laplace_3x3_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_highpass_3x3_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_highpass_3x3_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_highpass_3x3_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_highpass_3x3_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_highpass_3x3_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_filter_highpass_3x3_abs_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_filter_highpass_3x3_abs_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_filter_highpass_3x3_abs_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_filter_highpass_3x3_abs_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_filter_highpass_3x3_abs_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_sum_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_sum_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_sum_bin buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_sum_bin "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_sum_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_sum_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_sum_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_sum_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_sum_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_sum_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_sum2_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_sum2_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_sum2_u8 buf, dim, width, height, sum
#define RAPP_LOG_ARGSFORMAT_rapp_stat_sum2_u8 "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_stat_sum2_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_xsum_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_xsum_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_xsum_u8 src1, src1_dim, src2, src2_dim, width, height, sum
#define RAPP_LOG_ARGSFORMAT_rapp_stat_xsum_u8 "%p, %d, %p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_stat_xsum_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_min_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_min_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_min_bin buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_min_bin "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_min_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_max_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_max_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_max_bin buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_max_bin "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_max_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_min_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_min_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_min_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_min_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_min_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_stat_max_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_stat_max_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_stat_max_u8 buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_stat_max_u8 "%p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_stat_max_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_moment_order1_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_moment_order1_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_moment_order1_bin buf, dim, width, height, mom
#define RAPP_LOG_ARGSFORMAT_rapp_moment_order1_bin "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_moment_order1_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_moment_order2_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_moment_order2_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_moment_order2_bin buf, dim, width, height, mom
#define RAPP_LOG_ARGSFORMAT_rapp_moment_order2_bin "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_moment_order2_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_fill_4conn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_fill_4conn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_fill_4conn_bin dst, dst_dim, map, map_dim, width, height, xseed, yseed
#define RAPP_LOG_ARGSFORMAT_rapp_fill_4conn_bin "%p, %d, %p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_fill_4conn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_fill_8conn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_fill_8conn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_fill_8conn_bin dst, dst_dim, map, map_dim, width, height, xseed, yseed
#define RAPP_LOG_ARGSFORMAT_rapp_fill_8conn_bin "%p, %d, %p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_fill_8conn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_align_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_align_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_align_u8 buf, dim, width, height, value
#define RAPP_LOG_ARGSFORMAT_rapp_pad_align_u8 "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_align_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_const_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_const_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_const_u8 buf, dim, width, height, size, value
#define RAPP_LOG_ARGSFORMAT_rapp_pad_const_u8 "%p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_const_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_clamp_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_clamp_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_clamp_u8 buf, dim, width, height, size
#define RAPP_LOG_ARGSFORMAT_rapp_pad_clamp_u8 "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_clamp_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_align_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_align_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_align_bin buf, dim, off, width, height, value
#define RAPP_LOG_ARGSFORMAT_rapp_pad_align_bin "%p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_align_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_const_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_const_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_const_bin buf, dim, off, width, height, size, set
#define RAPP_LOG_ARGSFORMAT_rapp_pad_const_bin "%p, %d, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_const_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_pad_clamp_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_pad_clamp_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_pad_clamp_bin buf, dim, off, width, height, size
#define RAPP_LOG_ARGSFORMAT_rapp_pad_clamp_bin "%p, %d, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_pad_clamp_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_worksize_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_worksize_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_worksize_bin width, height
#define RAPP_LOG_ARGSFORMAT_rapp_morph_worksize_bin "%d, %d"
#define RAPP_LOG_RETFORMAT_rapp_morph_worksize_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_erode_rect_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_erode_rect_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_erode_rect_bin dst, dst_dim, src, src_dim, width, height, wrect, hrect, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_erode_rect_bin "%p, %d, %p, %d, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_erode_rect_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_dilate_rect_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_dilate_rect_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_dilate_rect_bin dst, dst_dim, src, src_dim, width, height, wrect, hrect, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_dilate_rect_bin "%p, %d, %p, %d, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_dilate_rect_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_erode_diam_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_erode_diam_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_erode_diam_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_erode_diam_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_erode_diam_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_dilate_diam_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_dilate_diam_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_dilate_diam_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_dilate_diam_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_dilate_diam_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_erode_oct_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_erode_oct_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_erode_oct_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_erode_oct_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_erode_oct_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_dilate_oct_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_dilate_oct_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_dilate_oct_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_dilate_oct_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_dilate_oct_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_erode_disc_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_erode_disc_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_erode_disc_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_erode_disc_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_erode_disc_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_morph_dilate_disc_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_morph_dilate_disc_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_morph_dilate_disc_bin dst, dst_dim, src, src_dim, width, height, radius, work
#define RAPP_LOG_ARGSFORMAT_rapp_morph_dilate_disc_bin "%p, %d, %p, %d, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_morph_dilate_disc_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_margin_horz_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_margin_horz_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_margin_horz_bin margin, src, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_margin_horz_bin "%p, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_margin_horz_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_margin_vert_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_margin_vert_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_margin_vert_bin margin, src, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_margin_vert_bin "%p, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_margin_vert_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_crop_seek_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_crop_seek_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_crop_seek_bin buf, dim, width, height, pos
#define RAPP_LOG_ARGSFORMAT_rapp_crop_seek_bin "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_crop_seek_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_crop_box_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_crop_box_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_crop_box_bin buf, dim, width, height, box
#define RAPP_LOG_ARGSFORMAT_rapp_crop_box_bin "%p, %d, %d, %d, %p"
#define RAPP_LOG_RETFORMAT_rapp_crop_box_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_contour_4conn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_contour_4conn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_contour_4conn_bin origin, contour, len, buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_contour_4conn_bin "%p, %p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_contour_4conn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_contour_8conn_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_contour_8conn_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_contour_8conn_bin origin, contour, len, buf, dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_contour_8conn_bin "%p, %p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_contour_8conn_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rasterize_4conn(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rasterize_4conn(x) x
#define RAPP_LOG_ARGLIST_rapp_rasterize_4conn line, x0, y0, x1, y1
#define RAPP_LOG_ARGSFORMAT_rapp_rasterize_4conn "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rasterize_4conn "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_rasterize_8conn(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_rasterize_8conn(x) x
#define RAPP_LOG_ARGLIST_rapp_rasterize_8conn line, x0, y0, x1, y1
#define RAPP_LOG_ARGSFORMAT_rapp_rasterize_8conn "%p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_rasterize_8conn "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_cond_set_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_cond_set_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_cond_set_u8 dst, dst_dim, map, map_dim, width, height, value
#define RAPP_LOG_ARGSFORMAT_rapp_cond_set_u8 "%p, %d, %p, %d, %d, %d, %u"
#define RAPP_LOG_RETFORMAT_rapp_cond_set_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_cond_copy_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_cond_copy_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_cond_copy_u8 dst, dst_dim, src, src_dim, map, map_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_cond_copy_u8 "%p, %d, %p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_cond_copy_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_gather_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_gather_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_gather_u8 pack, pack_dim, src, src_dim, map, map_dim, width, height, rows
#define RAPP_LOG_ARGSFORMAT_rapp_gather_u8 "%p, %d, %p, %d, %p, %d, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_gather_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_gather_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_gather_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_gather_bin pack, src, src_dim, map, map_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_gather_bin "%p, %p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_gather_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_scatter_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_scatter_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_scatter_u8 dst, dst_dim, map, map_dim, pack, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_scatter_u8 "%p, %d, %p, %d, %p, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_scatter_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_scatter_bin(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_scatter_bin(x) x
#define RAPP_LOG_ARGLIST_rapp_scatter_bin dst, dst_dim, map, map_dim, pack, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_scatter_bin "%p, %d, %p, %d, %p, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_scatter_bin "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_integral_sum_bin_u8(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_integral_sum_bin_u8(x) x
#define RAPP_LOG_ARGLIST_rapp_integral_sum_bin_u8 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_integral_sum_bin_u8 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_integral_sum_bin_u8 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_integral_sum_bin_u16(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_integral_sum_bin_u16(x) x
#define RAPP_LOG_ARGLIST_rapp_integral_sum_bin_u16 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_integral_sum_bin_u16 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_integral_sum_bin_u16 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_integral_sum_bin_u32(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_integral_sum_bin_u32(x) x
#define RAPP_LOG_ARGLIST_rapp_integral_sum_bin_u32 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_integral_sum_bin_u32 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_integral_sum_bin_u32 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_integral_sum_u8_u16(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_integral_sum_u8_u16(x) x
#define RAPP_LOG_ARGLIST_rapp_integral_sum_u8_u16 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_integral_sum_u8_u16 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_integral_sum_u8_u16 "%d"

#define RAPP_LOG_IDENTITY_IF_RETURN_rapp_integral_sum_u8_u32(x) x
#define RAPP_LOG_IDENTITY_IF_ARGS_rapp_integral_sum_u8_u32(x) x
#define RAPP_LOG_ARGLIST_rapp_integral_sum_u8_u32 dst, dst_dim, src, src_dim, width, height
#define RAPP_LOG_ARGSFORMAT_rapp_integral_sum_u8_u32 "%p, %d, %p, %d, %d, %d"
#define RAPP_LOG_RETFORMAT_rapp_integral_sum_u8_u32 "%d"

#define RAPP_LOG_MAX_RETURN_FORMAT_LENGTH 3
#define RAPP_LOG_MAX_ARGS_FORMAT_LENGTH 34
