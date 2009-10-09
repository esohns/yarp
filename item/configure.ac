#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(item, 0.1, erik.sohns@web.de)
AC_CONFIG_SRCDIR([rpg_item_common.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl AC_PROG_RANLIB
AC_PROG_MKDIR_P

# init other tools.
AM_INIT_AUTOMAKE(item, 0.1)
LT_INIT

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lACEXML':
AC_CHECK_LIB([ACEXML], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
