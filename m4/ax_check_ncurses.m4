
AC_DEFUN([AX_CHECK_NCURSES], [
  PKG_CHECK_MODULES(ncursesw, ["ncursesw"], [
    AC_DEFINE_UNQUOTED([HAVE_NCURSES], [1], [Define to 1, if you have ncursesw])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_NCURSES], [0], [Define to 1, if you have ncursesw])
    AC_MSG_WARN([ncursesw not found - disabling tracing gui])
  ])
  LIBS_GRAPROF="${ncursesw_LIBS} ${LIBS_GRAPROF}"
  CFLAGS_GRAPROF="${ncursesw_CFLAGS} ${CFLAGS_GRAPROF}"
  AC_SUBST(LIBS_GRAPROF)
  AC_SUBST(CFLAGS_GRAPROF)
])
