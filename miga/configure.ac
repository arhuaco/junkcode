AC_INIT(miga, 0.1)
AC_CONFIG_SRCDIR(src/miga.h)
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S 

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(assert.h)
AC_CHECK_HEADERS(math.h)
AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(time.h)

dnl this is enabled by default.
AC_ARG_ENABLE(debug, [  --enable-debug          turn on debugging [default=no]],
if test "x$enableval" = "xyes"; then
  CFLAGS="-g $CFLAGS"
fi)

AC_ARG_ENABLE(warnings, [  --enable-warnings       turn on warnings [default=yes]],
if test "x$enableval" = "xyes"; then
  CFLAGS="-Wall -pedantic $CFLAGS"
fi)

AC_ARG_ENABLE(efence, [  --enable-efence         link against efence [default=no]],
if test "x$enableval" = "xyes"; then
  AC_CHECK_LIB(efence, malloc)
fi)

dnl AC_CHECK_FUNCS(strdup malloc qsort printf)

AC_OUTPUT(Makefile src/Makefile)
