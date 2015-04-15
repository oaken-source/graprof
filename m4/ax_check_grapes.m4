
# Allow to check for the grapes library
AC_DEFUN([AX_CHECK_GRAPES], [
  PKG_CHECK_MODULES([grapes], [grapes], [
    ], [
      AC_MSG_ERROR([missing required grapes library])
    ])
  AC_SUBST([grapes_CFLAGS])
  AC_SUBST([grapes_LIBS])
])
