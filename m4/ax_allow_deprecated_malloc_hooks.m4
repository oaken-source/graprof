
AC_DEFUN([AX_ALLOW_DEPRECATED_MALLOC_HOOKS], [
  AC_ARG_ENABLE(deprecated-malloc-hooks,
  AS_HELP_STRING([--enable-deprecated-malloc-hooks], [Enable the deprecated malloc hooks instead of overriding the weak symbols. Use this when the default malloc interception does not work for you. Default: no]),
  [case "${enableval}" in
    yes) deprecatedmallochooks=1 ;;
    no)  deprecatedmallochooks=0 ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-deprecated-malloc-hooks]) ;;
  esac],
  [deprecatedmallochooks=0])

  AC_DEFINE_UNQUOTED([USE_DEPRECATED_MALLOC_HOOKS], [$deprecatedmallochooks], [enable deprecated malloc_hooks])
])

