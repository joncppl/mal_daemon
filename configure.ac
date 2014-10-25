#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_include(libcurl.m4)
m4_include(mysql.m4)
AC_PREREQ([2.68])
AC_INIT([mal_daemon], [0.1], [joncpl@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_ERROR([POSIX thread library is required])])
LIBCURL_CHECK_CONFIG([],[7.32],[],[AC_MSG_ERROR([libcurl is required])])


CXXFLAGS="$CXXFLAGS $LIBCURL_CPPFLAGS"
CFLAGS="$CFLAGS $LIBCURL_CPPFLAGS"
LIBS="$LIBS $LIBCURL"

# Checks for header files.
AC_CHECK_HEADERS([ \
	stdio.h \
	stdlib.h \
	stdarg.h \
	time.h \
	string.h 
	])
	
AC_CHECK_HEADER([pthread.h],[AC_DEFINE(PTHREAD_HAVE, [1] ,[ Define to 1 if you have the <pthread.h> header file])])

MYSQL_CLIENT([],[client])
CFLAGS="$CFLAGS $MYSQL_CLIENT_CFLAGS"
LIBS="$LIBS $MYSQL_CLIENT_LIBS"

PKG_CHECK_MODULES([LIBCONFIG], [libconfig >= 1.4],,
	AC_MSG_ERROR([libconfig 1.4 or newer not found.])
)
CFLAGS="$CFLAGS $LIBCONFIG_CFLAGS"
LIBS="$LIBS $LIBCONFIG_LIBS"

PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= 2.4,,
	AC_MSG_ERROR([libxml-2.0 2.4 or newer not found.])
)
CFLAGS="$CFLAGS $LIBXML_FLAGS"
LIBS="$LIBS $LIBXML_LIBS"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
