
AC_DEFUN([AX_CHECK_ARGP], [
  AC_CHECK_HEADER([argp.h], [
    AC_DEFINE_UNQUOTED([HAVE_ARGP_H], [1], [Define to 1, if you have the <argp.h> header])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_ARGP_H], [0], [Define to 1, if you have the <argp.h> header])
    AC_MSG_ERROR([argp.h not found - and no alternatives implemented yet.])
  ])
])
