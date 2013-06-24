#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([XML2CppCode], [0.1], [erik.sohns@web.de],[XML2CppCode],[http://www.rpg.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([XML_parser.cpp])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# get system information
AC_CANONICAL_TARGET

AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl AC_PROG_RANLIB

# init other tools.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H

# get the cpp compiler flags
#CXXFLAGS="${CXXFLAGS=}"
# reset the cpp compiler flags
CXXFLAGS=
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
#  CXXFLAGS="${CXXFLAGS} -g3 -O0 -Wall"
  CXXFLAGS="-g3 -O0 -Wall"
  AC_MSG_RESULT(yes)
else
#  CXXFLAGS="${CXXFLAGS} -O3 -march=native"
  CXXFLAGS="-O3 -march=i686"
  AC_MSG_RESULT(no)
fi

# export variables (so Makefile substitutions can be made)
AC_SUBST(CXXFLAGS)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lACEXML_Parser':
AC_CHECK_LIB([ACEXML_Parser], [main])
# FIXME: Replace `main' with a function in `-lACEXML':
AC_CHECK_LIB([ACEXML], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([strerror])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
