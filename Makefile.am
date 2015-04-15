
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

## libraries

noinst_LTLIBRARIES = \
    common/libcommon.la

common_libcommon_la_SOURCES = \
    common/digest.c common/digest.h \
    common/tracebuffer.c common/tracebuffer.h

lib_LTLIBRARIES = \
    libgraprof/libgraprof.la

libgraprof_libgraprof_la_SOURCES = \
    libgraprof/libgraprof.c libgraprof/libgraprof.h \
    libgraprof/instrument.c libgraprof/instrument.h \
    libgraprof/mallhooks.c libgraprof/mallhooks.h \
    libgraprof/highrestimer.c libgraprof/highrestimer.h

libgraprof_libgraprof_la_CFLAGS  = $(grapes_CFLAGS)
libgraprof_libgraprof_la_LDFLAGS = -version-info 0:0:0
libgraprof_libgraprof_la_LIBADD  = $(grapes_LIBS) ${LIBS_LIBGRAPROF} common/libcommon.la

## executables

bin_PROGRAMS = \
    graprof/graprof

graprof_graprof_SOURCES = \
    graprof/graprof.c graprof/graprof.h \
    graprof/trace.c graprof/trace.h \
    graprof/addr.c graprof/addr.h \
    graprof/function.c graprof/function.h \
    graprof/memory.c graprof/memory.h \
    graprof/flatprofile.c graprof/flatprofile.h \
    graprof/callgraph.c graprof/callgraph.h \
    graprof/strtime.c graprof/strtime.h \
    graprof/memprofile.c graprof/memprofile.h \
    graprof/bitmask.c graprof/bitmask.h \
    graprof/blocklist.c graprof/blocklist.h

    ##traceview/traceview.c traceview/traceview.h \
    ##traceview/traceview_keys.c traceview/traceview_keys.h \
    ##traceview/traceview_titlebar.c traceview/traceview_titlebar.h \
    ##traceview/traceview_footer.c traceview/traceview_footer.h \
    ##traceview/traceview_window_interactive.c traceview/traceview_window_interactive.h \
    ##traceview/traceview_window_flatprofile.c traceview/traceview_window_flatprofile.h \
    ##traceview/traceview_window_callgraph.c traceview/traceview_window_callgraph.h \
    ##traceview/traceview_window_memprofile.c traceview/traceview_window_memprofile.h \
    ##traceview/traceview_help_overlay.c traceview/traceview_help_overlay.h \
    ##traceview/traceview_scrollbar.c traceview/traceview_scrollbar.h

graprof_graprof_CFLAGS = $(grapes_CFLAGS) ${CFLAGS_GRAPROF}
graprof_graprof_LDADD  = $(grapes_LIBS) ${LIBS_GRAPROF} common/libcommon.la

## profiling builds

if PROFILING

noinst_LTLIBRARIES += \
    common/libcommon-gp.la

common_libcommon_gp_la_SOURCES = ${common_libcommon_la_SOURCES}
common_libcommon_gp_la_CFLAGS = ${common_libcommon_la_CFLAGS} -finstrument-functions -g
common_libcommon_gp_la_LIBADD = ${common_libcommon_la_LIBADD} libgraprof/libgraprof.la

noinst_PROGRAMS = \
    graprof/graprof-gp

graprof_graprof_gp_SOURCES = ${graprof_graprof_SOURCES}
graprof_graprof_gp_CFLAGS = ${graprof_graprof_CFLAGS} -finstrument-functions -g
graprof_graprof_gp_LDADD = $(grapes_LIBS) ${LIBS_GRAPROF} common/libcommon-gp.la libgraprof/libgraprof.la

endif ## PROFILING

SUBDIRS = tests docs
