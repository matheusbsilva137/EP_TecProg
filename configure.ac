#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([xwc], [1.1])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([xwc.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m],[tan])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_LIB([Xpm],[XpmReadFileToPixmap],[],[
	echo "Não deu"
	exit -1])
AC_CHECK_LIB([X11],[XFlush],[],[
	echo "Preciso de libX11"
	exit -1
	])

AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
