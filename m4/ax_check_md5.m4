
AC_DEFUN([AX_CHECK_MD5], [
  AC_SEARCH_LIBS([MD5], [crypto], [
    AC_DEFINE_UNQUOTED([HAVE_OPENSSL_MD5], [1], [Define to 1, if you have <openssl/md5.h>])
    AC_DEFINE_UNQUOTED([DIGEST_LENGTH], [MD5_DIGEST_LENGTH], [Value to use for md5 digest length])
  ], [
    AC_DEFINE_UNQUOTED([HAVE_OPENSSL_MD5], [0], [Define to 1, if you have <openssl/md5.h>])
    AC_SEARCH_LIBS([MD5Data], [bsd], [
      AC_DEFINE_UNQUOTED([DIGEST_LENGTH], [MD5_DIGEST_STRING_LENGTH], [Value to use for md5 digest length])
      AC_DEFINE_UNQUOTED([HAVE_BSD_MD5], [1], [Define to 1, if you have <bsd/md5.h>])
    ], [
      AC_DEFINE_UNQUOTED([DIGEST_LENGTH], [16], [Value to use for md5 digest length])
      AC_DEFINE_UNQUOTED([HAVE_BSD_MD5], [0], [Define to 1, if you have <bsd/md5.h>])
      AC_MSG_WARN([No usable md5 implementation found.])
    ])
  ])
])
