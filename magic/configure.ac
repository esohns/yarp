#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([RPG_Magic], [0.1], [eriksohns@123mail.org], [RPG_Magic], [http://www.github.com/esohns/Yarp])
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

## (re)set the cpp compiler flags
#CXXFLAGS_DEBUG="-fno-implicit-templates"
CXXFLAGS_DEBUG=
# --> enable debug support ?
AC_MSG_CHECKING([enable debugging])
debug_default="no"
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [turn on debugging [default="${debug_default}"]])],
              [case "${enableval}" in
                 yes) debug=yes ;;
                 no)  debug=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
               esac],
              [debug=$debug_default])
AM_CONDITIONAL([DEBUG], [test x$debug = xyes])
if test x$debug = xyes; then
  CXXFLAGS_DEBUG="-fno-inline -g -O0 -Wall -D_DEBUG"
  AC_MSG_RESULT([yes])
else
  CXXFLAGS_DEBUG="-O3 -Werror -DACE_NDEBUG"
  AC_MSG_RESULT([no])
fi

# export variables (so Makefile substitutions can be made)
AC_SUBST(META_PACKAGE)
AC_SUBST(CXXFLAGS)

# Checks for libraries.
PKG_CHECK_MODULES(pkg_libs, xerces-c)
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 scripts/Makefile])

AC_OUTPUT
