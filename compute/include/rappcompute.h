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
 *  @file   rappcompute.h
 *  @brief  RAPP Compute layer exported API.
 */

/**
 *  @mainpage RAPP Developer's Manual
 *
 *  <div style="margin-left: auto; margin-right: auto; width: 40em;">
 *  <pre>
 *  Copyright (C) 2010, Axis Communications AB, LUND, SWEDEN
 *  Permission is granted to copy, distribute and/or modify this document
 *  under the terms of the GNU Free Documentation License, Version 1.3
 *  or any later version published by the Free Software Foundation;
 *  with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the section entitled "GNU
 *  Free Documentation License".
 *  </pre>
 *  </div>
 *
 *
 *  @section overview Design Overview
 *  The RAPP library is divided into two distinct parts &ndash; the
 *  API layer and the Compute layer. The API layer handles argument
 *  validation and error handling, and then call the low-level functions
 *  in the Compute layer, where the actual processing takes place.
 *
 *  The Compute layer can implement a function in two versions, a
 *  @e generic version and a @e vector version. The vector version
 *  uses a platform-specific implementation of the <em>Vector Abstraction
 *  Layer</em> (VAL). This way it is possible to support a broad range
 *  of SIMD architectures with a minimal amount of development,
 *  maintenance and debugging work.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 1: Design Overview.</caption>
 *  <tr><td>
 *  <img src="design.png" style="width: 30em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section Tuning
 *  Implementations are parameterized by a few compile-time constants.
 *  The fastest implementations and parameters are selected
 *  automatically by a tuning process when the library is built.
 *  RAPP is tuned on the following platforms:
 *
 *  @htmlinclude tunelist.html
 *
 *  @section Contents
 *  The following sections describe the contents of RAPP in more detail.
 *
 *  - @subpage api
 *  - @subpage compute
 *  - @ref     vec_api
 *  - @subpage build
 *  - @subpage tune
 *  - @subpage test
 *  - @subpage perf
 *  - @subpage license
 *
 *  <p>@ref api "Next section: API Layer"</p>
 */

/**
 *  @page api API Layer
 *  The API layer performs argument validation and error handling,
 *  split operations into simpler components, and finally dispatches
 *  the low-level Compute functions that perform the actual number
 *  crunching. The API-level functions are called @e drivers for
 *  the low-level operations. In many cases the low-level interface
 *  provides small pieces that together make an API function. The rationale
 *  behind this is to separate the parts that are platform-dependent
 *  from the parts that are not. The things belonging to the
 *  API layer can be separated into three distinct categories:
 *
 *    -# Argument validation and error handling.
 *    -# Identifying simple or trivial cases (argument reduction).
 *    -# Algorithmic parts of the operation, such as decomposition of
 *       morphological operations, block processing and
 *       alignment-dependent selection of low-level processing functions.
 *
 *  Everything that should be implemented differently on different
 *  platforms, i.e. SSE and non-SSE, belong to the low-level Compute domain.
 *  For more information about the RAPP API, see the RAPP User's Manual.
 *
 *  <p>@ref compute "Next section: Compute Layer"</p>
 */

/**
 *  @page compute Compute Layer
 *  @section Overview
 *  The Compute layer contains the functions that perform the actual
 *  processing. All functions in this layer are prefixed with @c rc_
 *  for RAPP Compute, separating them from the API functions that
 *  all start with @c rapp_. A function may have two different
 *  implementations, a @e generic and a @e vector version.
 *  The generic implementation can run on any platform, but the vector
 *  version requires a platform-specific vector @ref vec_api "backend".
 *
 *  @section common Common Functionality
 *  There is a small layer of hardware abstractions to shield the
 *  implementations from all the platform-specific details:
 *
 *    - rc_platform.h: Platform-specific constants such as native
 *                     word size, endianness and alignment.
 *    - rc_stdbool.h:  Portable C99 stdbool.h.
 *    - rc_word.h:     Word operations independent of endianness and
 *                     native word size.
 *
 *  There is also some platform-independent functionality:
 *
 *    - rc_util.h:  Common utilities such as MIN(), MAX(), CLAMP().
 *    - rc_table.h: Static lookup tables used in more than one place.
 *
 *  The common functionality is located in the @c compute/common directory,
 *  except rc_platform.h and rc_stdbool.h that are exported to the
 *  API layer, and thus reside in @c compute/include.
 *
 *  @section export Compute API
 *  All headers for the Compute layer API are located in the
 *  @c compute/include directory. The header rappcompute.h includes
 *  everything that is exported, so the API layer should only include
 *  this file.
 *
 *  The interface of the Compute layer is:
 *    - rc_stdbool.h:     Portable C99 stdbool.h.
 *    - rc_platform.h:    Platform-specific constants.
 *    - rc_malloc.h:      Aligned memory allocator.
 *    - rc_bitblt_wm.h:   Bitblit, word-misaligned (bit-level).
 *    - rc_bitblt_wa.h:   Bitblit, word-aligned.
 *    - rc_bitblt_vm.h:   Bitblit, vector-misaligned (byte-level).
 *    - rc_bitblt_va.h:   Bitblit, vector-aligned.
 *    - rc_pixop.h:       Pixelwise arithmetic operations.
 *    - rc_type.h:        Type conversions.
 *    - rc_thresh.h:      Thresholding.
 *    - rc_stat.h:        Sum and sum-of-squares statistics.
 *    - rc_moment_bin.h:  Binary image moments.
 *    - rc_filter.h:      Fixed-filter convolutions.
 *    - rc_morph_bin.h:   Binary morphology primitives.
 *    - rc_pad.h:         8-bit padding.
 *    - rc_pad_bin.h:     Binary padding.
 *    - rc_reduce.h:      8-bit 2x spatial reduction.
 *    - rc_reduce_bin.h:  Binary 2x spatial reduction.
 *    - rc_expand_bin.h:  Binary 2x spatial expansion.
 *    - rc_rotate.h:      8-bit 90 degree rotation.
 *    - rc_rotate_bin.h:  Binary 90 degree rotation.
 *    - rc_margin.h:      Binary image logical margins.
 *    - rc_crop.h:        Binary region cropping.
 *    - rc_fill.h:        Connected-components seed fill.
 *    - rc_contour.h:     Contour chain code generation.
 *    - rc_rasterize.h:   Chain code line rasterization.
 *    - rc_cond.h:        Conditional pixelwise operations.
 *    - rc_gather.h:      Conditional 8-bit gather.
 *    - rc_gather_bin.h:  Conditional binary gather.
 *    - rc_scatter.h:     Conditional 8-bit scatter.
 *    - rc_scatter_bin.h: Conditional binary scatter.
 *    - rc_integral.h:     8-bit to 8, 16, and 32-bit integral images.
 *    - rc_integral_bin.h: Binary to 8, 16, and 32-bit integral images.
 *
 *  @section principles Implementation Principles
 *  Both the generic and the vector implementations follow two basic rules:
 *
 *    -# Minimize the amount of redundant code.
 *    -# Minimize the use of conditional compilations with @c @#ifdef.
 *
 *  This has two implications. First, preprocessor macros are used a lot.
 *  In the word and vector interfaces, it is for efficiency reasons
 *  (inlining), but in the actual function implementations they serve the
 *  purpose of @e templates. These template macros perform everything
 *  in common for a family of functions, and accept other macros
 *  as arguments for altering the actual computation performed in the
 *  inner loop. For example, the generic implementation of the
 *  double-operand @ref rc_pixop.h "pixelwise arithmetic" operations
 *  are almost identical for all functions &ndash; they differ only in
 *  the arithmetic operation performed. The template macros are usually
 *  private to the source file, but the thresholding templates are not
 *  since they are used for both thresholding and u8-to-binary conversion.
 *
 *  The second implication is the use of conditional @c if/else instead
 *  of preprocessor @c @#ifdef. We rely on the compiler to optimize out
 *  branches that are never taken, i.e. the condition is a compile-time
 *  constant. Usually the conditions are related the word and vector
 *  sizes, and also to loop unrolling factors.
 *
 *  @section generic Generic Implementation
 *  The generic implementations are located in the directory
 *  @c compute/generic. They have access to all the @ref common "common"
 *  functionality. To make things easier to test and maintain, one
 *  should try to avoid using the RC_BIG_ENDIAN/#RC_LITTLE_ENDIAN
 *  constants, and instead rely on the @ref rc_word.h "word interface".
 *
 *  All functions in the Compute layer API @e must have a generic
 *  implementation as a fallback, with one exception. The
 *  @ref rc_bitblt_va.h "vector-aligned" and
 *  @ref rc_bitblt_vm.h "vector-misaligned" bitblits are only available
 *  in a vector version, since they can be implemented with the
 *  soft-SIMD (SWAR) vector backend on all platforms. Using soft-SIMD,
 *  they essentially degenerate into their word-aligned equivalents.
 *
 *  @section vector Vector Implementation
 *  The vector implementations are located in the directory
 *  @c compute/vector. They also use the @ref common "common"
 *  functionality as the generic implementations, but also have access
 *  to the @ref vec_api "vector interface". An implementation of this
 *  interface is called a vector @e backend.
 *
 *  The vector interface is not restricted to SIMD-capable architectures.
 *  There is a soft-SIMD backend that emulates SIMD operations
 *  in software, on top of the @ref rc_word.h "word interface".
 *
 *  A particular vector backend may not implement all operations
 *  specified by the interface. The vector implementations must
 *  therefor protect the functions using @#ifdef conditional on the
 *  vector operations being used. This way only the functions where
 *  all prerequisites in terms of vector operations are fulfilled,
 *  will actually be compiled.
 *
 *  @section select Selecting Implementation
 *  There is a mechanism for selecting implementations automatically.
 *  The header file rc_impl_cfg.h provide two macros,
 *  RC_IMPL(@e function-name, @e unrollable), and
 *  RC_UNROLL(@e function-name). The first macro expands to either 0 or 1
 *  and can be tested with @c @#if. The second one expands to an unroll
 *  factor 1, 2 or 4. The following example demonstrates the use:
 *
 *  @code
 *  #if RC_IMPL(rc_example_u8, 1)
 *  void
 *  rc_example_u8(uint8_t *buf, int len)
 *  {
 *      int i;
 *      for (i = 0; i < len;) {
 *          buf[i] = ~buf[i]; i++;
 *          if (RC_UNROLL(rc_example_u8) >= 2) {
 *              buf[i] = ~buf[i]; i++;
 *          }
 *          if (RC_UNROLL(rc_example_u8) == 4) {
 *              buf[i] = ~buf[i]; i++;
 *              buf[i] = ~buf[i]; i++;
 *          }
 *      }
 *  }
 *  #endif
 *  @endcode
 *
 *  This mechanism @e must be used if one of the following apply:
 *
 *    -# There is more than one implementation of the same
 *       function (generic/vector).
 *    -# An implementation is unrollable, i.e. uses the
 *       RC_UNROLL() facility.
 *
 *  There is one version of the rc_impl_cfg.h file for generic
 *  implementations and one for vector implementations, located in
 *  @c compute/generic and @c compute/vector, respectively.
 *  They define the RC_IMPL() and RC_UNROLL() macros differently
 *  based on the content of a platform-specific configuration header
 *  called rapptune.h. This header is generated automatically by the
 *  @ref tune "tuning process".
 *
 *  @section internalref Internal References
 *  RAPP is not intended to contain layers of functionality, but
 *  sometimes it is necessary to call a RAPP function in the
 *  implementation of another.  Do not use the internal @c rc_ -name
 *  directly, if either function has implementations selected by @c
 *  RC_IMPL because then tuning will  malfunction or yield incorrect
 *  results.  Instead, please use #rc_stat_max_bin and its double
 *  rc_stat_max_bin__internal as a template, and how they are used in @c
 *  RC_INTEGRAL_SUM_BIN in @c compute/generic/rc_integral_bin.c.
 *
 *  @section computevars Influential Definitions
 *  There are a few preprocessor definitions that affect the implementation.
 *  They are to be defined @e only by the @ref build "build system".
 *
 *    - <em> RAPP_USE_SIMD </em> \n
 *           Use the SIMD vector backend instead of the SWAR one.
 *           The --enable-backend configure-time option determines what
 *           backend to use. When not specified, or when
 *           --enable-backend=auto is specified, the one used is
 *           determined by predefined C preprocessor macros set by the
 *           compiler. The macros __MMX__, __SSE__, __SSE2__, __SSSE3__,
 *           __ALTIVEC__, __VEC__ and __VADMX__ are inspected, and the last
 *           one found in that list is used. If no backend is found, no vector
 *           implementations are compiled.
 *
 *    - <em> RAPP_FORCE_GENERIC </em> \n
 *           Force the generic implementations to be used everywhere,
 *           overriding the configuration in rapptune.h.
 *
 *    - <em> RAPP_FORCE_SWAR </em> \n
 *           Force the vector implementations with the SWAR (soft-SIMD)
 *           vector backend to be used everywhere, overriding the
 *           configuration in rapptune.h.
 *
 *    - <em> RAPP_FORCE_SIMD </em> \n
 *           Force the vector implementations with the SIMD
 *           vector backend to be used everywhere, overriding the
 *           configuration in rapptune.h.
 *
 *    - <em> RAPP_FORCE_UNROLL={1,2,4} </em> \n
 *           Force all implementations to use a specific unroll
 *           factor, overriding the configuration in rapptune.h.
 *
 *    - <em> RAPP_FORCE_SIZE={2,4,8} </em> \n
 *           Force the default word size to be a specific
 *           value instead of the native machine word size.
 *
 *    - <em> RAPP_FORCE_EXPORT </em> \n
 *           Force all Compute layer API symbols to be exported
 *           in the final library, instead of the default hidden
 *           visibility.
 *
 *  The @e RAPP_FORCE family of parameters are for special purposes,
 *  such as tuning and regression tests, and are not used when building
 *  the final library.
 *
 *  <p>@ref vec_api "Next section: Vector Abstraction Layer"</p>
 */

/**
 *  @page build Build System
 *  @section Overview
 *  The build system uses the GNU Autotools and libtool. All
 *  platform-/compiler-dependent parameters should go into the
 *  configure.ac file that is processed by Autoconf. The configure
 *  script accepts a few non-standard arguments. The most interesting
 *  ones to non-developers are:
 *
 *    - <em> --enable-backend = {auto, nonvector, none, mmx, sse,
 *                               sse2, ssse3, altivec, vadmx} </em> \n
 *         Use the specified SIMD backend. The compiler will then set
 *         standard preprocessor flags for the SIMD extension in question.
 *         The default value @e auto will detect the SIMD backend
 *         automatically from the C compiler characteristics.
 *         The @e nonvector value will disable all vector implementations.
 *         The @e none value will disable all vector implementations and
 *         all tuning.
 *         The default is @e auto.
 *
 *    - <em> --enable-tune-cache </em> \n
 *         Use the pre-tuned cached configuration file if it exists.
 *         This has no effect if the SIMD backend is @e none.
 *         The default is @e yes, unless the pre-tuned file is
 *         out-of-date.
 *
 *    - <em> --with-doxygen </em> \n
 *         Generate doxygen documentation. The default is @e yes, if the
 *         @c doxygen program is present.
 *
 *    - <em> --enable-debug </em> \n
 *         Build in debug mode (default no). This enables asserts and debugging
 *         information, and disables optimization. The default is @e no.
 *
 *  For the full set of options, use @c configure @c --help.
 *
 *  @section Structure
 *  Each directory with source files has its own Makefile.am file
 *  that is processed by Automake. All local variables defined in
 *  these files are prefixed with @c RB_ for RAPP Build, to avoid
 *  accidental clashes with Automake variables.
 *
 *  When the RAPP library is built, several convenience libraries are
 *  built and merged to form the final library. The @c compute/vector
 *  directory is a bit special. It is built two times from the same sources,
 *  resulting in two different convenience libraries. The first time it
 *  is built with no extra flags, producing an implementation using the
 *  SWAR vector backend. The second time we define @e RAPP_USE_SIMD to
 *  build a version with the SIMD backend instead. At the top level of the
 *  Compute layer, all three implementations (generic, SWAR and SIMD) are
 *  combined into the Compute layer convenience library. The function
 *  @ref select "selection" mechanism guarantees, if used correctly, that a
 *  function is only implemented once.
 *
 *  The build system in the @c compute/tune and @c benchmark directories
 *  are a bit special. When the configure script is run, it will check
 *  for the presence of an up-to-date pre-tuned configuration file
 *  directory. If found, a symbolic link @c compute/tune/rapptune.h is
 *  created to this file. During build time, this symbolic link will be
 *  used for @ref select "selecting" the appropriate implementations. The file
 *  benchmarkplot-@<host@>-@<backend@>.html in the
 *  @c benchmark/arch directory is handled analogously.
 *  The symbolic links are only removed on @e distclean.
 *
 *  If the pre-tuned rapptune-@<host@>-@<backend@>.h is present but the
 *  set of tuned functions has changed since it was generated, the
 *  default build behaviour is to generate a new tuning. Alternatively,
 *  with an explicit @c --enable-tune-cache option, untuned fallback
 *  values will be used to fill in the missing tuning numbers, but
 *  compiler warnings will be emitted for each such untuned function
 *  when building RAPP. This is of course not recommended, but may be
 *  useful e.g. when cross-compiling and the target system is not
 *  available for tuning at the time RAPP is built.
 *
 *  <p>@ref tune "Next section: Tuning System"</p>
 */

/**
 *  @page tune Tuning System
 *
 *  @section Purpose
 *  The purpose of the tuning system is to generate the configuration
 *  file rapptune.h that is needed when building the library. The tuning
 *  system does this by running a suite of benchmark tests, and analyzing
 *  the measured performance for each candidate implementation, as shown
 *  in figure 2.
 *
 *  @htmlonly
 *  <table align="center" style="padding: 1em">
 *  <caption align="bottom">Figure 2: The Tuning Process.</caption>
 *  <tr><td>
 *  <img src="tuning.png" style="width: 28em"/>
 *  </td></tr>
 *  </table>
 *  @endhtmlonly
 *
 *  @section Overview
 *  The tuning system consists of all files in the  @c compute/tune
 *  and @c compute/tune/benchmark directories. It is layered on top of
 *  the standard @ref build "build system".  When needed, it is executed
 *  as part of e.g. @c make @c all. By separating the tuning
 *  system from the build system, the latter can be kept simple, and
 *  we can reuse it for the tuning purposes.
 *
 *  If the library is not already tuned, we add the @c tune directory to
 *  the SUBDIRS variable of @c compute/Makefile.am, to connect the tuning
 *  system to the ordinary build system. This effectively makes the
 *  build system re-entrant. That might seem like a contradiction to
 *  what was stated earlier about separation, but it is really only
 *  a matter of letting one system dispatch the other one. Their inner
 *  workings are still kept hidden from each other.
 *
 *  @section tuneproc The Tuning Process
 *  The tuning process consists of the following steps:
 *
 *    -#   For each set of candidate options, create a separate
 *       build subdirectory and configure RAPP there, using the option
 *       @c --with-internal-tune-generation=CAND, where @e CAND has the
 *       form @<impl@>,@<unroll@>, specifying the implementation and
 *       unroll factor for the candidate. Besides causing various @ref
 *       computevars "RAPP_FORCE flags" to be set for the build, this internal
 *       option shortcuts those parts of RAPP that don't apply when tuning,
 *       for example stopping @c compute/tune from being used, and stopping
 *       re-generation of e.g. documentation. The parts that need to be
 *       aware of this re-entrancy are confined to the top-level @c
 *       configure.ac script and the @c compute/tune/Makefile.am file.
 *    -#   In each such subdirectory, build a library with the
 *       implementation candidates using the configured set of options.
 *       Configuration, building and installation for each candidate
 *       subdirectory will happen in parallel, if a parallel-capable make
 *       program such as GNU make and its @c -j option is used.
 *    -#   Install candidate libraries temporarily in the build
 *       directory @c archive as separate libraries, named @c
 *       rappcompute_tune_@<impl@>_@<unroll@>.
 *    -#   Build the benchmark application in @c compute/tune/benchmark.
 *    -#   Create a self-extracting archive @c rappmeasure.run, containing
 *       the library candidates, the benchmark application, the script
 *       @c compute/tune/measure.sh and the progress bar script
 *       @c compute/tune/progress.sh.
 *    -#   If we are cross-compiling, the user is asked to manually run
 *       @c rappmeasure.run on the target platform. Otherwise it will
 *       be executed automatically. When finished, it has produced
 *       a data file @c tunedata.py.
 *    -#   Run the analyzer script @c compute/tune/analyze.py on the
 *       data file. It creates the configuration header rapptune.h
 *       and a report @c tunereport.html.
 *
 *  After tuning, all the generated files are located in the
 *  @c compute/tune directory of the build tree. To make RAPP tuned
 *  for the platform for everyone else, they must be copied to the source
 *  directory and/or added to the distribution. A tarball to send to the
 *  maintainers, containing the necessary files, can be created using the
 *  make-target @c export-new-archfiles. There's also a make-target @c
 *  update-tune-cache to use for copying the generated tune-file and
 *  HTML report to the right place and name in the local source directory.
 *  Alternatively, together with benchmark HTML after @ref perf
 *  "benchmark tests", use the make-target @c update-archfiles.
 *
 *  @section measure Measuring Performance
 *  The @c benchmark application takes the Compute layer library
 *  as an argument and loads it dynamically. It then runs its benchmark
 *  tests for the functions found in library, measuring the throughput
 *  in pixels/second. If a function is not found, the throughput is zero.
 *
 *  The script @c measure.sh runs the benchmark application with
 *  different library implementations and different image sizes.
 *  It generates a data file in Python format containing all measurement
 *  data.
 *
 *  @section metric Performance Metric
 *  When the measurement data file is generated, the Python script
 *  @c analyze.py is used to analyze the data and determine the optimal
 *  implementations and parameters and generate the configuration header
 *  rapptune.h. To be able to compare the performance between two
 *  implementations, we need some sort of @e metric.
 *
 *  For a particular function, we can have several possible
 *  implementations. Order them from 1 to @e N, where @e N denotes the
 *  total number of implementations. For each implementation we also
 *  have several benchmark tests, corresponding to different image sizes.
 *  Let @e M denote the number of tests. For our function, we get an
 *  \f$ M \times N \f$ matrix of measurements in pixels/second:
 *
 *    \f[ \mathbf{P} = [ p_{ij} ]. \f]
 *
 *  We want to compute a @e ranking number \f$ r_j \f$ for each
 *  implementation \f$ j \f$ of the function. First we compute the
 *  average throughput across all implementations:
 *
 *    \f[ q_i = \frac{1}{N} \sum_{j=1}^{N} p_{ij}. \f]
 *
 *  Next, we normalize the data with this average value, creating
 *  a data set of dimensionless values,
 *
 *    \f[ \hat{p}_{ij} = \frac{p_{ij}}{q_i}. \f]
 *
 *  These normalized numbers describe the speedup for a given
 *  implementation and test case, compared to the average performance
 *  of <em>this test case</em>. The normalized numbers are independent
 *  of the absolute throughput of each test case. This is what we want,
 *  since a fast test case could otherwise easily dwarf the results of the
 *  other tests. We want all test cases to contribute equally.
 *
 *  Finally, we compute the dimensionless ranking result as the
 *  arithmetic mean of the speedup results across all the test cases,
 *
 *    \f[ r_j = \sqrt[M]{\prod_{i=1}^{M} \hat{p}_{ij}}. \f]
 *
 *  The implementation with the highest ranking gets picked and the parameters
 *  are written to the configuration header rapptune.h.
 *
 *  @section report Tune Report
 *  The @c analyze.py script also produces a bar plot of the tuning result
 *  in HTML format. It shows the relative speedup for the fastest one
 *  (any unroll factor) of the generic, SWAR and SIMD implementations.
 *  The gain factor reported is the ranking result, normalized with respect
 *  to the slowest bar plotted. Only functions with at least two different
 *  implementations are included in the plot.
 *
 *  <p>@ref perf "Next section: Benchmark Tests"</p>
 */

/**
 *  @page perf Benchmark Tests
 *  There are benchmark tests for all relevant API functions,
 *  located in the @c benchmark directory. The benchmark application
 *  is similar to that of the @ref tune "tuning system", but measures
 *  the throughput of the RAPP API functions instead of the Compute layer
 *  functions. The @ref build "build system" runs all the benchmark tests
 *  and creates the output HTML plot, in a similar way to how the tuned
 *  configurations are built, but without the re-entrancy. The following
 *  steps are carried out at the end of e.g. @c make @c all:
 *
 *    -#   Check if the file @c benchmark/benchmarkplot.html exists.
 *       If so, we are done.
 *    -#   Compile the benchmark application and pack it together with
 *       the library to benchmark in a self-extracting archive
 *       @c rappbenchmark.run.
 *    -#   If we are cross-compiling, the user must manually run
 *       @c rappbenchmark.run on the target platform. Otherwise it will
 *       be executed automatically. When finished, it has produced
 *       data file benchmarkdata.py.
 *    -#   Run the plotdata.py script to generate the output HTML plot
 *       @c benchmarkplot.html.
 *
 *  After benchmarking, the generated plot file is located in the
 *  @c benchmark directory in the build tree. To make RAPP benchmarked
 *  on the platform for everyone else, it must be copied to the source
 *  directory and/or added to the distribution. A tarball to send to the
 *  maintainers, containing the necessary files, can be created using the
 *  make-target @c export-new-archfiles. There's also the make-target @c
 *  update-benchmarks (or together with the tune-file using @c
 *  update-archfiles) to put the generated files in the local source
 *  directory with the correct name.
 *
 *  <p>@ref test "Next section: Correctness Tests"</p>
 */

/**
 *  @page test Correctness Tests
 *  @section back_test Backend Tests
 *  In the Compute layer, only the @ref vec_api "vector backends"
 *  are tested explicitly. The directory @c compute/backend/test
 *  contains these tests. There are two kinds of tests, exhaustive
 *  tests and random tests. The exhaustive tests test all possible
 *  numeric values within the same field. For an operation with
 *  two operands and a 7-bit scalar, this means 8 + 8 + 7 = 23 bits
 *  of combinations, which will take some time on slower platforms.
 *  The random tests vary all parameters randomly.
 *  The build system creates and runs the tests for both the SIMD
 *  backend and the SWAR backend in all possible word sizes using the
 *  RC_FORCE_SIZE override mechanism.
 *
 *  @section api_test API Tests
 *  All API functions are tested by the regression tests in
 *  the @c test directory. The Check framework is used if present,
 *  otherwise the tests are run without it. The framework runs the
 *  tests in a separate process so that memory errors do not interfere
 *  with the actual test application.
 *
 *  There are reference implementations for most of the exported functions,
 *  located in the @c test/reference directory. Most tests check the
 *  RAPP implementations against the reference implementation
 *  using random data and parameters. The tests are designed to
 *  be as complete as possible, rather than just covering a few
 *  hand-coded cases. When writing a test, it is important to
 *  test images that are wide enough. With binary images, a single
 *  vector operation may process 128 pixels at a time, so the test
 *  images will have to be wider than that in order to test all cases.
 *
 *  @section install_test Installation Tests
 *  After running @c make @c install, the installation can be tested by
 *  configuring, building and running a simple application. There's such
 *  an application located in @c test/installtest for which those steps
 *  are done by @c make @c installcheck.
 *
 *  <p>@ref license "Next section: License"</p>
 */

/**
 *  @page license  License
 *  @section License
 *
 *  This document by itself is subject to the GNU Free Documentation
 *  License as mentioned in the note before the @ref overview "Design Overview",
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

#ifndef RAPPCOMPUTE_H
#define RAPPCOMPUTE_H

#include "rc_platform.h"    /* Platform-specific definitions  */
#include "rc_stdbool.h"     /* Portable stdbool.h             */
#include "rc_malloc.h"      /* Aligned memory allocation      */
#include "rc_bitblt_wa.h"   /* Word-aligned bitblit           */
#include "rc_bitblt_wm.h"   /* Word-misaligned bitblit        */
#include "rc_bitblt_va.h"   /* Vector-aligned bitblit         */
#include "rc_bitblt_vm.h"   /* Vector-misaligned bitblit      */
#include "rc_pixel.h"       /* Pixel access macros            */
#include "rc_pixop.h"       /* Pixelwise operations           */
#include "rc_type.h"        /* Type conversions               */
#include "rc_thresh.h"      /* Thresholding to binary         */
#include "rc_reduce.h"      /* 8-bit 2x reduction             */
#include "rc_reduce_bin.h"  /* Binary 2x reduction            */
#include "rc_expand_bin.h"  /* Binary 2x expansion            */
#include "rc_rotate.h"      /* 8-bit image rotation           */
#include "rc_rotate_bin.h"  /* Binary image rotation          */
#include "rc_stat.h"        /* Statistical operations         */
#include "rc_moment_bin.h"  /* Binary image moments           */
#include "rc_filter.h"      /* Fixed-filter convolutions      */
#include "rc_morph_bin.h"   /* Binary morphology              */
#include "rc_fill.h"        /* Connected-components seed fill */
#include "rc_pad.h"         /* 8-bit image padding            */
#include "rc_pad_bin.h"     /* Binary image padding           */
#include "rc_margin.h"      /* Binary image logical margins   */
#include "rc_crop.h"        /* Binary image cropping          */
#include "rc_contour.h"     /* Contour chain code generation  */
#include "rc_rasterize.h"   /* Line rasterization             */
#include "rc_cond.h"        /* Conditional operations         */
#include "rc_gather.h"      /* 8-bit pixel gather             */
#include "rc_gather_bin.h"  /* Binary pixel gather            */
#include "rc_scatter.h"     /* 8-bit pixel scatter            */
#include "rc_scatter_bin.h" /* Binary pixel scatter           */
#include "rc_integral.h"    /* 8-bit integral images          */
#include "rc_integral_bin.h"/* Binary pixel integral images   */

#endif /* RAPPCOMPUTE_H */
