
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

## libraries

noinst_LTLIBRARIES = \
    grapes/libgrapes.la \
    common/libcommon.la

grapes_libgrapes_la_SOURCES = \
    grapes/feedback.c grapes/feedback.h \
    grapes/file.c grapes/file.h \
    grapes/util.h

common_libcommon_la_SOURCES = \
    common/md5.c common/md5.h

common_libcommon_la_LIBADD = grapes/libgrapes.la

lib_LTLIBRARIES = \
    libgraprof/libgraprof.la

libgraprof_libgraprof_la_SOURCES = \
    libgraprof/libgraprof.c libgraprof/libgraprof.h \
    libgraprof/instrument.c libgraprof/instrument.h \
    libgraprof/mallhooks.c libgraprof/mallhooks.h \
    libgraprof/highrestimer.c libgraprof/highrestimer.h \
    libgraprof/tracebuffer.c libgraprof/tracebuffer.h

libgraprof_libgraprof_la_LDFLAGS = -version-info 0:0:0
libgraprof_libgraprof_la_LIBADD = ${LIBS_LIBGRAPROF} common/libcommon.la

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

graprof_graprof_CFLAGS = ${CFLAGS_GRAPROF}
graprof_graprof_LDADD = ${LIBS_GRAPROF} grapes/libgrapes.la common/libcommon.la

## profiling builds

if PROFILING

noinst_LTLIBRARIES += \
    grapes/libgrapes-gp.la \
    common/libcommon-gp.la

grapes_libgrapes_gp_la_SOURCES = ${grapes_libgrapes_la_SOURCES}
grapes_libgrapes_gp_la_CFLAGS = ${grapes_libgrapes_la_CFLAGS} -finstrument-functions -g
grapes_libgrapes_gp_la_LIBADD = ${grapes_libgrapes_la_LIBADD} libgraprof/libgraprof.la

common_libcommon_gp_la_SOURCES = ${common_libcommon_la_SOURCES}
common_libcommon_gp_la_CFLAGS = ${common_libcommon_la_CFLAGS} -finstrument-functions -g
common_libcommon_gp_la_LIBADD = ${common_libcommon_la_LIBADD} libgraprof/libgraprof.la

noinst_PROGRAMS = \
    graprof/graprof-gp

graprof_graprof_gp_SOURCES = ${graprof_graprof_SOURCES}
graprof_graprof_gp_CFLAGS = ${graprof_graprof_CFLAGS} -finstrument-functions -g
graprof_graprof_gp_LDADD = ${LIBS_GRAPROF} grapes/libgrapes-gp.la common/libcommon-gp.la libgraprof/libgraprof.la

endif ## PROFILING

SUBDIRS = tests docs
