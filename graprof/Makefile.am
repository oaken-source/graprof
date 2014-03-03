
## process this file with automake to produce Makefile.in

bin_PROGRAMS = graprof

graprof_SOURCES = graprof.c graprof.h trace.c trace.h addr.c addr.h function.c function.h memory.c memory.h flatprofile.c flatprofile.h callgraph.c callgraph.h strtime.c strtime.h memprofile.c memprofile.h bitmask.c bitmask.h blocklist.c blocklist.h traceview.c traceview.h

graprof_LDADD = ${LIBS_GRAPROF} ${top_builddir}/grapes/libgrapes.la
graprof_CFLAGS = ${CFLAGS_GRAPROF}

if TESTBUILD

noinst_PROGRAMS = graprof-gp

graprof_gp_SOURCES = ${graprof_SOURCES}
graprof_gp_LDADD = ${graprof_LDADD} ../libgraprof/libgraprof.la
graprof_gp_CFLAGS = ${graprof_CFLAGS} -finstrument-functions -g

endif ## TESTBUILD
