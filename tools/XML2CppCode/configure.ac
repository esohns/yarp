#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([XML2CppCode], [0.1], [eriksohns@123mail.org], [XML2CppCode], [http://www.github.com/esohns/Yarp])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
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
#AC_PROG_RANLIB
AM_PROG_AR

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H

# reset the cpp compiler flags
CXXFLAGS=
# --> enable debug support ?
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
              [ --enable-debug=[no/yes] turn on debugging [default=$debug_default]],
              ,
              enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
  CXXFLAGS="-g -O0 -Wall -D_DEBUG"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="-O3 -march=native -Wall -Werror"
  AC_MSG_RESULT(no)
fi

# --> enable tracing support ?
AC_MSG_CHECKING(whether to enable tracing)
tracing_default="no"
AC_ARG_ENABLE(tracing,
              [ --enable-tracing=[no/yes] turn on tracing [default=$tracing_default]],
              ,
              enable_tracing=$tracing_default)
if test "x$enable_tracing" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -DRPG_NTRACE=0"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -DRPG_NTRACE=1"
  AC_MSG_RESULT(no)
fi

# --> enable valgrind support ?
AC_MSG_CHECKING(whether to enable valgrind support)
valgrind_support_default="no"
AC_ARG_ENABLE(valgrind,
              [ --enable-valgrind=[no/yes] turn on valgrind support [default=$valgrind_support_default]],
              ,
              enable_valgrind=$valgrind_support_default)
if test "x$enable_valgrind" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -DRPG_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

# export variables (so Makefile substitutions can be made)
AC_SUBST(CXXFLAGS)

# checks for libraries
# FIXME: Replace `main' with a function in `-lACEXML_Parser':
AC_CHECK_LIB([ACEXML_Parser], [main])
# FIXME: Replace `main' with a function in `-lACEXML':
AC_CHECK_LIB([ACEXML], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# checks for header files
AC_HEADER_STDBOOL

# checks for typedefs, structures, and compiler characteristics
AC_C_INLINE
AC_TYPE_SIZE_T

# checks for library functions
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
