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
 *  @file   rapp.h
 *  @brief  RAPP - Raster Processing Primitives, main header.
 */

/**
 *  @mainpage RAPP User's Manual
 *
 *  <div style="margin-left: auto; margin-right: auto; width: 40em;">
 *  <pre>
 *  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
 *  Permission is granted to copy, distribute and/or modify this document
 *  under the terms of the @ref gfdl "GNU Free Documentation License, Version 1.3"
 *  or any later version published by the Free Software Foundation;
 *  with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the section entitled "GNU
 *  Free Documentation License".
 *  </pre>
 *  <!-- Keep the redundant "A copy..." text; consider printed copies. -->
 *  </div>
 *
 *
 *  @section intro Introduction
 *  RAPP is a library of low-level image processing operations,
 *  targeting mainly video analytics and computer vision applications.
 *  The library is meant to provide an optimized and reliable computational
 *  back-end for such algorithms. The interface is designed to allow
 *  hardware-accelerated implementations, while still being simple
 *  enough for easy deployment from higher-level code. Some of the
 *  design aspects are listed below.
 *
 *  - The library is thread-safe.
 *  - Only native data types are used in the interface. This
 *    leaves the higher-level code free to choose its own data
 *    objects and encapsulation without having to re-pack things
 *    for RAPP library calls.
 *  - There are no floating-point types in the interface, and no
 *    floating-point processing is done internally. This is a must
 *    for embedded platforms without FPU.
 *  - All memory is allocated by the caller. This makes it possible
 *    to use special memory, like fast on-chip memory.
 *  - There are restrictions on pixel buffer alignment to enable
 *    hardware acceleration on present and future SIMD platforms.
 *  - There are no I/O channels. It is up to the caller to print
 *    diagnostic messages on error returns and grab and draw images.
 *
 *  @section port Portability
 *  RAPP is designed to be portable @e and efficient on a wide range
 *  of platforms, from embedded systems to SSE3-enabled x86 desktops:
 *
 *  - It is written in ISO C99 (ISO/ANSI C90 plus <stdint.h>).
 *  - It can be compiled for Windows using Cygwin.
 *  - 16/32/64-bit CPUs are supported.
 *  - Both big- and little-endian CPUs are supported.
 *  - There are no external dependencies.
 *  - No floating-point support is needed except for the unit tests.
 *  - SIMD instructions like MMX and SSE* are used if present.
 *  - A build-time tuning system automatically selects the fastest
 *    implementation on different platforms.
 *  - There are extensive unit tests to ensure correctness.
 *
 *  @section contents Contents
 *  - @subpage design
 *  - @subpage definitions
 *  - @subpage support
 *  - @subpage pixelwise
 *  - @subpage statistics
 *  - @subpage filter
 *  - @subpage padding
 *  - @subpage geometry
 *  - @subpage segmentation
 *  - @subpage chaincode
 *  - @subpage conditional
 *  - @subpage integral
 *  - @subpage license
 *
 *  @section Usage
 *  Include the header file containing the RAPP API:
 *
 *<pre>    @#include &lt;rapp/rapp.h&gt; </pre>
 *
 *  Before using RAPP, the library must be initialized.
 *  Use the rapp_initialize() and rapp_terminate() functions for this
 *  purpose. See the section about @ref grp_init "initialization" for more
 *  information.
 *
 *  Link the application against the RAPP library; <code> -lrapp </code>
 *  if using the GNU compiler tools.
 *
 *
 *  @section benchmarks Benchmarks
 *  RAPP is benchmarked on the following platforms:
 *
 *  @htmlinclude benchmarklist.html
 *  <p>@ref design "Next section: Design Principles"</p>
 */

/**
 *  @page design Design Principles
 *  The RAPP library design is based on a few basic principles:
 *
 *  @section design_arg Function Arguments
 *  <em>Only native (C99) types in function arguments.</em>\n
 *  Although the interface may look simpler if images are encapsulated
 *  in structures, it is often more awkward to use that from higher-level
 *  code. Higher-level algorithm code often has its own encapsulated image
 *  object. By forcing the use of a RAPP image object, that code would either
 *  have to use the RAPP image object type also, or convert its own objects
 *  for RAPP function calls.
 *
 *  @section design_buf Buffer Alignment
 *  <em>Images must be aligned in memory.</em>\n
 *  It is often significantly more efficient to process aligned data than
 *  misaligned data. This could be hidden from the user by using separate
 *  internal processing functions dependent on alignment. However, this
 *  would make it easy to unintentionally use slow operations on misaligned
 *  data. An interface that explicitly requires aligned images will force
 *  the user to align the data once and then benefit from the faster
 *  aligned processing functions. Also, the library will contain
 *  much fewer functions to test for correctness, which means less bugs.
 *
 *  @section design_type Pixel Data Types
 *  <em>Binary images are 1 bit per pixel.</em>\n
 *  Binary images are stored as 1 bit-per-pixel bitmaps. In this format it
 *  it more difficult to read and write individual pixels, but the processing
 *  of binary images can be accelerated more than an order of magnitude
 *  in many cases.
 *
 *  <em>8-bit images are unsigned.</em>\n
 *  To keep the number of flavours per operation to a minimum, we want
 *  a single 8-bit data format. 8-bit unsigned integers is the obvious
 *  choice for unsigned data. For signed data, we add a bias of 128 and
 *  use the unsigned data format. With this convention, the processing
 *  functions do not need to know about the how the data is to be
 *  interpreted. To convert a pixel buffer between this unsigned-with-bias
 *  format and the standard two's complement representation, use
 *  rapp_pixop_flip_u8().
 *
 *  @section design_mem Memory Allocation
 *  <em>No internal memory allocation.</em>\n
 *  The caller is responsible for allocating pixel buffers. This makes it
 *  easier to use different memory areas for different buffers, and also
 *  optimize and reuse memory allocations between RAPP calls.
 *
 *  @section design_err Error Handling
 *  <em>Errors are indicated by return value error codes.</em>\n
 *  All RAPP functions must validate its arguments and return an error
 *  code if there is an error. It is up to the user to check this return
 *  value and handle the error.
 *
 *  @section design_maint Maintainability
 *  <em>Favour maintainability over last-percent performance.</em>\n
 *  It might be slightly more optimal to implement platform-specific
 *  assembly functions. This is a nightmare to test and maintain.
 *  The unified vector abstraction layer in RAPP provides a
 *  platform-independent SIMD abstraction that makes it much easier to
 *  implement new functions on @e any SIMD hardware.
 *
 *  <p>@ref definitions "Next section: Concepts and Definitions"</p>
 */

/**
 *  @page definitions Concepts and Definitions
 *  @section naming Naming Convention
 *  All symbols and constants in the API are prefixed with
 *  @c rapp_ and @c RAPP_, respectively. All functions operating on
 *  images with of the same type are suffixed with that type. The type
 *  suffixes are @c _bin for binary images and @c _u8 for 8-bit
 *  unsigned images. The complete naming scheme is shown below:
 *
 *<pre>    {rapp_ | RAPP_} <i>family-name _ name</i> [<i>suffix</i>]</pre>
 *
 *  @section def_coords Coordinate System
 *  The RAPP library uses an image coordinate system with the origin
 *  located in the the upper-left corner of the image. The positive
 *  x-direction is to the right and the positive y-direction is downwards.
 *
 *  @section def_mem Memory Allocation
 *  RAPP functions @em never allocate memory. All memory allocations
 *  must be handled by the caller. However, the library provides an
 *  @ref grp_memory "allocator" that is compatible with the
 *  platform-specific buffer alignment. This allocator may be implemented
 *  on top of malloc() and free().
 *
 *  @section def_err Error Handling
 *  All functions that perform computations return a negative
 *  @ref grp_error "error code" on failure. This code can then be
 *  converted to a description string for diagnostic purposes.
 *  There are no internal failure modes (with the exception of
 *  @ref grp_init "uninitialized"), which means that all errors
 *  are determined by the input argument values. Some functions also
 *  pass the non-negative computation result via the return value.
 *
 *  @section def_type Data Types
 *  @subsection type_bin Binary Images
 *  Binary images are stored in a bit-packed format, i.e. truly one
 *  bit per pixel. In an 8-pixel byte, the individual ordering
 *  left-to-right is 0 &ndash; 7 for little-endian platforms,
 *  and 7 &ndash; 0 for big-endian platforms, where bit 0 is the least
 *  significant bit in the byte. To put it another way, the @e bit order
 *  follows the @e byte order of the platform. There are two major benefits
 *  with this scheme:
 *
 *    -# Binary data can be read in any word width, not just bytes,
 *       and the pixels remain linearly stored in the word.
 *    -# @ref grp_bitblt "Bitblit operations" work on all pixel depths.
 *
 *  @subsection def_u8 8-Bit Images
 *  8-bit images are represented using 8-bit unsigned integers
 *  (@c uint8_t). This does not
 *  imply that data is always unsigned - they are just interpreted
 *  differently. Images with signed data are to be interpreted as
 *  having a constant bias of 128, i.e. the stored value 0 correspond
 *  to -128, and 255 to 127. All functions that produce a signed result
 *  use this encoding. The main reason for using this scheme is that it
 *  essentially halves the number of 8-bit implementations. In this
 *  manual the term "8-bit" will refer to this unsigned 8-bit data type.
 *
 *  @subsection def_code Chain Codes
 *  Chain codes are stored as ASCII characters in NUL-terminated
 *  strings. 4-connectivity chain codes use the characters 0 &ndash; 3
 *  for directions @em right, @em up, @em left and @em down.
 *  8-connectivity chains use 0 &ndash; 7 for directions @em right,
 *  @em up-right, @em up, @em up-left, @em left, @em down-left,
 *  @em down and @em down-right. The figure below illustrates
 *  code interpretation.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 1: Chain code directions for
 *                          4-connectivity (a) and 8-connectivity (b).
 *  </caption>
 *  <tr><td>
 *  <img src="chaincode.png" style="width: 26em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section def_buf Pixel Buffers
 *  @subsection store Storage
 *  Pixel data is stored linearly in memory using row-major order,
 *  from the upper-left to the lower-right corner. For 8-bit images,
 *  the layout in memory is endian-independent. Additionally, for binary
 *  images the bit-order in individual bytes follows the byte-order of the
 *  platform.
 *
 *  @subsection def_img Image Representation
 *  An image is described by four or five parameters for 8-bit images or
 *  binary images, respectively. These parameters are:
 *
 *    -# The pixel buffer pointer, @c buf.
 *    -# The row dimension in bytes, or leading dimension, @c dim.
 *    -# The bit offset 0 &ndash; 7 to the first pixel in the first
 *       byte, @c off. Only for binary images.
 *    -# The image width in pixels, @c width.
 *    -# The image height in pixels, @c height.
 *
 *  The row dimension is the actual size of the image rows and may be
 *  greater than what the width of the image would require. It corresponds
 *  to the increment we need to add to the buffer pointer to get to the pixel
 *  one step below. With an explicit row dimension, it is possible to work
 *  on sub images of a larger original image. The row dimension must always
 *  be aligned, i.e. it must be a multiple of the #rapp_alignment value.
 *
 *  Even though the buffer may start at an arbitrary byte, the @e allocated
 *  memory area must extend to the nearest alignment boundaries. To summarize
 *  the alignment restrictions, the following constraints apply:
 *
 *    -# The allocated memory must start at a #rapp_alignment boundary.
 *    -# The size of the allocated memory must be a multiple of
 *       #rapp_alignment.
 *    -# The row dimension must be a multiple of #rapp_alignment.
 *
 *  Figure 2 shows an example of a binary image buffer.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">
 *    Figure 2: A binary image in memory. For clarity, the rows are shown
 *    stacked on top of each other, even though memory is one-dimensional.
 *    The allocated area is limited by the outer rectangle. The grid
 *    represents individual bytes, so in this example the alignment value
 *    is four bytes. The buffer pointer is offset by one row and six bytes
 *    from the start of the allocated area. The binary offset is four bits,
 *    and combined with the image width, it also determines the bit position
 *    in the last byte in a row. The blue area is the actual image region,
 *    and the gray area is the additional processing region.
 *  </caption>
 *  <tr><td>
 *  <img src="buffer1.png" style="width: 40em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @subsection def_imalign Aligned Images
 *  For performance reasons, one additional alignment restriction is placed
 *  on the images passed to RAPP &ndash; the actual image region must
 *  start on a #rapp_alignment boundary. This implies the following:
 *
 *    - The pixel buffer pointer must be aligned.
 *    - For binary images, the bit offset must be zero.
 *
 *  Images with these properties are called @e aligned images. Unless
 *  explicitly stated, all functions in the RAPP library require
 *  aligned images. Figure 3 shows an example of an aligned image.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 3: An aligned image in memory.
 *  </caption>
 *  <tr><td>
 *  <img src="buffer2.png" style="width: 40em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @subsection Alignment
 *  The alignment value is platform-dependent. It is given by the
 *  constant global variable #rapp_alignment and is a power of two,
 *  at least four bytes. The supplied allocator always returns aligned
 *  memory pointers.
 *
 *  @subsection def_alias Aliasing
 *  Unless explicitly stated, pixel buffers must @e never overlap in
 *  memory. If they do, the result of the computation is undefined.
 *
 *  @subsection def_proc Processing Region
 *  The RAPP functions are allowed to process pixels to the left and
 *  to the right of the actual image region given, limited by the nearest
 *  alignment boundary. While this does not matter much for pixelwise
 *  operations, it may produce erroneous results in other cases if those
 *  alignment pixels are not properly cleared. The functions
 *  rapp_pad_align_bin() and rapp_pad_align_u8() does this for
 *  binary and 8-bit images.
 *
 *  @subsection def_tempbuf Temporary Buffers
 *  Some operations need temporary storage. Since no memory can be
 *  allocated inside a RAPP function, temporary buffers must be allocated
 *  by the caller and passed to the function. The pixel buffer alignment
 *  restrictions apply.
 *
 *  @subsection def_imgsize Image Size
 *  The RAPP functions cannot process arbitrarily large images. The
 *  maximum image size allowed is 2<sup>8*(sizeof(int) - 1)</sup> pixels.
 *  Some functions that perform reductions, e.g. @ref grp_stat "statistics"
 *  and @ref grp_moment "moments" may have additional size restrictions.
 *
 *  <p>@ref support "Next section: Support Functionality"</p>
 */

/**
 *  @page support Support Functionality
 *  @section Overview
 *  Not everything in RAPP process data. There are a few functions
 *  and constants that provide support functionality such as memory
 *  allocation and error handling.
 *
 *  @section Contents
 *  - @ref grp_version
 *  - @ref grp_init
 *  - @ref grp_info
 *  - @ref grp_memory
 *  - @ref grp_error
 *  - @ref grp_pixel
 *
 *  <p>@ref grp_version "Next section: Versioning"</p>
 */

/**
 *  @page pixelwise Pixelwise Operations
 *  @section Overview
 *  Pixelwise operations process pixels in one location at a time.
 *  This is not to be confused with reading and writing @ref grp_pixel
 *  "single pixels"
 *  Furthermore, no @e locality information is used. This means that
 *  the image width and height is only used for determining what
 *  pixels to process, it does not influence the operation itself.
 *  Since no neighbour information is used, pixelwise operations
 *  can easily be used together with the scatter/gather mechanism of
 *  @ref conditional "conditional processing".
 *
 *  @section Contents
 *  The following operations are pixelwise:
 *  - @ref grp_bitblt
 *  - @ref grp_pixop
 *  - @ref grp_thresh
 *  - @ref grp_type
 *
 *  <p>@ref grp_bitblt "Next section: Bitblit Operations"</p>
 */

/**
 *  @page statistics Statistical Operations
 *  @section Overview
 *  The statistical operations compute image-global statistical measures
 *  for one or two source images.
 *
 *  @section Contents
 *  - @ref grp_stat
 *  - @ref grp_moment
 *
 *  <p>@ref grp_stat "Next section: Sum, Sum-of-Squares, Min and Max Statistics"</p>
 */

/**
 *  @page filter Spatial Filtering
 *  @section Overview
 *  The spatial filtering functions read source pixels in a @e neighbourhood
 *  around the corresponding destination pixel being computed. The size of
 *  this neighbourhood depends on the size of the filter window. At the edges
 *  of the image, source image data will read @e outside the supplied area.
 *  It is the caller's responsibility to ensure that this data is valid,
 *  i.e. allocated and set to the appropriate values. The @ref grp_pad
 *  "padding" functions can be used for this. The row dimension of the
 *  source image must be large enough to span also the padding. The destination
 *  image is always of the same size as the source image (excluding padding).
 *
 *  @section Contents
 *  - @ref grp_filter
 *  - @ref grp_binmorph
 *
 *  <p>@ref grp_filter "Next section: Fixed-Filter Convolutions"</p>
 */

/**
 *  @page padding Border Padding
 *  @section Overview
 *  The padding functions set pixels in a border @e outside the image
 *  area. It is the caller's responsibility to ensure that the border
 *  is within the allocated area. There are three different kinds of
 *  padding functions:
 *
 *    -# Set all left/right border pixels up to the
 *       nearest alignment boundary to a value (align).
 *    -# Set all border pixels to a constant value (const).
 *    -# Set all border pixels to the value of the image edge (clamp).
 *
 *  The first variant is used to set all alignment pixels so that
 *  functions reading them produce a correct result. The other two are used
 *  to set the pad pixels around an image for functions that need padding, e.g.
 *  @ref filter "spatial filters". Note that the number of pad pixels to
 *  the left and right that are actually set is <em>at least</em> the
 *  specified number. The functions may set all pad pixels up to the nearest
 *  alignment boundaries.
 *
 *  @section Contents
 *  - @ref grp_pad
 *
 *  <p>@ref grp_pad "Next section: Padding"</p>
 */

/**
 *  @page geometry Geometrical Transformations
 *  @section Overview
 *  These functions alter the geometry of an image. The image width
 *  and height given always refer to the source image.
 *
 *  @section Contents
 *  - @ref grp_reduce
 *  - @ref grp_expand
 *  - @ref grp_rotate
 *  - @ref grp_margin
 *
 *  <p>@ref grp_reduce "Next section: Spatial Reduction"</p>
 */

/**
 *  @page segmentation Segmentation and Labelling
 *  @section Overview
 *  These functions work on binary data only. They detect regions of
 *  non-zero pixels in the image. The alignment pixels must be cleared
 *  when calling any of these functions, otherwise the result will
 *  not be correct. The performance is input-sensitive.
 *
 *  @section Contents
 *  - @ref grp_crop
 *  - @ref grp_fill
 *
 *  <p>@ref grp_crop "Next section: Cropping"</p>
 */

/**
 *  @page chaincode Chain Codes
 *  @section Overview
 *  Chain codes are represented as NUL-terminated ASCII strings, using
 *  the characters 0 &ndash; 7, see the figure below.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 1: Chain code directions for
 *                          4-connectivity (a) and 8-connectivity (b).
 *  </caption>
 *  <tr><td>
 *  <img src="chaincode.png" style="width: 26em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section Contents
 *  - @ref grp_contour
 *  - @ref grp_rasterize
 *
 *  <p>@ref grp_contour "Next section: Contour Chain Code Generation"</p>
 */

/**
 *  @page conditional Conditional Operations
 *  @section Introduction
 *  When working with more complex analytics/vision algorithms, it is
 *  necessary to process only a @e selection of pixels. Typical
 *  applications use a cascade of algorithms. First, we start with simple
 *  algorithm that processes the entire image. It is fast but may produce
 *  many false positives. The parts of the image that pass this
 *  quick-rejection test move on to the next processing step, and so on.
 *  As we move up the ladder of algorithm complexity, the data processed
 *  becomes more and more sparse. There are two major problems with this:
 *
 *    -# Sparse data cannot be processed efficiently with SIMD technology.
 *    -# Mixing arithmetic with conditional branching in the inner loop
 *       kills performance.
 *
 *  The gather/scatter mechanism of conditional processing aims to
 *  solve this issue.
 *
 *  @section cond_gs The Gather/Scatter Mechanism
 *  The gather/scatter mechanism is a generic framework for conditional
 *  pixel processing. It allows the user to
 *
 *    -# Increase performance on sparse images by not processing all pixels.
 *    -# Alter a selection of pixels and leave the rest unchanged.
 *    -# Leverage the power of SIMD-accelerated processing functions.
 *
 *  The basic concept is to decouple the @e selection of pixels from
 *  the actual processing. The @e gather and @e scatter functions
 *  handle this selection, while most of the other functions in this
 *  library do the processing.
 *
 *  @section cond_theory The Theory of Gather and Scatter
 *
 *  @subsection cond_basic Basic Definitions
 *
 *  We start by defining an @e ordering function for the coordinates
 *  in a two-dimensional image, \f$ r = r(x, y) \f$. It has the following
 *  properties:
 *
 *    -# It is invertible.
 *    -# It is strictly increasing in x,
 *       i.e. \f$ r(x + 1, y) > r(x, y) \f$.
 *
 *  Define a function \f$ [f_x, f_y] = f(\mathbf{m}, k) \f$ to be the
 *  x and y coordinates of the <em>k</em>:th non-zero ordered pixels in
 *  a binary map image <b>m</b>. Define @e h as
 *
 *  \f[
 *      h(\mathbf{m}, x, y) = \sum_{r(i,j) < r(x,y)} m_{ij},
 *  \f]
 *
 *  i.e. the sum of all ordered map pixels up to, but not including,
 *  the pixel at x, y. Finally, define <em>n</em>(<b>m</b>) as the
 *  sum of all pixels in the binary map:
 *
 *  \f[
 *      n(\mathbf{m}) = \sum_{ij} m_{ij}.
 *  \f]
 *
 *  Now we can define the @e gather function:
 *
 *  \f[
 *      G(\mathbf{u}, \mathbf{m}) = [u_{f(\mathbf{m}, 1)},
 *                                   u_{f(\mathbf{m}, 2)}, \dots,
 *                                   u_{f(\mathbf{m}, n(\mathbf{m}))}],
 *  \f]
 *
 *  where <b>u</b> is an arbitrary @e w x @e h image.
 *  The dimension of @e G is 1 x <em>n</em>(<b>m</b>).
 *
 *  The gather function \f$ G \f$ is invertible for all non-zero
 *  pixels in the map. The @e scatter function is such an inverse
 *  in that sense:
 *
 *  \f[
 *      S(\mathbf{p}, \mathbf{m}) = [ m_{xy} p_{h(\mathbf{m},x,y)} ]_{xy},
 *  \f]
 *
 *  where <b>p</b> is an arbitrary 1 x <em>n</em>(<b>m</b>) vector.
 *
 *  The gather function can be seen as @e compressing an @e w x @e h
 *  image into a 1 x <em>n</em>(<b>m</b>) image, with the map
 *  <b>m</b> as a key. The scatter function then restores
 *  the compressed pixels, using <em>the same map</em>. If our
 *  image processing is independent of the raster position and does not
 *  use any neighbouring pixels, we can perform the processing in the
 *  compressed domain. Examples of operations are pixelwise arithmetics,
 *  thresholding, pixel variance and histograms. Figures 1 and 2
 *  illustrate the basic gather and scatter operations.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 1: The Gather operation.</caption>
 *  <tr><td>
 *  <img src="gather.png" style="width: 30em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 2: The Scatter operation.</caption>
 *  <tr><td>
 *  <img src="scatter.png" style="width: 30em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section cond_neighbour Neighbourhood Access
 *  The gather/scatter mechanism can be extended for use with operations
 *  that require a local neighbourhood of pixels, e.g. spatial filtering
 *  operations. We describe the technique for processing a 3x3 neighbourhood,
 *  but the concept holds for arbitrary window sizes.
 *
 *  Let <b>m</b><sub>0</sub> denote the binary map image. We dilate this map
 *  with a 1x3 structuring element, producing a new map <b>m</b><sub>1</sub>.
 *  This essentially pads the non-zero pixels in the horizontal direction.
 *  Next, we perform @e three gather operations with the dilated map,
 *  starting from the three top rows of the source image. We store
 *  the gathered data in three different rows in the @e pack image
 *  <b>p</b> of size 3 x <em>n</em>(<b>m</b><sub>1</sub>).
 *  For all pixels designated by the original binary map, the neighbours
 *  are available in the pack image on exactly the same relative positions as
 *  in the source image. We can now run our 3x3 spatial operations on
 *  the middle row of the pack image.
 *
 *  The processing of the pack image will produce correct results only
 *  for the original pixels of interest. If we scatter the
 *  1 x <em>n</em>(<b>m</b><sub>1</sub>) result back to a destination image,
 *  also the invalid values for the pad pixel will be written. There are
 *  two ways to fix this problem:
 *
 *    -# Clear the pad pixels after scattering, using XOR of the
 *       map and the padded map.
 *    -# Scatter only the relevant pixels.
 *
 *  The first alternative is probably best if the pixels to scatter
 *  are binary data, i.e. the processing ends in some threshold
 *  operation.
 *
 *  The second approach is more general. We create a @e residual map
 *  <b>m</b><sub>01</sub> by gathering <b>m</b><sub>0</sub> with
 *  <b>m</b><sub>1</sub>,
 *
 *  \f[
 *      \mathbf{m}_{01} = G(\mathbf{m}_0, \mathbf{m}_1).
 *  \f]
 *
 *  It is a map of the extra pixels that need to be gathered
 *  when translating gatherings with <b>m</b><sub>1</sub> to
 *  <b>m</b><sub>0</sub>, i.e.
 *
 *  \f[
 *      G(\mathbf{u}, \mathbf{m}_0) =
 *         G(G(\mathbf{u}, \mathbf{m}_1), \mathbf{m}_{01}).
 *  \f]
 *
 *  With this map, we can gather the result pixels to a new, smaller
 *  pack buffer, and then scatter them to the destination image using
 *  the original map <b>m</b><sub>0</sub>. Figure 3 shows an example of
 *  the 3x3 processing.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">
 *    Figure 3: A 3x3 neighbourhood example.
 *    Starting from the left on the first row, we have the source image
 *    <b>u</b> where the light blue pixels are the pixels of interest.
 *    The image <b>m<sub>0</sub></b> is the map image, designating the
 *    the pixels of interest in the source image. First, we perform a 1x3
 *    binary dilation on the map image, producing a new map
 *    <b>m<sub>1</sub></b>. Next, we perform three gathering operations with
 *    the map <b>m<sub>1</sub></b>, starting on the first three rows of the
 *    image <b>u</b>. The result is written to the three rows of the image
 *    <b>p</b>. Note how every blue pixel in the middle row are surrounded
 *    their 3x3 neighbours from the source image. On the image <b>p</b> we
 *    can now compute our 3x3 neighbourhood result for this middle row.
 *    It will be correct only for the blue pixels. To gather the correct
 *    result, we create the residual map <b>m<sub>01</sub></b> by gathering
 *    <b>m<sub>0</sub></b> with <b>m<sub>1</sub></b>. The result of the
 *    3x3 operation is then gathered by this map, producing the result in
 *    the last row. This is the correct result of the 3x3 operation on the
 *    original image, gathered by the original map <b>m<sub>0</sub></b>.
 *  </caption>
 *  <tr><td>
 *  <img src="gather3.png" style="width: 50em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section Performance
 *  The performance gain of using the scatter/gather technique is highly
 *  dependent on the density of the map image and which operations
 *  are performed. For the basic scatter/gather use case, we get the
 *  saving in processing cost
 *
 *  \f[
 *      \Delta C =  (w h - n) C_{proc} - C_{GS},
 *  \f]
 *
 *  where
 *  \f$ C_{proc} \f$ is the per-pixel processing cost,
 *  \f$ C_{GC}   \f$ is the cost of the gather and scatter operations,
 *  @e n             is the density of the binary map
 *                   (number of non-zero pixels), and
 *  @e w, @e h       is the image size.
 *
 *  It is obvious that the processing term will eventually dominate over
 *  the gather/scatter term as long as not all map pixels are set, given
 *  a high enough cost of processing. This is expected &ndash; the cost
 *  of gather and scatter will be amortized over the total processing
 *  performed in the compressed domain.
 *
 *  What is not obvious is that the cost of gather/scatter is also
 *  dependent on the map density @e n. If the map is sparse, the
 *  zero pixels in it will be discarded rapidly. However, this just alters the
 *  break-even threshold a bit, making it feasible to use the gather/scatter
 *  mechanism in cases of lightweight processing and sparse maps.
 *
 *  For the neighbourhood processing we get similar results. The map density
 *  of interest is now the density of the @e padded map.
 *
 *  @section Usage
 *  In general, the cost saving is dependent on the cost of processing,
 *  and the layout of the map image in a non-trivial way. However, modelling
 *  it as a function of @e n is a reasonable approximation. The typical
 *  usage is as follows:
 *
 *    -# Compute the number of pixels in the map, using rapp_stat_sum_bin().
 *    -# If the number of pixels is less than a constant threshold,
 *       perform gathering.
 *    -# Perform the processing on either the full images, or the pack images.
 *    -# Scatter the result if packed.
 *
 *  The threshold is processing and platform dependent, and must be set
 *  to a reasonable value by profiling the algorithm.
 *
 *  The gather/scatter mechanism can be used in cascade. This means that
 *
 *    -# The scattered binary output from one algorithm can serve as the
 *       map for the next algorithm in the chain.
 *    -# If two cascaded algorithms use the same source data, it is not
 *       necessary to scatter the binary output of the first one back to the
 *       image domain. We can simply gather the compressed domain directly,
 *       which can cut the cost of the gather/scatter operations substantially.
 *
 *  @section Contents
 *  - @ref grp_cond
 *  - @ref grp_gather
 *  - @ref grp_scatter
 *
 *  <p>@ref grp_cond "Next section: Conditional Set and Copy"</p>
 */

/**
 *  @page integral Integral Images
 *  @section Overview
 *  The positions in the integral image represent the sum of all pixels above
 *  and to the left of the pixel in the source image.
 *  The integral image is calculated in one pass over the image.
 *
 *  The destination buffers need to be padded with at least 1 pixel above and
 *  a rapp_alignment to the left of the image start. The border pixels have to
 *  be properly @ref padding "cleared". <!-- While not necessary at the
 *  moment, a future SIMD implementation would surely want to read and
 *  include the whole rc_vec_t to the left. -->
 *
 *  @section Usage
 *  The table can be used for calculating the integral image sum of any
 *  rectangle in the source image as
 *
 *  \f[
 *      S = table(C) - table(B) - table(D) + table(A)
 *  \f]
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 1: Calculating the integral sum over a
 *  subsection.</caption>
 *  <tr><td>
 *  <img src="integral.png" style="width: 25em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section Contents
 *  - @ref grp_integral
 *
 *  <p>@ref grp_integral "Next section: Integral Images"</p>
 *
 */

/**
 *  @page license License
 *  @section License
 *
 *  This document by itself is subject to the GNU Free Documentation
 *  License as mentioned in the note before the @ref intro "introduction",
 *  while the rest of RAPP is licensed as below.
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
 *  @section lgpl GNU Lesser General Public License
 *
 *  @verbinclude COPYING.LESSER
 *
 *  @section gpl GNU General Public License
 *
 *  This text is included for completeness, only because the @ref lgpl
 *  refers to it; no part of RAPP is covered by the GPL alone.
 *
 *  @verbinclude COPYING
 *
 *  @section gfdl GNU Free Documentation License
 *
 *  @verbinclude COPYING.FDL
 */

#ifndef RAPP_H
#define RAPP_H

/*
 * Define 'restrict' as empty to make it possible to use RAPP
 * with a non-C99 compiler. This file is never included from
 * within the RAPP library.
 */
#if !(__STDC_VERSION__ >= 199901L)
#undef  restrict
#define restrict
#endif

#include "rapp_main.h"        /* Library initialization        */
#include "rapp_version.h"     /* Versioning                    */
#include "rapp_info.h"        /* Build information             */
#include "rapp_pixel.h"       /* Pixel access                  */
#include "rapp_error.h"       /* Error handling                */
#include "rapp_malloc.h"      /* Memory allocation             */
#include "rapp_bitblt.h"      /* Bitblit operations            */
#include "rapp_pixop.h"       /* Pixelwise operations          */
#include "rapp_type.h"        /* Type conversions              */
#include "rapp_thresh.h"      /* Thresholding to binary        */
#include "rapp_reduce.h"      /* 8-bit 2x reduction            */
#include "rapp_reduce_bin.h"  /* Binary 2x reduction           */
#include "rapp_expand_bin.h"  /* Binary 2x expansion           */
#include "rapp_rotate.h"      /* 8-bit image rotation          */
#include "rapp_rotate_bin.h"  /* Binary image rotation         */
#include "rapp_stat.h"        /* Statistical operations        */
#include "rapp_moment_bin.h"  /* Binary image moments          */
#include "rapp_filter.h"      /* Fixed-filter convolutions     */
#include "rapp_fill.h"        /* Connected-components fill     */
#include "rapp_pad.h"         /* 8-bit image padding           */
#include "rapp_pad_bin.h"     /* Binary image padding          */
#include "rapp_morph_bin.h"   /* Binary morphology             */
#include "rapp_margin.h"      /* Binary image logical margins  */
#include "rapp_crop.h"        /* Binary image cropping         */
#include "rapp_contour.h"     /* Contour chain codes           */
#include "rapp_rasterize.h"   /* Chain code line rasterization */
#include "rapp_cond.h"        /* Conditional operations        */
#include "rapp_gather.h"      /* 8-bit pixel gather            */
#include "rapp_gather_bin.h"  /* Binary pixel gather           */
#include "rapp_scatter.h"     /* 8-bit pixel scatter           */
#include "rapp_scatter_bin.h" /* Binary pixel scatter          */
#include "rapp_integral.h"    /* Integral images               */

#if !(__STDC_VERSION__ >= 199901L)
#undef restrict
#endif

#endif /* RAPP_H */
