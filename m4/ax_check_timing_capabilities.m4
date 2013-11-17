
AC_DEFUN([AX_CHECK_TIMING_CAPABILITIES], [
  LIBS_TMP=${LIBS}
  AC_SEARCH_LIBS([clock_gettime], [rt], [
    AC_DEFINE_UNQUOTED([HAVE_CLOCK_GETTIME], [1], [Define to 1, if you have the clock_gettime function])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_CLOCK_GETTIME], [0], [Define to 1, if you have the clock_gettime function])
    AC_SEARCH_LIBS([mach_absolute_time], [], [
      AC_DEFINE_UNQUOTED([HAVE_MACH_ABSOLUTE_TIME], [1], [Define to 1, if you have the mach_absolute_time_function])
    ], [
      AC_DEFINE_UNQUOTED([HAVE_MACH_ABSOLUTE_TIME], [0], [Define to 1, if you have the mach_absolute_time_function])
      AC_MSG_ERROR([No known timing mechanism found! Please file a bug containing your system specs!])
    ])
  ])
  LIBS_LIBGRAPROF=${LIBS} ${LIBS_LIBGRAPROF}
  LIBS=${LIBS_TMP}
  AC_SUBST(LIBS_LIBGRAPROF)
])

