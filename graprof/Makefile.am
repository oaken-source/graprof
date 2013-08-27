
## process this file with automake to produce Makefile.in

bin_PROGRAMS = graprof graprof-gp graprof-pg

graprof_SOURCES = graprof.c graprof.h trace.c trace.h addr.c addr.h function.c function.h memory.c memory.h flatprofile.c flatprofile.h callgraph.c callgraph.h strtime.c strtime.h memprofile.c memprofile.h

graprof_LDADD = -lgrapes -lbfd

graprof_gp_SOURCES = ${graprof_SOURCES}
graprof_gp_LDADD = -lgrapes -lbfd -lgraprof
graprof_gp_CFLAGS = -finstrument-functions -g

graprof_pg_SOURCES = ${graprof_SOURCES}
graprof_pg_LDADD = -lgrapes -lbfd
graprof_pg_CFLAGS = -pg -g
