
## process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

noinst_LTLIBRARIES = \
    grapes/libgrapes.la

grapes_libgrapes_la_SOURCES = \
    grapes/feedback.c grapes/feedback.h \
    grapes/file.c grapes/file.h \
    grapes/util.h

lib_LTLIBRARIES = \
    libgraprof/libgraprof.la

libgraprof_libgraprof_la_SOURCES = \
    libgraprof/libgraprof.c libgraprof/libgraprof.h \
    libgraprof/instrument.c libgraprof/instrument.h \
    libgraprof/mallhooks.c libgraprof/mallhooks.h \
    libgraprof/highrestimer.c libgraprof/highrestimer.h \
    libgraprof/buffer.h

libgraprof_libgraprof_la_LDFLAGS = -version-info 0:0:0
libgraprof_libgraprof_la_LIBADD = ${LIBS_LIBGRAPROF} grapes/libgrapes.la

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
    graprof/blocklist.c graprof/blocklist.h \
    graprof/traceview/traceview.c graprof/traceview/traceview.h \
    graprof/traceview/traceview_keys.c graprof/traceview/traceview_keys.h \
    graprof/traceview/traceview_titlebar.c graprof/traceview/traceview_titlebar.h \
    graprof/traceview/traceview_footer.c graprof/traceview/traceview_footer.h \
    graprof/traceview/traceview_window_interactive.c graprof/traceview/traceview_window_interactive.h \
    graprof/traceview/traceview_window_flatprofile.c graprof/traceview/traceview_window_flatprofile.h \
    graprof/traceview/traceview_window_callgraph.c graprof/traceview/traceview_window_callgraph.h \
    graprof/traceview/traceview_window_memprofile.c graprof/traceview/traceview_window_memprofile.h \
    graprof/traceview/traceview_help_overlay.c graprof/traceview/traceview_help_overlay.h \
    graprof/traceview/traceview_scrollbar.c graprof/traceview/traceview_scrollbar.h

graprof_graprof_CFLAGS = ${CFLAGS_GRAPROF}
graprof_graprof_LDADD = ${LIBS_GRAPROF} grapes/libgrapes.la

if PROFILING

noinst_LTLIBRARIES += \
    grapes/libgrapes-gp.la

grapes_libgrapes_gp_la_SOURCES = ${grapes_libgrapes_la_SOURCES}
grapes_libgrapes_gp_la_CFLAGS = ${grapes_libgrapes_la_CFLAGS} -finstrument-functions -g
grapes_libgrapes_gp_la_LIBADD = ${grapes_libgrapes_la_LIBADD} libgraprof/libgraprof.la

noinst_PROGRAMS = \
    graprof/graprof-gp

graprof_graprof_gp_SOURCES = ${graprof_graprof_SOURCES}
graprof_graprof_gp_CFLAGS = ${graprof_graprof_CFLAGS} -finstrument-functions -g
graprof_graprof_gp_LDADD = ${graprof_graprof_LDADD} grapes/libgrapes-gp.la libgraprof/libgraprof.la

endif ## PROFILING

SUBDIRS = docs
