
# Allow for a graprof-profiled test build to be enabled at configure time
AC_DEFUN([AX_ALLOW_PROFILING], [
  AC_ARG_ENABLE(profiling,
    AS_HELP_STRING([--enable-profiling], [Build and install instrumented versions of grapes used for profiling with graprof. This is mainly useful for developers working on grapes itself. Default: no]),
    [case "${enableval}" in
      yes) profiling=true ;;
      no)  profiling=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-profiling]) ;;
    esac],
    [profiling=false])

  AM_CONDITIONAL(PROFILING, test x"$profiling" = x"true")
])


