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
 *  @file   rapp_morph_bin.c
 *  @brief  RAPP binary morphology.
 *
 *  ALGORITHM
 *  ---------
 *  The binary morphology drivers implement the morphology operations
 *  by decomposing them into morphological sequences. The primitive
 *  operations, called atoms in this driver, are implemented by the
 *  RAPP Compute layer.
 *
 *  For each operation, there is an atom table, containing all the
 *  relevant atomic primitives. There is also a sequence table,
 *  where each entry in the table describes the sequence of atoms
 *  to run for a given size of the operation. The line and diamond
 *  elements share the same sequence table (rapp_morph_seq_generic),
 *  while the octagon and disc elements have use custom sequences.
 *
 *  Operations using diamonds, octagons and discs are decomposed using
 *  logarithmic decomposition. Rectangle operations are first decomposed
 *  into horizontal and vertical lines, which are then further decomposed
 *  using logarithmic decomposition.
 */

#include "rappcompute.h"    /* RAPP Compute API      */
#include "rapp_api.h"       /* API symbol macro      */
#include "rapp_util.h"      /* Validation            */
#include "rapp_error.h"     /* Error codes           */
#include "rapp_error_int.h" /* Error handling        */
#include "rapp_pad_bin.h"   /* Binary image padding  */
#include "rapp_morph_bin.h" /* Binary morphology API */


/*
 * -------------------------------------------------------------
 *  Constants and macros
 * -------------------------------------------------------------
 */

/**
 *  The maximum padding needed.
 */
#define RAPP_MORPH_PADDING 16

/**
 *  Empty atom initializer.
 */
#define RAPP_MORPH_ATOM_NONE \
    {NULL, NULL, 0}

/**
 *  Initializer for atom dispatcher tables.
 */
#define RAPP_MORPH_ATOM(name, pad)      \
    {&rc_morph_erode_  ## name ## _bin, \
     &rc_morph_dilate_ ## name ## _bin, \
     pad}



/*
 * -------------------------------------------------------------
 *  Type definitions
 * -------------------------------------------------------------
 */

/**
 *  Morphology compute function type.
 */
typedef void rapp_morph_func_t(uint8_t *restrict, int,
                               const uint8_t *restrict, int, int, int);

/**
 *  The morphological atom data object.
 */
typedef struct rapp_morph_atom_st {
    rapp_morph_func_t *erode;  /**< Erosion  function pointer  */
    rapp_morph_func_t *dilate; /**< Dilation function pointer  */
    int                pad;    /**< Required padding in pixels */
} rapp_morph_atom_t;


/**
 *  The decomposition sequence table object.
 */
typedef struct rapp_morph_seq_st {
    int     num;     /**< The number of components */
    uint8_t comp[7]; /**< Index in atom tables     */
} rapp_morph_seq_t;


/*
 * -------------------------------------------------------------
 *  Global atom tables
 * -------------------------------------------------------------
 */

/**
 *  Horizontal line atoms.
 *  To be used with the generic sequence table rapp_morph_generic_seq[].
 */
static const rapp_morph_atom_t rapp_morph_horz_tab[] = {
    RAPP_MORPH_ATOM(line_1x2,     1), /* Even-size finalizer */
    RAPP_MORPH_ATOM(line_1x3,     1), /*  1x3 base           */
    RAPP_MORPH_ATOM(line_1x3_p,   1), /*  1x3 perimeter      */
    RAPP_MORPH_ATOM(line_1x5_p,   2), /*  1x5 perimeter      */
    RAPP_MORPH_ATOM(line_1x7_p,   3), /*  1x7 perimeter      */
    RAPP_MORPH_ATOM(line_1x9_p,   4), /*  1x9 perimeter      */
    RAPP_MORPH_ATOM(line_1x13_p,  6), /* 1x13 perimeter      */
    RAPP_MORPH_ATOM(line_1x15_p,  7), /* 1x15 perimeter      */
    RAPP_MORPH_ATOM(line_1x17_p,  8), /* 1x17 perimeter      */
    RAPP_MORPH_ATOM(line_1x25_p, 12), /* 1x25 perimeter      */
    RAPP_MORPH_ATOM(line_1x29_p, 14), /* 1x29 perimeter      */
    RAPP_MORPH_ATOM(line_1x31_p, 15)  /* 1x31 perimeter      */
};

/**
 *  Vertical line atoms.
 *  To be used with the generic sequence table rapp_morph_generic_seq[].
 */
static const rapp_morph_atom_t rapp_morph_vert_tab[] = {
    RAPP_MORPH_ATOM(line_2x1,     1), /* Even-size finalizer */
    RAPP_MORPH_ATOM(line_3x1,     1), /*  3x1 base           */
    RAPP_MORPH_ATOM(line_3x1_p,   1), /*  3x1 perimeter      */
    RAPP_MORPH_ATOM(line_5x1_p,   2), /*  5x1 perimeter      */
    RAPP_MORPH_ATOM(line_7x1_p,   3), /*  7x1 perimeter      */
    RAPP_MORPH_ATOM(line_9x1_p,   4), /*  9x1 perimeter      */
    RAPP_MORPH_ATOM(line_13x1_p,  6), /* 13x1 perimeter      */
    RAPP_MORPH_ATOM(line_15x1_p,  7), /* 15x1 perimeter      */
    RAPP_MORPH_ATOM(line_17x1_p,  8), /* 17x1 perimeter      */
    RAPP_MORPH_ATOM(line_25x1_p, 12), /* 25x1 perimeter      */
    RAPP_MORPH_ATOM(line_29x1_p, 14), /* 29x1 perimeter      */
    RAPP_MORPH_ATOM(line_31x1_p, 15)  /* 31x1 perimeter      */
};

/**
 *  Diamond atoms.
 *  To be used with the generic sequence table rapp_morph_generic_seq[].
 */
static const rapp_morph_atom_t rapp_morph_diamond_tab[] = {
    RAPP_MORPH_ATOM_NONE,                 /* No finalizer    */
    RAPP_MORPH_ATOM(diamond_3x3,      1), /*  3x3  base      */
    RAPP_MORPH_ATOM(diamond_3x3_p,    1), /*  3x3  perimeter */
    RAPP_MORPH_ATOM(diamond_5x5_p,    2), /*  5x5  perimeter */
    RAPP_MORPH_ATOM(diamond_7x7_p,    3), /*  7x7  perimeter */
    RAPP_MORPH_ATOM(diamond_9x9_p,    4), /*  9x9  perimeter */
    RAPP_MORPH_ATOM(diamond_13x13_p,  6), /* 13x13 perimeter */
    RAPP_MORPH_ATOM(diamond_15x15_p,  7), /* 15x15 perimeter */
    RAPP_MORPH_ATOM(diamond_17x17_p,  8), /* 17x17 perimeter */
    RAPP_MORPH_ATOM(diamond_25x25_p, 12), /* 25x25 perimeter */
    RAPP_MORPH_ATOM(diamond_29x29_p, 14), /* 29x29 perimeter */
    RAPP_MORPH_ATOM(diamond_31x31_p, 15)  /* 31x31 perimeter */
};

/**
 *  Octagon atoms.
 *  To be used with the octagon sequence table rapp_morph_octagon_seq[].
 */
static const rapp_morph_atom_t rapp_morph_octagon_tab[] = {
    RAPP_MORPH_ATOM(line_1x3,         1), /*  3x3  base part one */
    RAPP_MORPH_ATOM(line_3x1,         1), /*  3x3  base part two */
    RAPP_MORPH_ATOM(square_3x3_p,     1), /*  3x3  perimeter     */
    RAPP_MORPH_ATOM(diamond_3x3_p,    1), /*  3x3  perimeter alt */
    RAPP_MORPH_ATOM(octagon_5x5_p,    2), /*  5x5  perimeter     */
    RAPP_MORPH_ATOM(octagon_7x7_p,    3), /*  7x7  perimeter     */
    RAPP_MORPH_ATOM(octagon_9x9_p,    4), /*  9x9  perimeter     */
    RAPP_MORPH_ATOM(octagon_13x13_p,  6), /* 13x13 perimeter     */
    RAPP_MORPH_ATOM(octagon_15x15_p,  7), /* 15x15 perimeter     */
    RAPP_MORPH_ATOM(octagon_17x17_p,  8), /* 17x17 perimeter     */
    RAPP_MORPH_ATOM(octagon_25x25_p, 12), /* 25x25 perimeter     */
    RAPP_MORPH_ATOM(octagon_29x29_p, 14), /* 29x29 perimeter     */
    RAPP_MORPH_ATOM(octagon_31x31_p, 15)  /* 31x31 perimeter     */
};

/**
 *  Disc atoms.
 *  To be used with the disc sequence table rapp_morph_disc_seq[].
 */
static const rapp_morph_atom_t rapp_morph_disc_tab[] = {
    RAPP_MORPH_ATOM(diamond_3x3,   1), /*  3x3  base          */
    RAPP_MORPH_ATOM(diamond_5x5_p, 2), /*  5x5  perimeter     */
    RAPP_MORPH_ATOM(diamond_7x7_p, 3), /*  7x7  perimeter alt */
    RAPP_MORPH_ATOM(disc_7x7,      3), /*  7x7  base          */
    RAPP_MORPH_ATOM(disc_7x7_p,    3), /*  7x7  perimeter     */
    RAPP_MORPH_ATOM(disc_9x9_p,    4), /*  9x9  perimeter     */
    RAPP_MORPH_ATOM(disc_11x11_p,  5), /* 11x11 perimeter     */
    RAPP_MORPH_ATOM(disc_13x13_p,  6), /* 13x13 perimeter     */
    RAPP_MORPH_ATOM(disc_15x15_p,  7), /* 15x15 perimeter     */
    RAPP_MORPH_ATOM(disc_17x17_p,  8), /* 17x17 perimeter     */
    RAPP_MORPH_ATOM(disc_19x19_p,  9), /* 19x19 perimeter     */
    RAPP_MORPH_ATOM(disc_25x25_p, 12), /* 25x25 perimeter     */
};


/*
 * -------------------------------------------------------------
 *  Global decomposition sequence tables
 * -------------------------------------------------------------
 */

/**
 *  Logaritmic decomposition for SEs that have exactly the same
 *  shape at all scales, e.g. lines, squares and diamonds.
 *  The atom index 0 is reserved for the even-size finalizer SE.
 */
static const rapp_morph_seq_t rapp_morph_generic_seq[] = {
    {1, {1, 0                }},  /*  3x3  */
    {2, {1, 2, 0             }},  /*  5x5  */
    {3, {1, 2, 2, 0          }},  /*  7x7  */
    {3, {1, 2, 3, 0          }},  /*  9x9  */
    {4, {1, 2, 3, 2, 0       }},  /* 11x11 */
    {4, {1, 2, 3, 3, 0       }},  /* 13x13 */
    {4, {1, 2, 3, 4, 0       }},  /* 15x15 */
    {4, {1, 2, 3, 5, 0       }},  /* 17x17 */
    {5, {1, 2, 3, 5, 2, 0    }},  /* 19x19 */
    {5, {1, 2, 3, 5, 3, 0    }},  /* 21x21 */
    {5, {1, 2, 3, 5, 4, 0    }},  /* 23x23 */
    {5, {1, 2, 3, 5, 5, 0    }},  /* 25x25 */
    {5, {1, 2, 3, 4, 6, 0    }},  /* 27x27 */
    {5, {1, 2, 3, 5, 6, 0    }},  /* 29x29 */
    {5, {1, 2, 3, 5, 7, 0    }},  /* 31x31 */
    {5, {1, 2, 3, 5, 8, 0    }},  /* 33x33 */
    {6, {1, 2, 3, 5, 8,  2, 0}},  /* 35x35 */
    {6, {1, 2, 3, 5, 8,  3, 0}},  /* 37x37 */
    {6, {1, 2, 3, 5, 8,  4, 0}},  /* 39x39 */
    {6, {1, 2, 3, 5, 8,  5, 0}},  /* 41x41 */
    {6, {1, 2, 3, 5, 7,  6, 0}},  /* 43x43 */
    {6, {1, 2, 3, 5, 8,  6, 0}},  /* 45x45 */
    {6, {1, 2, 3, 5, 8,  7, 0}},  /* 47x47 */
    {6, {1, 2, 3, 5, 8,  8, 0}},  /* 49x49 */
    {6, {1, 2, 3, 4, 6,  9, 0}},  /* 51x51 */
    {6, {1, 2, 3, 4, 7,  9, 0}},  /* 53x53 */
    {6, {1, 2, 3, 5, 7,  9, 0}},  /* 55x55 */
    {6, {1, 2, 3, 5, 8,  9, 0}},  /* 57x57 */
    {6, {1, 2, 3, 5, 7, 10, 0}},  /* 59x59 */
    {6, {1, 2, 3, 5, 7, 11, 0}},  /* 61x61 */
    {6, {1, 2, 3, 5, 8, 11, 0}}   /* 63x63 */
};

/**
 *  Logaritmic decomposition of octagon SEs.
 */
static const rapp_morph_seq_t rapp_morph_octagon_seq[] = {
    {2, {0, 1                }},  /*  3x3  */
    {3, {0, 1, 3             }},  /*  5x5  */
    {4, {0, 1, 3, 3          }},  /*  7x7  */
    {4, {0, 1, 3, 4          }},  /*  9x9  */
    {5, {0, 1, 3, 4, 3       }},  /* 11x11 */
    {5, {0, 1, 3, 4, 4       }},  /* 13x13 */
    {5, {0, 1, 3, 4, 5       }},  /* 15x15 */
    {5, {0, 1, 3, 4, 6       }},  /* 17x17 */
    {6, {0, 1, 3, 4, 6, 3    }},  /* 19x19 */
    {6, {0, 1, 3, 4, 5, 5    }},  /* 21x21 */
    {6, {0, 1, 3, 4, 6, 5    }},  /* 23x23 */
    {6, {0, 1, 2, 3, 5, 7    }},  /* 25x25 */
    {6, {0, 1, 3, 4, 5, 7    }},  /* 27x27 */
    {6, {0, 1, 3, 4, 6, 7    }},  /* 29x29 */
    {6, {0, 1, 3, 4, 6, 8    }},  /* 31x31 */
    {6, {0, 1, 3, 4, 6, 9    }},  /* 33x33 */
    {7, {0, 1, 3, 4, 6, 9,  3}},  /* 35x35 */
    {7, {0, 1, 3, 4, 6, 9,  4}},  /* 37x37 */
    {7, {0, 1, 3, 4, 6, 9,  5}},  /* 39x39 */
    {7, {0, 1, 3, 4, 6, 9,  6}},  /* 41x41 */
    {7, {0, 1, 3, 4, 6, 8,  7}},  /* 43x43 */
    {7, {0, 1, 3, 4, 6, 9,  7}},  /* 45x45 */
    {7, {0, 1, 3, 4, 6, 9,  8}},  /* 47x47 */
    {7, {0, 1, 3, 4, 6, 9,  9}},  /* 49x49 */
    {7, {0, 1, 3, 4, 5, 7, 10}},  /* 51x51 */
    {7, {0, 1, 3, 4, 5, 8, 10}},  /* 53x53 */
    {7, {0, 1, 3, 4, 6, 8, 10}},  /* 55x55 */
    {7, {0, 1, 3, 4, 6, 9, 10}},  /* 57x57 */
    {7, {0, 1, 3, 4, 5, 9, 11}},  /* 59x59 */
    {7, {0, 1, 3, 4, 6, 8, 12}},  /* 61x61 */
    {7, {0, 1, 3, 4, 6, 9, 12}}   /* 63x63 */
};

/**
 *  Logaritmic decomposition of disc SEs.
 */
static const rapp_morph_seq_t rapp_morph_disc_seq[] = {
    {1, {0                }},  /*  3x3  */
    {2, {0, 0             }},  /*  5x5  */
    {1, {3                }},  /*  7x7  */
    {2, {3, 0             }},  /*  9x9  */
    {3, {0, 4, 0          }},  /* 11x11 */
    {2, {3, 4             }},  /* 13x13 */
    {3, {0, 4, 4          }},  /* 15x15 */
    {3, {0, 3, 5,         }},  /* 17x17 */
    {4, {0, 4, 5,  0      }},  /* 19x19 */
    {4, {0, 4, 6,  0      }},  /* 21x21 */
    {4, {0, 4, 7,  0      }},  /* 23x23 */
    {4, {0, 4, 7,  1      }},  /* 25x25 */
    {4, {0, 4, 7,  4      }},  /* 27x27 */
    {4, {0, 4, 8,  4      }},  /* 29x29 */
    {4, {0, 4, 8,  5      }},  /* 31x31 */
    {5, {0, 4, 8,  5, 0   }},  /* 33x33 */
    {5, {0, 4, 8,  5, 1   }},  /* 35x35 */
    {5, {0, 4, 8,  5, 4   }},  /* 37x37 */
    {5, {0, 4, 9,  5, 2   }},  /* 39x39 */
    {5, {0, 4, 9,  6, 2   }},  /* 41x41 */
    {5, {0, 4, 9,  8, 1   }},  /* 43x43 */
    {5, {0, 4, 9,  8, 4   }},  /* 45x45 */
    {5, {0, 4, 8, 10, 4   }},  /* 47x47 */
    {5, {0, 4, 8, 10, 5   }},  /* 49x49 */
    {5, {0, 4, 7, 11, 4   }},  /* 51x51 */
    {5, {0, 4, 8, 11, 4   }},  /* 53x53 */
    {5, {0, 4, 8, 11, 5   }},  /* 55x55 */
    {6, {0, 4, 7, 11, 4, 4}},  /* 57x57 */
    {6, {0, 4, 8, 11, 4, 4}},  /* 59x59 */
    {6, {0, 4, 8, 11, 5, 4}},  /* 61x61 */
    {6, {0, 4, 9, 11, 5, 4}}   /* 63x63 */
};



/*
 * -------------------------------------------------------------
 *  Local functions fwd declare
 * -------------------------------------------------------------
 */

static int
rapp_morph_isotropic(uint8_t *dst, int dst_dim,
                     const uint8_t *src, int src_dim,
                     int width, int height, int radius,
                     bool erode, bool pad,
                     const rapp_morph_atom_t *atm,
                     const rapp_morph_seq_t  *seq,
                     void *work);

static void
rapp_morph_separable(uint8_t *dst, int dst_dim,
                     const uint8_t *src, int src_dim,
                     int width, int height,
                     bool erode,
                     const rapp_morph_atom_t *hatm,
                     const rapp_morph_atom_t *vatm,
                     const rapp_morph_seq_t  *hseq,
                     const rapp_morph_seq_t  *vseq,
                     void *work);

static void
rapp_morph_get_entry(rapp_morph_seq_t *seq,
                     const rapp_morph_seq_t *tab, int size);

static void
rapp_morph_setup(uint8_t **tmp1, uint8_t **tmp2, int *dim,
                 int width, int height, void *work);

static void
rapp_morph_copy_padding(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height);

static void
rapp_morph_seq_driver(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      uint8_t *tmp1, uint8_t *tmp2, int tmp_dim,
                      int width, int height, bool erode, bool pad,
                      const rapp_morph_seq_t  *seq,
                      const rapp_morph_atom_t *atom);


/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

/**
 *  Compute the minimum size of the working buffer needed by the
 *  binary morphological functions.
 */
static int
rc_morph_worksize_bin(int width, int height)
{
    return (2*height + 3*16)*(rc_align((width + 7) / 8) + 2*RC_ALIGNMENT);
}

RAPP_API(int, rapp_morph_worksize_bin,
         (int width, int height))
{
    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    if (width < 1 || height < 1) {
        return RAPP_ERR_IMG_SIZE;
    }

    return rc_morph_worksize_bin(width, height);
}


/**
 *  Erosion with a rectangular SE.
 */
RAPP_API(int, rapp_morph_erode_rect_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int wrect, int hrect,
          void *restrict work))
{
    int minxpadding = rc_align((MIN((wrect + 1) / 2, 16) + 7) / 8);
    int minypadding = MIN((hrect + 1) / 2, 16);

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate the arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0, rc_align((width + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, work, 0, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0,
                                       rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, work, 0, height,
                                     rc_align((width + 7) / 8),
                                     rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_BIN(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        /* Return the error code */
        return rapp_error_bin_bin(dst, dst_dim, width,                  height,
                                  src, src_dim, width + 2*RC_ALIGNMENT, height);
    }
    if (wrect < 1 || wrect > 63 ||
        hrect < 1 || hrect > 63)
    {
        return RAPP_ERR_PARM_RANGE;
    }
    else if (!work) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    if (wrect == 2 && hrect == 2) {
        /* Handle the 2x2 special case */
        rc_morph_erode_square_2x2_bin(dst, dst_dim,
                                      src, src_dim, width, height);
    }
    else {
        rapp_morph_seq_t wseq, hseq;

        /* Copy the sequence tables and append finalizing SEs */
        rapp_morph_get_entry(&wseq, rapp_morph_generic_seq, wrect);
        rapp_morph_get_entry(&hseq, rapp_morph_generic_seq, hrect);

        if (wrect != 1 && hrect != 1) {
            /* Run two separable sequences */
            rapp_morph_separable(dst, dst_dim, src, src_dim,
                                 width, height, true,
                                 rapp_morph_horz_tab,
                                 rapp_morph_vert_tab,
                                 &wseq, &hseq, work);
        }
        else if (wrect != 1) {
            /* Run the horizontal sequence */
            return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                        width, height, 2, true, false,
                                        rapp_morph_horz_tab, &wseq, work);
        }
        else if (hrect != 1) {
            /* Run the vertical sequence */
            return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                        width, height, 2, true, false,
                                        rapp_morph_vert_tab, &hseq, work);
        }
    }

    return RAPP_OK;
}


/**
 *  Dilation with a rectangular SE.
 */
RAPP_API(int, rapp_morph_dilate_rect_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int wrect, int hrect,
          void *restrict work))
{
    int minxpadding = rc_align((MIN((wrect + 1) / 2, 16) + 7) / 8);
    int minypadding = MIN((hrect + 1) / 2, 16);

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate the arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0, rc_align((width + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, work, 0, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0,
                                       rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, work, 0, height,
                                     rc_align((width + 7) / 8),
                                     rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_BIN(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        /* Return the error code */
        return rapp_error_bin_bin(dst, dst_dim, width,                  height,
                                  src, src_dim, width + 2*RC_ALIGNMENT, height);
    }
    if (wrect < 1 || wrect > 63 ||
        hrect < 1 || hrect > 63)
    {
        return RAPP_ERR_PARM_RANGE;
    }
    else if (!work) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Perform operation */
    if (wrect == 2 && hrect == 2) {
        /* Handle the 2x2 special case */
        rc_morph_dilate_square_2x2_bin(dst, dst_dim,
                                       src, src_dim, width, height);
    }
    else {
        rapp_morph_seq_t wseq, hseq;

        /* Copy the sequence tables and append finalizing SEs */
        rapp_morph_get_entry(&wseq, rapp_morph_generic_seq, wrect);
        rapp_morph_get_entry(&hseq, rapp_morph_generic_seq, hrect);

        if (wrect != 1 && hrect != 1) {
            /* Run two separable sequences */
            rapp_morph_separable(dst, dst_dim, src, src_dim,
                                 width, height, false,
                                 rapp_morph_horz_tab,
                                 rapp_morph_vert_tab,
                                 &wseq, &hseq, work);
        }
        else if (wrect != 1) {
            /* Run the horizontal sequence */
            return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                        width, height, 2, false, false,
                                        rapp_morph_horz_tab, &wseq, work);
        }
        else if (hrect != 1) {
            /* Run the vertical sequence */
            return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                        width, height, 2, false, false,
                                        rapp_morph_vert_tab, &hseq, work);
        }
    }

    return RAPP_OK;
}


/**
 *  Erosion with a diamond SE.
 */
RAPP_API(int, rapp_morph_erode_diam_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, true, false,
                                rapp_morph_diamond_tab,
                                &rapp_morph_generic_seq[radius - 2],
                                work);
}


/**
 *  Dilation with a diamond SE.
 */
RAPP_API(int, rapp_morph_dilate_diam_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, false, false,
                                rapp_morph_diamond_tab,
                                &rapp_morph_generic_seq[radius - 2],
                                work);
}


/**
 *  Erosion with an octagon SE.
 *  We need to tell the isotropic sequence driver to copy the top/bottom
 *  padding from the src buffer in the second pass because the octagon
 *  sequence starts with a separated 3x3 square.
 */
RAPP_API(int, rapp_morph_erode_oct_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, true,
                                true /* Copy padding */,
                                rapp_morph_octagon_tab,
                                &rapp_morph_octagon_seq[radius - 2],
                                work);
}


/**
 *  Dilation with an octagon SE.
 *  We need to tell the isotropic sequence driver to copy the top/bottom
 *  padding from the src buffer in the second pass because the octagon
 *  sequence starts with a separated 3x3 square.
 */
RAPP_API(int, rapp_morph_dilate_oct_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, false,
                                true /* Copy padding */,
                                rapp_morph_octagon_tab,
                                &rapp_morph_octagon_seq[radius - 2],
                                work);
}


/**
 *  Erosion with a disc SE.
 */
RAPP_API(int, rapp_morph_erode_disc_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, true, false,
                                rapp_morph_disc_tab,
                                &rapp_morph_disc_seq[radius - 2],
                                work);
}


/**
 *  Dilation with a disc SE.
 */
RAPP_API(int, rapp_morph_dilate_disc_bin,
         (uint8_t *restrict dst, int dst_dim,
          const uint8_t *restrict src, int src_dim,
          int width, int height, int radius,
          void *restrict work))
{
    return rapp_morph_isotropic(dst, dst_dim, src, src_dim,
                                width, height, radius, false, false,
                                rapp_morph_disc_tab,
                                &rapp_morph_disc_seq[radius - 2],
                                work);
}


/*
 * -------------------------------------------------------------
 *  Local functions
 * -------------------------------------------------------------
 */

/**
 *  Perform an isotropic operation.
 */
static int
rapp_morph_isotropic(uint8_t *dst, int dst_dim,
                     const uint8_t *src, int src_dim,
                     int width, int height, int radius,
                     bool erode, bool pad,
                     const rapp_morph_atom_t *atm,
                     const rapp_morph_seq_t  *seq,
                     void *work)
{
    uint8_t *tmp1;
    uint8_t *tmp2;
    int      dim;
    int      minxpadding = rc_align((MIN(radius, 16) + 7) / 8);
    int      minypadding = MIN(radius, 16);

    if (!RAPP_INITIALIZED()) {
        return RAPP_ERR_UNINITIALIZED;
    }

    /* Validate the arguments */
    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, dst, dst_dim, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0, rc_align((width + 7) / 8)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_OFFSET(src, src_dim, work, 0, height,
                                       -minxpadding - minypadding * src_dim,
                                       minxpadding + minypadding * src_dim +
                                       rc_align((width + 7) / 8),
                                       0,
                                       rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_RESTRICT_PLUS(dst, dst_dim, work, 0, height,
                                     rc_align((width + 7) / 8),
                                     rc_morph_worksize_bin(width, height)))
    {
        return RAPP_ERR_OVERLAP;
    }

    if (!RAPP_VALIDATE_BIN(dst, dst_dim, width,                  height) ||
        !RAPP_VALIDATE_BIN(src, src_dim, width + 2*RC_ALIGNMENT, height))
    {
        /* Return the error code */
        return rapp_error_bin_bin(dst, dst_dim, width,                  height,
                                  src, src_dim, width + 2*RC_ALIGNMENT, height);
    }
    if (radius < 2 || radius > 32) {
        return RAPP_ERR_PARM_RANGE;
    }
    if (!work) {
        return RAPP_ERR_PARM_NULL;
    }

    /* Initialize the temporary working buffers */
    rapp_morph_setup(&tmp1, &tmp2, &dim, width, height, work);

    /* Execute the sequence */
    rapp_morph_seq_driver(dst, dst_dim, src, src_dim, tmp1, tmp2, dim,
                          width, height, erode, pad, seq, atm);
    return RAPP_OK;
}

/**
 *  Perform a separable operation.
 *  No argument validation.
 */
static void
rapp_morph_separable(uint8_t *dst, int dst_dim,
                     const uint8_t *src, int src_dim,
                     int width, int height,
                     bool erode,
                     const rapp_morph_atom_t *hatm,
                     const rapp_morph_atom_t *vatm,
                     const rapp_morph_seq_t  *hseq,
                     const rapp_morph_seq_t  *vseq,
                     void *work)
{
    uint8_t *tmp1;
    uint8_t *tmp2;
    uint8_t *buf;
    int      dim;

    /* Initialize the temporary working buffers */
    rapp_morph_setup(&tmp1, &tmp2, &dim, width, height, work);

    /* Set the buffer pointer to the buffer not used in the last write */
    buf = hseq->num & 1 ? tmp1 : tmp2;

    /* Execute the first sequence (horizontal) */
    rapp_morph_seq_driver(buf, dim, src, src_dim, tmp1, tmp2, dim,
                          width, height, erode, false, hseq, hatm);

    /* Copy the top and bottom padding from the source image */
    rapp_morph_copy_padding(buf, dim, src, src_dim, width, height);

    /* Execute the second sequence (vertical) */
    rapp_morph_seq_driver(dst, dst_dim, buf, dim,
                          buf == tmp1 ? tmp2 : tmp1, buf, dim,
                          width, height, erode, false, vseq, vatm);
}

/**
 *  Copy the sequence table entry for an SE of size 'size'.
 *  If 'size' is even, the count is increased by one to include the
 *  even-size finalizer SE with index '0' at the end of the sequence.
 */
static void
rapp_morph_get_entry(rapp_morph_seq_t *seq,
                     const rapp_morph_seq_t *tab, int size)
{
    /* Initialize */
    seq->comp[0] = 0;
    seq->num     = 0;

    /* Copy sequence table */
    if (size >= 3 && size <= 63) {
        *seq = tab[(size - 3) / 2];
    }

    /* Add even-size finalizer */
    seq->num += !(size & 1);
}

/**
 *  Set up the temporary buffers for morphology sequencies.
 */
static void
rapp_morph_setup(uint8_t **tmp1, uint8_t **tmp2, int *dim,
                 int width, int height, void *work)
{
    /* Set the buffer row dimension */
    *dim = rc_align((width + 7)/8) + 2*RC_ALIGNMENT;

    /* Set buffer pointers */
    *tmp1 = &((uint8_t*)work)[RAPP_MORPH_PADDING*(*dim) + RC_ALIGNMENT];
    *tmp2 = &(*tmp1)[(*dim)*(height + RAPP_MORPH_PADDING)];
}

/**
 *  Copy the top and bottom 1 pixel padding from the source buffer.
 */
static void
rapp_morph_copy_padding(uint8_t *dst, int dst_dim,
                        const uint8_t *src, int src_dim,
                        int width, int height)
{
    rc_bitblt_va_copy_bin(&dst[      -dst_dim], dst_dim,
                          &src[      -src_dim], src_dim, width, 1);
    rc_bitblt_va_copy_bin(&dst[height*dst_dim], dst_dim,
                          &src[height*src_dim], src_dim, width, 1);
}

/**
 *  Execute a morphology sequence.
 */
static void
rapp_morph_seq_driver(uint8_t *dst, int dst_dim,
                      const uint8_t *src, int src_dim,
                      uint8_t *tmp1, uint8_t *tmp2, int tmp_dim,
                      int width, int height, bool erode, bool pad,
                      const rapp_morph_seq_t  *seq,
                      const rapp_morph_atom_t *atom)
{
    rapp_morph_func_t *func = NULL;
    const uint8_t     *sbuf; /* Current source buffer             */
    uint8_t           *dbuf; /* Current destination buffer        */
    int                sdim; /* Current source row dimension      */
    int                ddim; /* Current destination row dimension */
    int                k;

    /* Check for invalid buffer aliasing */
    assert(src != tmp1);
    assert(dst != (seq->num & 1 ? tmp2 : tmp1));

    /* Initialize the source buffer params */
    sbuf = src;
    sdim = src_dim;

    /* Execute the morphological sequence */
    for (k = 0; k < seq->num; k++) {
        int idx;

        if (k == seq->num - 1) {
            /* Write to the dst buffer in the last operation */
            dbuf = dst;
            ddim = dst_dim;
        }
        else {
            /* Write intermediate results to temporary buffers */
            dbuf = k & 1 ? tmp2 : tmp1;
            ddim = tmp_dim;
        }

        /* Set the atom index */
        idx = seq->comp[k];

        /* Pad the intermediate result */
        if (k == 1 && pad) {
            /* Copy the horizontal padding from the source buffer */
            rapp_morph_copy_padding((uint8_t*)sbuf, sdim,
                                    src, src_dim, width, height);
        }
        else if (k > 0) {
            /* Pad with the clamped edge value */
            rapp_pad_clamp_bin((uint8_t*)sbuf, sdim, 0,
                               width, height, atom[idx].pad);
        }

        /* Call the morphological atom operation */
        func = erode ? atom[idx].erode : atom[idx].dilate;
        (*func)(dbuf, ddim, sbuf, sdim, width, height);

        /* Read from the destination buffer in the next pass */
        sbuf = dbuf;
        sdim = ddim;
    }
}
