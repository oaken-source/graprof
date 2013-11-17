
dnl process this file with autoconf to produce configure script

AC_PREREQ([2.69])
AC_INIT([graprof], [0.7], [graprof@grapentin.org])
AC_CONFIG_SRCDIR([libgraprof/libgraprof.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([AM_CFLAGS], [])

AM_INIT_AUTOMAKE([-Wall -Werror gnu])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR

AX_ALLOW_TESTBUILD
AX_ALLOW_DEBUG_MESSAGES
AX_ALLOW_DEPRECATED_MALLOC_HOOKS

AX_CHECK_TIMING_CAPABILITIES
AX_CHECK_ARGP

LT_INIT

AC_CONFIG_FILES([
  Makefile
  libgraprof/Makefile
  graprof/Makefile
  docs/Makefile])

AC_OUTPUT
