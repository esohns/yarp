#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([RPG_Graphics], [0.1], [erik.sohns@web.de], [RPG_Graphics], [http://www.rpg.org])
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

# checks for programs
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
dnl AC_PROG_RANLIB
AC_PROG_MKDIR_P
AM_PROG_AR

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H

# get the cpp compiler flags
#CXXFLAGS="${CXXFLAGS=}"
# reset the cpp compiler flags
CXXFLAGS="-DRPG_GRAPHICS_BUILD_DLL"
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

# checks for libraries
# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lSDL_ttf':
#AC_CHECK_LIB([SDL_ttf], [main])
PKG_CHECK_MODULES(pkg_libs, libpng sdl SDL_ttf)

# checks for header files
AC_HEADER_STDBOOL

# checks for typedefs, structures, and compiler characteristics
AC_C_INLINE

# checks for library functions
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 data/creatures/Makefile
                 data/cursors/Makefile
                 data/doors/Makefile
                 data/floors/Makefile
                 data/fonts/Makefile
                 data/images/Makefile
                 data/walls/Makefile
                 scripts/Makefile])

AC_OUTPUT

