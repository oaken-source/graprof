
dnl process this file with autoconf to produce configure script

AC_PREREQ([2.68])
AC_INIT([graprof], [0.7.2], [graprof@grapentin.org])
AC_CONFIG_SRCDIR([libgraprof/libgraprof.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([AM_CFLAGS], [])

AM_INIT_AUTOMAKE([-Wall -Werror gnu])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR

AX_ALLOW_DEBUG
AX_ALLOW_PROFILING
AX_ALLOW_LIBGRAPROF_TIMING

AX_CHECK_TIMING_CAPABILITIES
AX_CHECK_ARGP
AX_CHECK_LIBBFD
AX_CHECK_MD5
AX_CHECK_NCURSES

AX_CHECK_GRAPES

LT_INIT

AC_CONFIG_FILES([
  Makefile
  tests/Makefile
  docs/Makefile])

AC_OUTPUT
