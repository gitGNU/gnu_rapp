#!/bin/sh
# To rebuild everything requiring autotools, just run "./autogen.sh".
autoreconf --install || exit
# All config.h.in will be updated as needed, but their timestamp
# won't, if there's no change in the contents.  We update the
# timestamps manually, so the auto* rules won't trig when we run
# "make" (breaking e.g. "make distcheck").
find . -name config.h.in -exec touch '{}' ';'
