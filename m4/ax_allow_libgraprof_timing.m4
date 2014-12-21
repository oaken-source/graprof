
# Allow for libgraprof timing hooks to be enabled
AC_DEFUN([AX_ALLOW_LIBGRAPROF_TIMING], [
  AC_ARG_ENABLE(libgraprof-timing,
    AS_HELP_STRING([--enable-libgraprof-timing], [Enable libgraprof to output its timing behaviour. Should only be enabled for evaluation builds. Default: no]),
    [case "${enableval}" in
      yes) libgraprof_timing=1 ;;
      no)  libgraprof_timing=0 ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-libgraprof-timing]) ;;
    esac],
    [libgraprof_timing=0])

  AC_DEFINE_UNQUOTED([LIBGRAPROF_TIMING], [$libgraprof_timing], [libgraprof timing output])
])
