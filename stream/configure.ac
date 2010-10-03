#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([RPG_Stream], [0.1], [erik.sohns@web.de], [RPG_Stream], [http://www.rpg.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([rpg_stream.h])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# define meta-package
META_PACKAGE=RPG
AC_DEFINE([META_PACKAGE], ["RPG"], [meta-package])

# get system information
AC_CANONICAL_TARGET

AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
# AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
# AM_PROG_LIBTOOL

# init other tools.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H

# get the cpp compiler flags
CXXFLAGS="${CXXFLAGS=}"
# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-debug)
# Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
              [ --enable-debug=[no/yes] turn on debugging [default=$debug_default]],
              ,
              enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -g -march=i686 -Wall"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -O3 -march=i686 -DRPG_NTRACE=0"
  AC_MSG_RESULT(no)
fi

# export variables (so Makefile substitutions can be made)
AC_SUBST(META_PACKAGE)
AC_SUBST(CXXFLAGS)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])

# Checks for header files.
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TIMEZONE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([localtime_r])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([tzset])
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
