
AC_DEFUN([AX_CHECK_LIBBFD], [
  LIBS_TMP=${LIBS}
  AC_SEARCH_LIBS([bfd_openr], [bfd], [
    AC_DEFINE_UNQUOTED([HAVE_LIBBFD], [1], [Define to 1, if you have bfd_openr function])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_LIBBFD], [0], [Define to 1, if you have bfd_openr function])
    AC_MSG_ERROR([no libbfd with bfd_openr found])
  ])
  AC_MSG_WARN([LIBCHECK: libs is '${LIBS_GRAPROF}'])
  LIBS_GRAPROF="${LIBS} ${LIBS_GRAPROF}"
  AC_MSG_WARN([LIBCHECK: libs is '${LIBS_GRAPROF}'])
  LIBS="${LIBS_TMP}"
  AC_SUBST(LIBS_GRAPROF)
])
