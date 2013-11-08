#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([yarp], [0.1], [erik.sohns@web.de], [Yarp], [http://www.yarp.org])
AC_PREREQ(2.63)
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

# init other tools.
#AM_INIT_AUTOMAKE([gnu nostdinc -Wall])
AM_INIT_AUTOMAKE(yarp, 0.1)
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
  CXXFLAGS="${CXXFLAGS} -g -O0 -Wall"
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

# export these variable (so Makefile substitutions
# can be made.
AC_SUBST(META_PACKAGE)
AC_SUBST(CXXFLAGS)

# checks for libraries
PKG_CHECK_MODULES(pkg_libs, ACE xerces-c sdl SDL_ttf libpng libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0 gthread-2.0 gtk+-2.0)
dnl AC_SUBST(pkg_libs_CFLAGS)
dnl AC_SUBST(pkg_libs_LIBS)

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lACE':
#AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 chance/Makefile
                 chance/dice/Makefile
                 common/Makefile
                 magic/Makefile
                 item/Makefile
                 character/Makefile
                 character/player/Makefile
                 character/monster/Makefile
                 combat/Makefile
                 map/Makefile
                 map/scripts/Makefile
                 graphics/Makefile
                 graphics/data/Makefile
                 graphics/data/creatures/Makefile
                 graphics/data/cursors/Makefile
                 graphics/data/doors/Makefile
                 graphics/data/floors/Makefile
                 graphics/data/fonts/Makefile
                 graphics/data/images/Makefile
                 graphics/data/walls/Makefile
                 sound/Makefile
                 sound/data/Makefile
                 sound/data/ambient/Makefile
                 sound/data/effect/Makefile
                 engine/Makefile
                 net/Makefile
                 net/stream/Makefile
                 net/protocol/Makefile
                 net/protocol/scripts/Makefile
                 net/client/Makefile
                 client/Makefile
                 test_u/Makefile
                 test_u/chance/Makefile
                 test_u/character/Makefile
								 test_u/character/player/Makefile
								 test_u/character/monster/Makefile
                 test_u/client/Makefile
                 test_u/combat/Makefile
                 test_u/graphics/Makefile
								 test_u/item/Makefile
                 test_u/magic/Makefile
                 test_u/map/Makefile
                 test_u/net/Makefile
                 test_u/net/protocol/Makefile
								 test_u/sound/Makefile])

AC_OUTPUT
