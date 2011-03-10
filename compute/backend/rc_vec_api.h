/*  Copyright (C) 2005-2011, Axis Communications AB, LUND, SWEDEN
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
 *  @file   rc_vec_api.h
 *  @brief  RAPP Vector Abstraction Layer API.
 */

/**
 *  @defgroup vec_api Vector Abstraction Layer
 *
 *  @section vec_api_intro Introduction
 *  The vector abstraction layer provides a platform-independent SIMD API.
 *  It can be implemented using different vector instruction sets.
 *  It is also possible to implement it using SIMD-Within-A-Register,
 *  or SWAR, techniques on native machine words.
 *
 *  The vector implementation of the compute-layer functions uses
 *  only the vector operations defined in the vector interface. This
 *  means that only the vector interface needs to be re-implemented
 *  when porting the library to a new SIMD architecture.
 *
 *  @section vec_api_comp Compliance
 *  A compliant implementation does not need to implement all operations.
 *  The generic vector driver code will only use the operations that the
 *  particular vector implementation defines. However, there is some
 *  mandatory functionality that @e must be present:
 *
 *    - #rc_vec_t:         The vector data type.
 *    - #RC_VEC_SIZE:      The vector size in bytes.
 *    -  RC_VEC_DECLARE(): Global declarations.
 *    -  RC_VEC_CLEANUP(): Global vector state cleanup.
 *    -  RC_VEC_LOAD():    Aligned vector load from memory.
 *    -  RC_VEC_STORE():   Aligned vector store to memory.
 *
 *  @section vec_api_hints Performance Hints
 *  There is a mechanism for passing performance hints to the generic
 *  vector drivers. By defining a hint, the vector implementation
 *  tells the drivers to use a particular operation over a similar
 *  one, when applicable. The hints are:
 *
 *    - #RC_VEC_HINT_AVGT     / #RC_VEC_HINT_AVGR
 *    - #RC_VEC_HINT_CMPGT    / #RC_VEC_HINT_CMPGE
 *    - #RC_VEC_HINT_GETMASKW / #RC_VEC_HINT_GETMASKV
 *
 *  Hints are only needed if more than one alternative is implemented.
 *  Note that presence of alternatives does not imply redundancy.
 *  Finally, it is an error to hint an unimplemented operation,
 *  and also to hint both choices.
 *
 *  @section vec_api_redun Redundancies
 *  Some operations are redundant. This means that there are two vector
 *  operations that can be used interchangeably by the driver code.
 *  The redundant operations are:
 *
 *    - #RC_VEC_CMPGT()    / #RC_VEC_CMPGE()
 *    - #RC_VEC_GETMASKW() / #RC_VEC_GETMASKV()
 *
 *  The RC_VEC_GETMASKV() operation is only useful if also
 *  RC_VEC_ALIGNC(), RC_VEC_SHINIT() and RC_VEC_SHL() are implemented.
 *  The redundant operations use the @ref vec_api_hints "hint" mechanism.
 *
 *  @section vec_api_type Types
 *  Value arguments to operations are of the type rc_vec_t, and pointers
 *  point at uint8_t data, except where otherwise specified.
 *
 *  @section vec_api_impl Implementations
 *  The available implementations are
 *
 *    - rc_vec_mmx.h: Intel MMX.
 *         Uses 64-bit MMX registers.
 *         Implements only a limited subset of the interface.
 *    - rc_vec_sse.h: Intel SSE.
 *         Uses 64-bit MMX registers.
 *         Implements the complete interface.
 *    - rc_vec_sse2.h: Intel SSE2.
 *         Uses 128-bit XMM registers.
 *         Implements most of the interface.
 *    - rc_vec_ssse3.h: Intel SSSE3.
 *         Uses 128-bit XMM registers.
 *         Increases performance of some parts of the SSE2 implementation.
 *    - rc_vec_altivec.h: Freescale AltiVec.
 *         Uses 128-bit vector registers.
 *         Implements the complete interface.
 *    - rc_vec_vadmx.h: Axis VADMX.
 *         Uses 64-bit VADMX registers.
 *         Implements the complete interface.
 *    - rc_vec_swar.h: SIMD-Within-A-Register.
 *         Software emulation on native machine words.
 *         Implements most of the interface.
 *
 *  @section vec_api_port Portability
 *  The SWAR implementation can be compiled with any ISO/ANSI C compiler.
 *  It supports word sizes of 16, 32 and 64 bits. The implementation
 *  is endian-independent. The MMX/SSE* implementations rely on the
 *  presence of the *mmintrin.h intrinsics headers. They are available
 *  on systems using GCC, ICC (Intel) and CL (Microsoft). The AltiVec
 *  implementation uses the altivec.h intrinsics header. VADMX-support
 *  is only available through GCC builtins.
 *
 *  @section vec_api_test Correctness Tests
 *  There are extensive tests available for the vector interface.
 *  The field-wise arithmetic tests are exhaustive, which means
 *  that the tests of operations such as linear interpolation
 *  may take some time on embedded platforms.
 *
 *  <p>@ref build "Next section: Build System"</p>
 *
 * @{
 */

#error The Vector Abstraction Layer documentation header is not to be included

/*
 * -------------------------------------------------------------
 *  Performance hints
 * -------------------------------------------------------------
 */

/**
 *  @name Performance Hints
 *  @{
 */

/**
 *  Hint to use CMPGT instead of CMPGE when possible.
 */
#define RC_VEC_HINT_CMPGT

/**
 *  Hint to use CMPGE instead of CMPGT when possible.
 */
#define RC_VEC_HINT_CMPGE

/**
 *  Hint to use AVGT/SUBHT instead of AVGR/SUBHR when possible.
 */
#define RC_VEC_HINT_AVGT

/**
 *  Hint to use AVGR/SUBHR instead of AVGT/SUBHT when possible.
 */
#define RC_VEC_HINT_AVGR

/**
 *  Hint to use GETMASKW instead of GETMASKV when possible.
 */
#define RC_VEC_HINT_GETMASKW

/**
 *  Hint to use GETMASKV instead of GETMASKW when possible.
 */
#define RC_VEC_HINT_GETMASKV

/*  @} */


/*
 * -------------------------------------------------------------
 *  Vector data type
 * -------------------------------------------------------------
 */

/**
 *  @name Vector Data Type
 *  @{
 */

/**
 *  The vector type definition.
 *  Mandatory.
 */
typedef arch_vector_t rc_vec_t;

/**
 *  The number of bytes in a vector.
 *  Must be a literal constant, @e not something like sizeof(rc_vec_t).
 *  Mandatory.
 */
#define RC_VEC_SIZE

/* @} */


/*
 * -------------------------------------------------------------
 *  Vector state
 * -------------------------------------------------------------
 */

/**
 *  @name Vector State
 *  @{
 */

/**
 *  Global declarations needed for the vector operations.
 *  This can be used to declare vector constants.
 *  Mandatory.
 */
#define RC_VEC_DECLARE()

/**
 *  Global clean up after we are done with the vector computations.
 *  Mandatory.
 */
#define RC_VEC_CLEANUP()

/* @} */


/*
 * -------------------------------------------------------------
 *  Memory access
 * -------------------------------------------------------------
 */

/**
 *  @name Memory Access
 *  @{
 */

/**
 *  Aligned vector load from memory.
 *  The data pointer must be aligned on an #RC_VEC_SIZE boundary.
 *  Mandatory.
 *
 *  @param vec  The loaded vector.
 *  @param ptr  An aligned data pointer to load the vector from.
 */
#define RC_VEC_LOAD(vec, ptr)

/**
 *  Aligned vector store to memory.
 *  The data pointer must be aligned on an #RC_VEC_SIZE boundary.
 *  Mandatory.
 *
 *  @param ptr  An aligned data pointer where to store the vector.
 *  @param vec  The vector to store.
 */
#define RC_VEC_STORE(ptr, vec)

/* @} */


/*
 * -------------------------------------------------------------
 *  Misaligned memory access
 * -------------------------------------------------------------
 */

/**
 *  @name Misaligned Memory Access
 *  @{
 */

/**
 *  Initialize a sequence of misaligned loads.
 *  The output is three intermediate vectors and a data pointer
 *  to be passed to RC_VEC_LOADU(). The input data pointer
 *  must not be aligned on an #RC_VEC_SIZE boundary.
 *  The result of the operation is implementation-specific.
 *  Mandatory if RC_VEC_LOADU() is implemented.
 *
 *  @param vec1  Internal state vector 1.
 *  @param vec2  Internal state vector 2.
 *  @param vec3  Internal state vector 3.
 *  @param uptr  Read data pointer_t.
 *  @param ptr   Misaligned data pointer.
 */
#define RC_VEC_LDINIT(vec1, vec2, vec3, uptr, ptr)

/**
 *  Misaligned vector load from memory.
 *  The intermediate vectors and data pointer from
 *  RC_VEC_LDINIT() are used to load a misaligned vector.
 *  The vector can only be loaded once. To load the next
 *  misaligned vector, the data pointer @e uptr must be
 *  advanced #RC_VEC_SIZE bytes.
 *
 *  @param dstv  The loaded misaligned vector.
 *  @param vec1  Internal state vector 1.
 *  @param vec2  Internal state vector 2.
 *  @param vec3  Internal state vector 3.
 *  @param uptr  Read data pointer.
 */
#define RC_VEC_LOADU(dstv, vec1, vec2, vec3, uptr)

/* @} */


/*
 * -------------------------------------------------------------
 *  Field relocation
 * -------------------------------------------------------------
 */

/**
 *  @name Field Relocation
 *  @{
 */

/**
 *  Initialize a field shift.
 *  Initializes a shift vector to be used for
 *  RC_VEC_SHL() / RC_VEC_SHR() operations.
 *
 *  @param shv    Output shift vector.
 *  @param bytes  The field shift number in the range [0, #RC_VEC_SIZE - 1].
 */
#define RC_VEC_SHINIT(shv, bytes)

/**
 *  Shift all fields to the logical left.
 *
 *  @param dstv   Output vector.
 *  @param srcv   Input vector.
 *  @param shv    Shift vector obtained from RC_VEC_SHINIT().
 */
#define RC_VEC_SHL(dstv, srcv, shv)

/**
 *  Shift fields to the logical right.
 *
 *  @param dstv   Output vector.
 *  @param srcv   Input vector.
 *  @param shv    Shift vector obtained from RC_VEC_SHINIT().
 */
#define RC_VEC_SHR(dstv, srcv, shv)

/**
 *  Shift all fields to the logical left by a constant value.
 *
 *  @param dstv   Output vector.
 *  @param srcv   Input vector.
 *  @param bytes  The field shift constant in the range [0, #RC_VEC_SIZE - 1].
 */
#define RC_VEC_SHLC(dstv, srcv, bytes)

/**
 *  Shift fields to the logical right by a constant value.
 *
 *  @param dstv   Output vector.
 *  @param srcv   Input vector.
 *  @param bytes  The field shift constant in the range [0, #RC_VEC_SIZE - 1].
 */
#define RC_VEC_SHRC(dstv, srcv, bytes)

/**
 *  Align srcv1 and srcv2 to dstv, starting at field @e bytes
 *  into concatenation of srcv1 and srcv2. The alignment
 *  value @e bytes must be a constant.
 *
 *  @param  dstv   Aligned output vector.
 *  @param  srcv1  First source vector.
 *  @param  srcv2  Second source vector.
 *  @param  bytes  The constant alignment shift in the range
 *                 [1, #RC_VEC_SIZE - 1].
 */
#define RC_VEC_ALIGNC(dstv, srcv1, srcv2, bytes)

/**
 *  Pack the even fields of srcv1 and srcv2 into one vector,
 *  with fields 0, ..., #RC_VEC_SIZE/2 - 1 from srcv1 and
 *  fields #RC_VEC_SIZE/2, ..., #RC_VEC_SIZE - 1 from srcv2.
 *
 *  @param  dstv   Packed output vector.
 *  @param  srcv1  First source vector.
 *  @param  srcv2  Second source vector.
 */
#define RC_VEC_PACK(dstv, srcv1, srcv2)

/* @} */


/*
 * -------------------------------------------------------------
 *  Bitwise logical operations
 * -------------------------------------------------------------
 */

/**
 *  @name Bitwise Logical Operations
 *  @{
 */

/**
 *  Set all bits to zero.
 *  Sets vec = 0.
 *
 *  @param vec  The vector to set to zero.
 */
#define RC_VEC_ZERO(vec)

/**
 *  Bitwise NOT.
 *  Computes dstv = ~srcv.
 *
 *  @param dstv  The output vector.
 *  @param srcv  The input vector.
 */
#define RC_VEC_NOT(dstv, srcv)

/**
 *  Bitwise AND.
 *  Computes dstv = srcv1 & srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_AND(dstv, srcv1, srcv2)

/**
 *  Bitwise OR.
 *  Computes dstv = srcv1 | srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_OR(dstv, srcv1, srcv2)

/**
 *  Bitwise XOR.
 *  Computes dstv = srcv1 ^ srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_XOR(dstv, srcv1, srcv2)

/**
 *  Bitwise AND NOT.
 *  Computes dstv = srcv1 & ~srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_ANDNOT(dstv, srcv1, srcv2)

/**
 *  Bitwise OR NOT.
 *  Computes dstv = srcv1 | ~srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_ORNOT(dstv, srcv1, srcv2)

/**
 *  Bitwise XOR NOT.
 *  Computes dstv = srcv1 ^ ~srcv2 for all bits.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_XORNOT(dstv, srcv1, srcv2)

/* @} */


/*
 * -------------------------------------------------------------
 *  Arithmetic operations on 8-bit fields
 * -------------------------------------------------------------
 */

/**
 *  @name Arithmetic Operations
 *  @{
 */

/**
 *  Splat a scalar variable.
 *  Sets the scalar value 'scal' in each 8-bit field.
 *
 *  @param  vec  The output vector.
 *  @param  scal The 8-bit scalar variable to set, any scalar data type.
 */
#define RC_VEC_SPLAT(vec, scal)

/**
 *  Saturating addition.
 *  Computes dstv = min(srcv1 + srcv2, 0xff) for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_ADDS(dstv, srcv1, srcv2)

/**
 *  Average value, truncated.
 *  Computes dstv = (srcv1 + srcv2) >> 1 for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_AVGT(dstv, srcv1, srcv2)

/**
 *  Average value, rounded.
 *  Computes dstv = (srcv1 + srcv2 + 1) >> 1 for each 8-bit field.
 */
#define RC_VEC_AVGR(dstv, srcv1, srcv2)

/**
 *  Average value, rounded towards srcv1.
 *  Computes dstv = (srcv1 + srcv2 + (srcv1 > srcv2)) >> 1
 *  for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_AVGZ(dstv, srcv1, srcv2)

/**
 *  Saturating subtraction.
 *  Computes dstv = max(srvc1 - srcv2, 0) for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_SUBS(dstv, srcv1, srcv2)

/**
 *  Absolute-value subtraction.
 *  Computes dstv = abs(srcv1 - srcv2) for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_SUBA(dstv, srcv1, srcv2)

/**
 *  Half subtraction with bias, truncated.
 *  Computes dstv = (srcv1 - srcv2 + 0xff) >> 1 for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_SUBHT(dstv, srcv1, srcv2)

/**
 *  Half subtraction with bias, rounded.
 *  Computes dstv = (srcv1 - srcv2 + 0x100) >> 1 for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_SUBHR(dstv, srcv1, srcv2)

/**
 *  Absolute value.
 *  Computes dstv = 2*abs(srcv - 0x80) for each 8-bit field.
 *  The result is saturated to [0,0xff].
 *
 *  @param dstv   The output vector.
 *  @param srcv   The input vector.
 */
#define RC_VEC_ABS(dstv, srcv)

/**
 *  Comparison greater-than.
 *  Computes MSB(dst) = srcv1 > srcv2 ? 1 : 0 for each 8-bit field.
 *  Only the most significant bit in each field is set, the remaining
 *  bits are undefined.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_CMPGT(dstv, srcv1, srcv2)

/**
 *  Comparison greater-than-or-equal-to.
 *  Computes MSB(dst) = srcv1 >= srcv2 ? 1 : 0 for each 8-bit field.
 *  Only the most significant bit in each field is set, the remaining
 *  bits are undefined.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_CMPGE(dstv, srcv1, srcv2)

/**
 *  Minimum value.
 *  Computes dstv = min(srcv1, srcv2) for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_MIN(dstv, srcv1, srcv2)

/**
 *  Maximum value.
 *  Computes dstv = max(srcv1, srcv2) for each 8-bit field.
 *
 *  @param dstv   The output vector.
 *  @param srcv1  The first input vector.
 *  @param srcv2  The second input vector.
 */
#define RC_VEC_MAX(dstv, srcv1, srcv2)

/**
 *  Generate the blend vector needed by RC_VEC_LERP().
 *
 *  @param blendv  The output blend vector.
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_BLEND(blendv, blend8)

/**
 *  Linear interpolation.
 *  Computes dstv = srcv1 + ((blend8*(srcv2 - srcv1) + 0x80) >> 8) for each
 *  8-bit field. The Q.8 blend factor @e blend8 must be in the range [0,0x7f].
 *
 *  @param dstv    The output vector.
 *  @param srcv1   The first input vector.
 *  @param srcv2   The second input vector.
 *  @param blendv  The blend vector obtained from RC_VEC_BLEND().
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_LERP(dstv, srcv1, srcv2, blend8, blendv)

/**
 *  Generate the blend vector needed by RC_VEC_LERPZ().
 *
 *  @param blendv  The output blend vector.
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_BLENDZ(blendv, blend8)

/**
 *  Linear interpolation rounded towards srcv1.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded towards zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 *
 *  @param dstv    The output vector.
 *  @param srcv1   The first input vector.
 *  @param srcv2   The second input vector.
 *  @param blendv  The blend vector obtained from RC_VEC_BLENDZ().
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_LERPZ(dstv, srcv1, srcv2, blend8, blendv)

/**
 *  Generate the blend vector needed by RC_VEC_LERPN().
 *
 *  @param blendv  The output blend vector.
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_BLENDN(blendv, blend8)

/**
 *  Linear interpolation rounded towards srcv2.
 *  Computes dstv = srcv1 + (blend8*(srcv2 - srcv1)/256) for each 8-bit
 *  field, with the update term rounded away from zero. The Q.8 blend factor
 *  @e blend8 must be in the range [0,0x7f].
 *
 *  @param dstv    The output vector.
 *  @param srcv1   The first input vector.
 *  @param srcv2   The second input vector.
 *  @param blendv  The blend vector obtained from RC_VEC_BLENDN().
 *  @param blend8  The Q.8 fixed-point scalar blend factor as an unsigned int.
 */
#define RC_VEC_LERPN(dstv, srcv1, srcv2, blend8, blendv)

/* @} */


/*
 * -------------------------------------------------------------
 *  Binary mask operations
 * -------------------------------------------------------------
 */

/**
 *  @name Binary Mask Operations
 *  @{
 */

/**
 *  Get a binary mask word of the most significant bits.
 *  Packs the most significant bits in each 8-bit field to the
 *  physically left-most bits in a binary mask word.
 *  The unused mask bits are set to zero.
 *
 *  @param  maskw  The output mask word as an unsigned int.
 *  @param  vec    The input vector.
 */
#define RC_VEC_GETMASKW(maskw, vec)

/**
 *  Get a binary mask vector of the most significant bits.
 *  Packs the most significant bits in each 8-bit field to the
 *  logically left-most bits in a binary mask vector. The unused
 *  fields in the mask vector are undefined.
 *
 *  @param  maskv  The output mask vector.
 *  @param  vec    The input vector.
 */
#define RC_VEC_GETMASKV(maskv, vec)

/* @} */


/*
 * -------------------------------------------------------------
 *  Reductions
 * -------------------------------------------------------------
 */

/**
 *  @name Reductions
 *  @{
 */

/**
 *  Count bits in all vector fields, iteration count.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_CNTV() and RC_VEC_CNTR() together compute the bit count.
 *  The accumulation step can be iterated at most #RC_VEC_CNTN times
 *  before the reduction step. The value of #RC_VEC_CNTN must be either
 *  one, or be divisible by four.
 */
#define RC_VEC_CNTN

/**
 *  Count bits in all vector fields, accumulation step.
 *
 *  @param  accv  The input/output accumulator vector.
 *  @param  srcv  The input source vector.
 */
#define RC_VEC_CNTV(accv, srcv)

/**
 *  Count bits in all vector fields, reduction step.
 *
 *  @param  cnt   The uint32_t output bitcount value.
 *  @param  accv  The input accumulator to reduce.
 */
#define RC_VEC_CNTR(cnt, accv)

/**
 *  Sum all 8-bit vector fields, iteration count.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_SUMV() and RC_VEC_SUMR() together compute the sum.
 *  The accumulation step can be iterated at most #RC_VEC_SUMN times
 *  before the reduction step. The value of #RC_VEC_SUMN must be either
 *  one, or be divisible by four.
 */
#define RC_VEC_SUMN

/**
 *  Sum all 8-bit vector fields, accumulation step.
 *
 *  @param  accv  The input/output accumulator vector.
 *  @param  srcv  The input source vector.
 */
#define RC_VEC_SUMV(accv, srcv)

/**
 *  Sum all 8-bit vector fields, reduction step.
 *
 *  @param  sum   The uint32_t output sum value.
 *  @param  accv  The input accumulator to reduce.
 */
#define RC_VEC_SUMR(sum, accv)

/**
 *  Multiply and accumulate all 8-bit fields, iteration count.
 *  The format of the accumulator vector is implementation-specific,
 *  but RC_VEC_MACV() and RC_VEC_MACR() together compute the sum.
 *  The accumulation step can be iterated at most #RC_VEC_MACN times
 *  before the reduction step. The value of #RC_VEC_MACN must be either
 *  one, or be divisible by four.
 */
#define RC_VEC_MACN

/**
 *  Multiply and accumulate all 8-bit fields, accumulation step.
 *
 *  @param  accv   The input/output accumulator vector.
 *  @param  srcv1  The first source vector.
 *  @param  srcv2  The second source vector.
 */
#define RC_VEC_MACV(accv, srcv1, srcv2)

/**
 *  Multiply and accumulate all 8-bit fields, reduction step.
 *
 *  @param  mac   The uint32_t output multiply-accumulate value.
 *  @param  accv  The input accumulator to reduce.
 */
#define RC_VEC_MACR(mac, accv)

/* @} */
/* @} */
