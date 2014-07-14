
## process this file with automake to produce Makefile.in

bin_PROGRAMS = graprof

graprof_SOURCES = graprof.c graprof.h trace.c trace.h addr.c addr.h function.c function.h memory.c memory.h flatprofile.c flatprofile.h callgraph.c callgraph.h strtime.c strtime.h memprofile.c memprofile.h bitmask.c bitmask.h blocklist.c blocklist.h

graprof_SOURCES += traceview/traceview.c traceview/traceview.h traceview/traceview_titlebar.c traceview/traceview_titlebar.h traceview/traceview_footer.c traceview/traceview_footer.h traceview/traceview_window_interactive.c traceview/traceview_window_interactive.h traceview/traceview_window_flatprofile.c traceview/traceview_window_flatprofile.h traceview/traceview_window_callgraph.c traceview/traceview_window_callgraph.h traceview/traceview_window_memprofile.c traceview/traceview_window_memprofile.h

graprof_LDADD = ${LIBS_GRAPROF} ${top_builddir}/grapes/libgrapes.la
graprof_CFLAGS = ${CFLAGS_GRAPROF}

if TESTBUILD

noinst_PROGRAMS = graprof-gp

graprof_gp_SOURCES = ${graprof_SOURCES}
graprof_gp_LDADD = ${LIBS_GRAPROF} ${top_builddir}/grapes/libgrapes-gp.la ../libgraprof/libgraprof.la
graprof_gp_CFLAGS = ${graprof_CFLAGS} -finstrument-functions -g

endif ## TESTBUILD
