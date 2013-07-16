
dnl process this file with autoconf to produce configure script

AC_PREREQ([2.69])
AC_INIT([graprof], [0.1], [andreas@grapentin.org])
AC_CONFIG_SRCDIR([libgraprof/libgraprof.h])
AC_CONFIG_HEADERS([libgraprof/config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([scripts/m4])

AC_SUBST([AM_CFLAGS], [])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_AR
AM_PROG_LEX

LT_INIT

AC_CONFIG_FILES([
  Makefile
  libgraprof/Makefile
  graprof/Makefile])

AC_OUTPUT
