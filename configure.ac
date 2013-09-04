
dnl process this file with autoconf to produce configure script

AC_PREREQ([2.69])
AC_INIT([graprof], [0.5], [andreas@grapentin.org])
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

AC_DEFINE([DEBUG], [1], [This causes error messages to usually print their origins])
AC_DEFINE([FREE_ALL_MEMORY_EXPLICITLY], [1], [Useful for debugging memory leaks.])
AC_DEFINE([USE_DEPRECATED_MALLOC_HOOKS], [1], [Do not redefine malloc and friends, but use deprecated malloc_hooks instead])

LT_INIT

AC_CONFIG_FILES([
  Makefile
  libgraprof/Makefile
  graprof/Makefile
  docs/Makefile])

AC_OUTPUT
