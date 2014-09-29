/*  Copyright (C) 2005-2012, 2014 Axis Communications AB, LUND, SWEDEN
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
 *  @file   rapp_main.c
 *  @brief  RAPP library initialization.
 */

#include "rapp_main.h"
#include "rapp_api.h"
#include "rapp_util.h"
#include "rapp_error.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * -------------------------------------------------------------
 *  Exported functions
 * -------------------------------------------------------------
 */

int rc_initialized = 0;

/**
 *  Initialize the library. Empty for now, except for verification that
 *  it hasn't been called before, not without a matching call to
 *  rapp_terminate.
 */
RAPP_API(void, rapp_initialize, (void))
{
    if (RAPP_INITIALIZED()) {
        return;
    }

    rc_initialized = 1;
}

/**
 *  Finish working with the library. Empty for now, except for
 *  book-keeping that there has been a call.
 */
RAPP_API(void, rapp_terminate, (void))
{
    rc_initialized = 0;
}

#ifdef RAPP_LOGGING

/**
 *  Log a call to a RAPP API function.
 */
void rapp_log_rappcall(const char fname[], const struct timeval tv[],
                       const char retformat[], const char argformats[],
                       ...)
{
    static uint64_t t0;
    uint64_t td, tb;
    static FILE *logfile;
    va_list ap;
    const char *logformat;
    int ret;
    static char modf_format[(RAPP_LOG_MAX_RETURN_FORMAT_LENGTH +
                             RAPP_LOG_MAX_ARGS_FORMAT_LENGTH +
                             sizeof ") = \n")];

    if (logfile == NULL) {
        char *logfilebase = getenv("RAPP_LOGFILE");
        char *tmpdir = getenv("TMPDIR");
        size_t tmpnamsiz;
        char *logfilenam;
        bool this_is_rapp_initialize = strcmp(fname, "rapp_initialize") == 0;

        assert(this_is_rapp_initialize);
        if (!this_is_rapp_initialize)
            return;

        if (logfilebase == NULL)
            logfilebase = "rapp_log.txt";

        if (logfilebase[0] == '/')
           tmpdir = "";

        if (tmpdir == NULL)
           tmpdir = "/tmp";

        tmpnamsiz = strlen(tmpdir) + 1 + strlen(logfilebase) + 1;
        logfilenam = malloc(tmpnamsiz);
        assert(logfilenam != NULL);
        if (logfilenam == NULL)
            return;

        sprintf(logfilenam, "%s/%s", tmpdir, logfilebase);
        logfile = fopen(logfilenam, "a");
        free(logfilenam);
        assert(logfile != NULL);
        if (logfile == NULL)
            return;

        t0 = tv[0].tv_sec * 1000000ULL + tv[0].tv_usec;
    }

    tb = tv[0].tv_sec * 1000000ULL + tv[0].tv_usec;
    td = tv[1].tv_sec * 1000000ULL + tv[1].tv_usec - tb;
    tb -= t0;
    ret = fprintf(logfile, "%lu.%06lu..%lu.%06lu %s(",
                  (unsigned long)(tb / 1000000), (unsigned long)(tb % 1000000),
                  (unsigned long)(td / 1000000), (unsigned long)(td % 1000000),
                  fname);
    assert(ret > 0);

    va_start(ap, argformats);

    if (*retformat == 0) {
        /* A void function. Just use argformats as-is. */
        logformat = argformats;
    } else {
        /**
         *  A non-void function. Re-format argformats to reflect that the
         *  last passed stdarg value is the return-value.
         */
        sprintf(modf_format, "%s) = %s\n", argformats, retformat);
        logformat = modf_format;
    }

    ret = vfprintf(logfile, logformat, ap);
    assert(ret >= 0);

    if (*retformat == 0) {
        /* A void function; terminate the presentation of the argument list. */
        fprintf(logfile, ")\n");
    }

    va_end(ap);

    /**
     * This is needed to pacify set-but-unused warnings, as we happen to
     * use "ret" only in asserts, and assert(x) isn't required to
     * evaluate x.
     */
    (void)ret;

    if (strcmp(fname, "rapp_terminate") == 0) {
        /* We shouldn't get any further calls. */
       ret = fclose(logfile);
       assert(ret == 0);
       logfile = NULL;
    }
}

#endif /* RAPP_LOGGING */
