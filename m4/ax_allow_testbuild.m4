
AC_DEFUN([AX_ALLOW_TESTBUILD], [
  AC_ARG_ENABLE(testbuild,
  AS_HELP_STRING([--enable-testbuild], [Build and install instrumented versions of graprof used for self-profiling. This is mainly useful for developers working on graprof itself. Default: no]),
  [case "${enableval}" in
    yes) testbuild=true ;;
    no)  testbuild=false ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-testbuild]) ;;
  esac],
  [testbuild=false])

  AM_CONDITIONAL(TESTBUILD, test x"$testbuild" = x"true")
])
