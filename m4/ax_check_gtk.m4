
AC_DEFUN([AX_CHECK_GTK], [
  PKG_CHECK_MODULES(GTK, ["gtk+-2.0"], [
    AC_DEFINE_UNQUOTED([HAVE_GTK], [1], [Define to 1, if you have gtk+-2.0])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_GTK], [0], [Define to 1, if you have gtk+-2.0])
    AC_MSG_WARN([gtk+-2.0 not found - disabling tracing gui])
  ])
  LIBS_GRAPROF="${GTK_LIBS} ${LIBS_GRAPROF}"
  CFLAGS_GRAPROF="${GTK_CFLAGS} ${CFLAGS_GRAPROF}"
  AC_SUBST(LIBS_GRAPROF)
  AC_SUBST(CFLAGS_GRAPROF)
])
