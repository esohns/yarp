#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([RPG_UTest], [0.1], [erik.sohns@web.de], [RPG_UTest], [http://www.rpg.org])
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
# AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
# AM_PROG_LIBTOOL
AM_PROG_AR

# init other tools.
#AM_INIT_AUTOMAKE([gnu nostdinc -Wall])
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H([rpg_config.h], [RPG])

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
  CXXFLAGS="${CXXFLAGS} -g -O0 -Wall -D_DEBUG"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -O3 -march=native"
  AC_MSG_RESULT(no)
fi

# this macro is used to get the arguments supplied
# to the configure script (./configure --enable-tracing)
# Check if we have enable tracing support.
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

# export variables (so Makefile substitutions can be made)
AC_SUBST(META_PACKAGE)
AC_SUBST(CXXFLAGS)

# checks for libraries
PKG_CHECK_MODULES(pkg_libs, 
                  libpng sdl SDL_ttf SDL_mixer xerces-c libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0 gthread-2.0 gtk+-2.0)
dnl AC_SUBST(pkg_libs_CFLAGS)
dnl AC_SUBST(pkg_libs_LIBS)

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T

# Checks for library functions.
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 chance/Makefile
                 character/Makefile
                 character/player/Makefile
                 character/monster/Makefile
                 combat/Makefile
                 item/Makefile
                 magic/Makefile
                 map/Makefile
                 net/Makefile
                 net/protocol/Makefile
                 sound/Makefile
                 graphics/Makefile
                 client/Makefile])

AC_OUTPUT

