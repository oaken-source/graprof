
dnl process this file with autoconf to produce configure script

AC_PREREQ([2.69])
AC_INIT([graprof], [0.5], [graprof@grapentin.org])
AC_CONFIG_SRCDIR([libgraprof/libgraprof.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([scripts/m4])

AC_SUBST([AM_CFLAGS], [])

AM_INIT_AUTOMAKE([-Wall -Werror gnu])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AM_PROG_LEX

# Allow for a self-profiling test build to be enabled at configure time
AC_ARG_ENABLE(testbuild,
AS_HELP_STRING([--enable-testbuild],
               [Build and install instrumented versions of graprof used for self-profiling. This is mainly useful for developers working on graprof itself. Default: no]),
[case "${enableval}" in
	yes) testbuild=true ;;
	no)  testbuild=false ;;
	*)   AC_MSG_ERROR([bad value ${enableval} for --enable-testbuild]) ;;
esac],
[testbuild=false])

AM_CONDITIONAL(TESTBUILD, test x"$testbuild" = x"true")

# Allow for deeper error messaging to be enabled at configure time
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [Enable output of failed assertions that may or may not lead to errors that surface on stderr. Useful for debugging, and for submitting bug reports. Default: no]),
[case "${enableval}" in
	yes) debug=1 ;;
	no)  debug=0 ;;
	*)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=0])

AC_DEFINE_UNQUOTED([DEBUG], [$debug], [increased debug output])

# Allow for explicit memory free mode to be enabled at configure time
AC_ARG_ENABLE(explicit-free,
AS_HELP_STRING([--enable-explicit-free],
               [Enable routines that explictly free all known memory locations. This is only useful to debug memory leaks in graprof itself. Default: no]),
[case "${enableval}" in
	yes) explicitfree=1 ;;
	no)  explicitfree=0 ;;
	*)   AC_MSG_ERROR([bad value ${enableval} for --enable-explicit-free]) ;;
esac],
[explicitfree=0])

AC_DEFINE_UNQUOTED([FREE_ALL_MEMORY_EXPLICITLY], [$explicitfree], [explicitly free known memory on exit])
 
# Allow for deprecated malloc hooks to be enabled at configure time
AC_ARG_ENABLE(deprecated-malloc-hooks,
AS_HELP_STRING([--enable-deprecated-malloc-hooks],
               [Enable the deprecated malloc hooks instead of overriding the weak symbols. Use this when the default malloc interception does not work for you. Default: no]),
[case "${enableval}" in
	yes) deprecatedmallochooks=1 ;;
	no)  deprecatedmallochooks=0 ;;
	*)   AC_MSG_ERROR([bad value ${enableval} for --enable-deprecated-malloc-hooks]) ;;
esac],
[deprecatedmallochooks=0])

AC_DEFINE_UNQUOTED([USE_DEPRECATED_MALLOC_HOOKS], [$deprecatedmallochooks], [enable deprecated malloc_hooks])

LT_INIT

AC_CONFIG_FILES([
  Makefile
  libgraprof/Makefile
  graprof/Makefile
  docs/Makefile])

AC_OUTPUT
