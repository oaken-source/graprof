
## process this file with automake to produce Makefile.in

EXTRA_DIST = \
    asparagus.exp \
    lib/feedback.tcl lib/globals.tcl lib/string.tcl \
    steps/dispatcher.tcl steps/custom.tcl steps/default.tcl \
    overhead.tests/overhead.exp

AUTOMAKE_OPTIONS = dejagnu

DEJAGNU  = ${top_srcdir}/tests/asparagus.exp
DEJATOOL = overhead

libdir   = $(top_builddir)/libgraprof/.libs/

export DEJAGNU libdir
