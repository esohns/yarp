#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([yarp], [0.1], [erik.sohns@web.de], [Yarp], [http://www.github.org/esohns/yarp])
AC_PREREQ(2.63)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_HEADERS([config.h
                   tools/XML2CppCode/config.h:tools/XML2CppCode/config.h.in])
AC_CONFIG_MACRO_DIR([m4])

# define meta-package
META_PACKAGE=RPG
AC_DEFINE([META_PACKAGE], ["RPG"], [meta-package])

# define version information
AC_SUBST([YARP_SO_VERSION], [0:0:0])
AC_SUBST([YARP_API_VERSION], [0.1])

# get system information
AC_CANONICAL_TARGET

AC_LANG([C++])

# checks for programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
AC_PROG_LEX
AC_PROG_YACC

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H([rpg_config.h], [RPG], [config.h])
#AX_PREFIX_CONFIG_H([tools/XML2CppCode/XML2CppCode-config.h], [XML2CppCode], [tools/XML2CppCode/config.h])

# reset the cpp compiler flags
CXXFLAGS=
# --> enable debug support ?
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
              [--enable-debug=[no/yes] turn on debugging [default=$debug_default]],
              ,
              enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
#  CXXFLAGS="-g -O0 -Wall -D_DEBUG"
  CXXFLAGS="-g -O0 -Wall"
  AC_MSG_RESULT(yes)
else
#  CXXFLAGS="-O3 -march=native -Wall -Werror -Wno-unused-variable -Wno-sign-compare"
  CXXFLAGS="-O3 -march=native -DNDEBUG -Wall -Werror"
  AC_MSG_RESULT(no)
fi

# --> enable tracing support ?
AC_MSG_CHECKING(whether to enable tracing)
tracing_default="no"
AC_ARG_ENABLE(tracing,
              [--enable-tracing=[no/yes] turn on tracing [default=$tracing_default]],
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
              [--enable-valgrind=[no/yes] turn on valgrind support [default=$valgrind_support_default]],
              ,
              enable_valgrind=$valgrind_support_default)
if test "x$enable_valgrind" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -DRPG_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

# export these variable (so Makefile substitutions can be made
AC_SUBST(META_PACKAGE)
AC_SUBST(CXXFLAGS)

# checks for libraries
PKG_CHECK_MODULES(pkg_libs, xerces-c libpng sdl SDL_ttf libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0 gthread-2.0 gtk+-2.0)
AC_SEARCH_LIBS([Mix_Init], [SDL_mixer])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])

# checks for header files
AC_HEADER_STDBOOL

# checks for typedefs, structures, and compiler characteristics.

# checks for library functions
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 Yarp-${YARP_API_VERSION}.pc:scripts/Yarp.pc.in
                 chance/Makefile
                 chance/dice/Makefile
                 chance/dice/scripts/Makefile
                 chance/scripts/Makefile
                 common/Makefile
                 common/scripts/Makefile
                 magic/Makefile
                 magic/scripts/Makefile
                 item/Makefile
                 item/scripts/Makefile
                 character/Makefile
                 character/scripts/Makefile
                 character/player/Makefile
                 character/player/scripts/Makefile
                 character/monster/Makefile
                 character/monster/scripts/Makefile
                 combat/Makefile
                 combat/scripts/Makefile
                 map/Makefile
                 map/data/Makefile
                 map/scripts/Makefile
                 graphics/Makefile
                 graphics/scripts/Makefile
                 graphics/data/Makefile
                 graphics/data/creatures/Makefile
                 graphics/data/cursors/Makefile
                 graphics/data/doors/Makefile
                 graphics/data/floors/Makefile
                 graphics/data/fonts/Makefile
                 graphics/data/images/Makefile
                 graphics/data/walls/Makefile
                 sound/Makefile
                 sound/scripts/Makefile
                 sound/data/Makefile
                 sound/data/ambient/Makefile
                 sound/data/effect/Makefile
                 engine/Makefile
                 engine/data/Makefile
                 engine/scripts/Makefile
                 net/Makefile
                 net/stream/Makefile
                 net/stream/scripts/Makefile
                 net/protocol/Makefile
                 net/protocol/scripts/Makefile
                 net/client/Makefile
                 net/server/Makefile
                 net/scripts/Makefile
                 client/Makefile
                 client/scripts/Makefile
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
                 test_u/sound/Makefile
                 tools/Makefile
                 tools/XML2CppCode/Makefile
                 scripts/Makefile])

AC_OUTPUT
