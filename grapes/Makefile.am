
## process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libgrapes.la

libgrapes_la_SOURCES = feedback.c feedback.h file.c file.h util.h

if TESTBUILD

noinst_LTLIBRARIES += libgrapes-gp.la

libgrapes_gp_la_SOURCES = ${libgrapes_la_SOURCES}
libgrapes_gp_la_CFLAGS = ${libgrapes_la_CFLAGS} -finstrument-functions -g

endif ## TESTBUILD
