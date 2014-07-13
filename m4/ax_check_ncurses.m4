
AC_DEFUN([AX_CHECK_NCURSES], [
  PKG_CHECK_MODULES(ncurses, ["ncurses"], [
    AC_DEFINE_UNQUOTED([HAVE_NCURSES], [1], [Define to 1, if you have ncurses])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_NCURSES], [0], [Define to 1, if you have ncurses])
    AC_MSG_WARN([ncurses not found - disabling tracing gui])
  ])
  LIBS_GRAPROF="${ncurses_LIBS} ${LIBS_GRAPROF}"
  CFLAGS_GRAPROF="${ncurses_CFLAGS} ${CFLAGS_GRAPROF}"
  AC_SUBST(LIBS_GRAPROF)
  AC_SUBST(CFLAGS_GRAPROF)
])
