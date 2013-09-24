
## process this file with automake to produce Makefile.in

bin_PROGRAMS = graprof

graprof_SOURCES = graprof.c graprof.h trace.c trace.h addr.c addr.h function.c function.h memory.c memory.h flatprofile.c flatprofile.h callgraph.c callgraph.h strtime.c strtime.h memprofile.c memprofile.h grapes/feedback.c grapes/feedback.h grapes/util.h

graprof_LDADD = -lbfd -lcrypto

if TESTBUILD

bin_PROGRAMS += graprof-gp graprof-pg

graprof_gp_SOURCES = ${graprof_SOURCES}
graprof_gp_LDADD = ${graprof_LDADD} -lgraprof
graprof_gp_CFLAGS = -finstrument-functions -g

graprof_pg_SOURCES = ${graprof_SOURCES}
graprof_pg_LDADD = ${graprof_LDADD}
graprof_pg_CFLAGS = -pg -g

endif ## TESTBUILD
