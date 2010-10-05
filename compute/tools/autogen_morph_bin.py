#!/usr/bin/python

#  Copyright (C) 2005-2010, Axis Communications AB, LUND, SWEDEN
#
#  This file is part of RAPP.
#
#  RAPP is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  You can use the comments under either the terms of the GNU Lesser General
#  Public License version 3 as published by the Free Software Foundation,
#  either version 3 of the License or (at your option) any later version, or
#  the GNU Free Documentation License version 1.3 or any later version
#  published by the Free Software Foundation; with no Invariant Sections, no
#  Front-Cover Texts, and no Back-Cover Texts.
#  A copy of the license is included in the documentation section entitled
#  "GNU Free Documentation License".
#
#  RAPP is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License and a copy of the GNU Free Documentation License along
#  with RAPP. If not, see <http://www.gnu.org/licenses/>.


################################################################
#  Script for generating morphological dilation,
#  erosion and hit-or-miss primitives.
################################################################

import sys, datetime, getopt

#
# --------------------------------------------------------------
#  Constants
# --------------------------------------------------------------
#

NAME_BASE   = 'rc_morph'
NAME_ERODE  = NAME_BASE + '_erode'
NAME_DILATE = NAME_BASE + '_dilate'
NAME_HMT    = NAME_BASE + '_hmt'

NAME_LEFT   = 'RC_MORPH_LEFT '
NAME_CENTER = 'RC_WORD_LOAD  '
NAME_RIGHT  = 'RC_MORPH_RIGHT'

TEMPLATE_NAME = 'RC_MORPH_TEMPLATE'
TEMPLATE_ARGS = 'dst, dst_dim, src, src_dim, width, height'

ARG_ROW1 = 'uint8_t *restrict dst, int dst_dim,'
ARG_ROW2 = 'const uint8_t *restrict src, int src_dim,'
ARG_ROW3 = 'int width, int height'

FILENAME = NAME_BASE + '_bin'

#
# --------------------------------------------------------------
#  Verbatim code sections
# --------------------------------------------------------------
#

MACRO_SECTION = """
/**
 *  Align words left.
 */
#define RC_MORPH_LEFT(ptr, off)                       \\
    RC_WORD_ALIGN(RC_WORD_LOAD((ptr) - RC_WORD_SIZE), \\
                  RC_WORD_LOAD(ptr),                  \\
                  8*RC_WORD_SIZE - (off))

/**
 *  Align words right.
 */
#define RC_MORPH_RIGHT(ptr, off)                      \\
    RC_WORD_ALIGN(RC_WORD_LOAD(ptr),                  \\
                  RC_WORD_LOAD((ptr) + RC_WORD_SIZE), \\
                  off)

/**
 *  Morphology iteration step.
 */
#define RC_MORPH_ITER(dptr, sptr, sew) \\
do {                                   \\
    rc_word_t word_ = sew(sptr);       \\
    RC_WORD_STORE(dptr, word_);        \\
    (sptr) += RC_WORD_SIZE;            \\
    (dptr) += RC_WORD_SIZE;            \\
} while (0)


/**
 *  Morphology template.
 */
#define RC_MORPH_TEMPLATE(dst, dst_dim, src, src_dim, \\
                          width, height, sew, unroll) \\
do {                                                  \\
    int tot_ = RC_DIV_CEIL(width, 8*RC_WORD_SIZE);    \\
    int len_ = tot_ / (unroll);                       \\
    int rem_ = tot_ % (unroll);                       \\
    int x_, y_;                                       \\
    for (y_ = 0; y_ < (height); y_++) {               \\
        uint8_t       *dp_ = &(dst)[y_*(dst_dim)];    \\
        const uint8_t *sp_ = &(src)[y_*(src_dim)];    \\
        for (x_ = 0; x_ < len_; x_++) {               \\
            RC_MORPH_ITER(dp_, sp_, sew);             \\
            if ((unroll) >= 2) {                      \\
                RC_MORPH_ITER(dp_, sp_, sew);         \\
            }                                         \\
            if ((unroll) == 4) {                      \\
                RC_MORPH_ITER(dp_, sp_, sew);         \\
                RC_MORPH_ITER(dp_, sp_, sew);         \\
            }                                         \\
        }                                             \\
        for (x_ = 0; x_ < rem_; x_++) {               \\
            RC_MORPH_ITER(dp_, sp_, sew);             \\
        }                                             \\
    }                                                 \\
} while (0)
"""


#
# --------------------------------------------------------------
#  Code generation functions
# --------------------------------------------------------------
#

# Rotate coordinates
def rotate(coords, quarter):
    mat = [[ 1,  0,  0,  1],
           [ 0, -1,  1,  0],
           [-1,  0,  0, -1],
           [ 0,  1, -1,  0]][quarter]
    return [[mat[0]*c[0] + mat[1]*c[1],
             mat[2]*c[0] + mat[3]*c[1]] for c in coords]


# Normalization
def normalize(selist):
    for se in selist:
        try:
            desc = se['desc']
        except:
            se['desc'] = None
        try:
            hit = se['hit']
        except:
            se['hit'] = None
        try:
            miss = se['miss']
        except:
            se['miss'] = None
        try:
            se['rot'] %= 4
        except:
            se['rot'] = 0

        if se['hit']:
            se['hit']  = rotate(se['hit'], se['rot'])
        if se['miss']:
            se['miss'] = rotate(se['miss'], se['rot'])


# Validation
def validate(selist):
    for se in selist:
        if se['hit']:
            for hit in se['hit']:
                # Validate the filter size
                if abs(hit[0]) > 63 or abs(hit[1]) > 63:
                    print 'Filter size is greater than 63x63.'
                    return False
        if se['miss']:
            for miss in se['miss']:
                # Validate the filter size
                if abs(miss[0]) > 63 or abs(miss[1]) > 63:
                    print 'Filter size is greater than 63x63.'
                    return False
    return True


# Generate the file name base
def get_filename():
    return NAME_BASE + '_bin'


# Generate the file description comment
def get_description(suffix):
    return '/**\n' + \
           ' *  @file   ' + FILENAME + suffix + '\n' + \
           ' *  @author Auto-generated by ' + sys.argv[0].strip('./') + \
           ' on ' + datetime.datetime.now().ctime() + '\n' + \
           ' *  @brief  RAPP Compute layer binary ' + \
           'morphology primitives.\n' + \
           ' */\n\n'


# Generate a section comment
def get_section(text):
    return '/*\n' + \
           ' * '  + 62*'-' + '\n' + \
           ' *  ' + text   + '\n' + \
           ' * '  + 62*'-' + '\n' + \
           ' */\n'


# Generate the hit/miss comment string
def get_hmstring(prefix, data):
    ret = prefix
    vec = repr(data).replace('],', '],\n').split('\n')
    vec[len(vec) - 1].strip(',')
    for k in range((len(vec) + 3) / 4):
        ret += ''.join(vec[4*k:4*(k + 1)]) + \
               '\n *  ' + len(prefix)*' '
    return ret.strip() + '  '


# Generate a function comment
def get_comment(se, op):
    comment = '/**\n *  ' + op + '\n *  '
    if se['desc']:
        comment += '\n *  '.join(se['desc'].split('\n *  ')) + \
                   '\n *  '
    if se['hit']:
        comment += get_hmstring('Hit  = ', se['hit'])
    if se['miss']:
        comment += get_hmstring('Miss = ', se['miss'])
    return comment.rstrip() + '/\n'


# Generate the list of y offsets
def get_yoffset(se):
    yoff = []
    if se['hit']:
        yoff += [pt[1] for pt in se['hit']]
    if se['miss']:
        yoff += [pt[1] for pt in se['miss']]
    yoff = map(abs, yoff)
    yoff = list(set(yoff))
    yoff.sort()
    return yoff


# Generate the word macro name ('sew' used in the template)
def get_macro(se, variant):
    name = se['name'].upper()
    macro = '#define ' + variant.upper() + '_' + name + \
            '(ptr) \\\n'
    return macro


# Generate the word macro content
def get_operation(offsets, flip, binop):
    macro = ''
    for k in range(len(offsets)):
        dx = offsets[k][0]*[1,-1][flip];
        dy = offsets[k][1]*[1,-1][flip];

        macro += NAME_LEFT*(dx < 0)    + \
                 NAME_CENTER*(dx == 0) + \
                 NAME_RIGHT*(dx > 0)   + \
                 '(&(ptr)['
        if dy:
            macro += ['-', ' '][dy > 0] + \
                     'dim' + str(abs(dy)).ljust(2)
        else:
            macro += ' 0    '
        macro += ']'
        if dx:
            macro += ',' + str(abs(dx)).rjust(3)
        else:
            macro += '    '
        macro += ') ' + binop + ' \\\n     '
    return macro.strip(binop + ' \\\n')


# Generate the erode word macro content
def get_erode(se):
    return '    (' + get_operation(se['hit'], False, '&') + ')'


# Generate the dilate word macro content
def get_dilate(se):
    return '    (' + get_operation(se['hit'], True, '|') + ')'


# Generate the hit-or-miss word macro content
def get_hmt(se):
    macro = ''
    if se['hit']:
        macro += ' (' + get_operation(se['hit'], False, '&') + ') & \\\n   '
    macro += '~('
    macro += get_operation(se['miss'], False, '|') + ')'
    return '  (' + macro + ')'


# Generate the function name
def get_function(se, variant):
    return variant + '_' + se['name'] + '_bin'


# Generate the function prototype
def get_prototype(se, variant):
    func  = get_function(se, variant)
    proto = 'void\n'   + func + '(' + ARG_ROW1 + '\n' + \
            (len(func) + 1)*' ' + ARG_ROW2 + '\n' + \
            (len(func) + 1)*' ' + ARG_ROW3 + ')'
    return proto


# Generate the row dimension declaration
def get_dimdecl(se):
    decl = ''
    for off in get_yoffset(se):
        if off:
            decl += '    int dim' + str(off) + \
                    ' = src_dim*' + str(off) + ';\n'
    return decl


# Generate the function definition
def get_definition(se, variant, flip):
    name  = se['name']
    space = (len(TEMPLATE_NAME) + 5)*' '
    func  = get_function(se, variant)
    impl  = '#if RC_IMPL(' + func + ', 1)\n' + \
            get_prototype(se, variant) + '\n{\n' + \
            get_dimdecl(se) + '    ' + \
            TEMPLATE_NAME + '(' + TEMPLATE_ARGS + ',\n' + \
            space + variant.upper() + '_' + name.upper() + ',\n' + \
            space + 'RC_UNROLL(' + func + '));\n}\n' + \
            '#endif\n'
    return impl


# Generate the source file content
def get_source(selist):
    # Generate the file description comment
    src = get_description('.c')

    # Generate the include file section
    src += '#include <stddef.h>\n'      + \
           '#include "rc_platform.h"\n' + \
           '#include "rc_impl_cfg.h"\n' + \
           '#include "rc_util.h"\n'     + \
           '#include "rc_word.h"\n'     + \
           '#include "rc_morph_bin.h"\n\n'

    # Copy the template macro section verbatim
    src += get_section('Template macros')
    src += MACRO_SECTION + '\n'

    # Generate the word macros
    src += get_section('SE word macros') + '\n'
    for se in selist:
        if se['miss']:
            src += get_macro(se, NAME_HMT) + get_hmt(se) + '\n\n'
        else:
            src += get_macro(se, NAME_ERODE)  + get_erode(se) + '\n\n'
            src += get_macro(se, NAME_DILATE) + get_dilate(se) + '\n\n'

    # Generate function definitions
    src += get_section('Exported functions') + '\n'
    for se in selist:
        if se['miss']:
            src += get_definition(se, NAME_HMT, False) + '\n\n'
        else:
            src += get_definition(se, NAME_ERODE, False) + '\n\n'
            src += get_definition(se, NAME_DILATE, True) + '\n\n'

    return src


# Generate the header file content
def get_header(selist):
    # Generate the file description comment
    hdr = get_description('.h')

    # Generate include guard start
    hdr += '#ifndef ' + FILENAME.upper() + '_H\n' + \
           '#define ' + FILENAME.upper() + '_H\n\n'

    # Generate the include file section
    hdr += '#include <stdint.h>\n' + \
           '#include "rc_export.h"\n\n'

    # Generate function prototypes.
    for se in selist:
        if not se['miss']:
            hdr += get_comment(se, 'Erosion.')
            hdr += 'RC_EXPORT ' + get_prototype(se, NAME_ERODE)  + ';\n\n'
            hdr += get_comment(se, 'Dilation.')
            hdr += 'RC_EXPORT ' + get_prototype(se, NAME_DILATE) + ';\n\n'

    for se in selist:
        if se['miss']:
            hdr += get_comment(se, 'Hit-or-miss transform.')
            hdr += 'RC_EXPORT ' + get_prototype(se, NAME_HMT) + ';\n\n'

    return hdr + \
           '#endif /* ' + FILENAME.upper() + '_H */\n'

# Print the usage string
def usage():
    print 'Usage: ' + sys.argv[0] + ' <config>'


# Main function
def main():
    # Open the config file
    try:
        cfg = open(sys.argv[1], 'rb')
    except:
        usage()
        sys.exit(1)

    try:
        # Parse the config file
        exec cfg

        # Normalize the config data
        normalize(selist)

        # Validate the config data
        validate(selist)

        # Close the config file
        cfg.close()

    except:
        print 'Invalid config file.'
        usage()
        cfg.close()
        sys.exit(1)

    # Create the source file
    source = open(FILENAME + '.c', 'wb')
    source.write(get_source(selist))
    source.close()

    # Create the header file
    header = open(FILENAME + '.h', 'wb')
    header.write(get_header(selist))
    header.close()

main()
