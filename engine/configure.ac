#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([RPG_Engine], [0.1], [eriksohns@123mail.org], [RPG_Engine], [http://www.github.com/esohns/Yarp])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
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
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
#AC_PROG_RANLIB
AM_PROG_AR
AM_PROG_LIBTOOL

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
CXXFLAGS="-DRPG_ENGINE_BUILD_DLL"
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
  CXXFLAGS="-g3 -O0 -Wall ${CXXFLAGS}"
  AC_MSG_RESULT(yes)
else
#  CXXFLAGS="${CXXFLAGS} -O3 -march=native -DRPG_NTRACE=0"
  CXXFLAGS="-O3 -march=i686 -DRPG_NTRACE=0 ${CXXFLAGS}"
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

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([modf])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 scripts/Makefile])

AC_OUTPUT
