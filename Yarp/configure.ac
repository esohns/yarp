#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# define version information
YARP_SO_VERSION="0:0:0"
YARP_API_VERSION="0.1"
AC_SUBST([YARP_SO_VERSION])
AC_SUBST([YARP_API_VERSION])

AC_INIT([yarp], [YARP_API_VERSION], [eriksohns@123mail.org], [Yarp], [http://www.github.com/esohns/Yarp])
AC_PREREQ(2.63)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_HEADERS([config.h])
#                   ../tools/XML2CppCode/config.h:../tools/XML2CppCode/config.h.in])
#AC_CONFIG_LINKS([../common/rpg_common.xsd:../engine/rpg_common.xsd])
AC_CONFIG_SUBDIRS([chance
                   character
                   client
                   combat
                   common
                   engine
                   graphics
                   item
                   magic
                   map
                   net
                   sound
                   test_u
                   tools/XML2CppCode])
AC_CONFIG_MACRO_DIR([m4])

# define meta-package
META_PACKAGE="RPG"
AC_DEFINE([META_PACKAGE], ["RPG"], [meta-package])

# get system information
#AC_CANONICAL_TARGET

#AC_LANG([C++])

# checks for programs
AC_PROG_CXX
#AC_PROG_CC
#AC_PROG_CPP
#AC_PROG_INSTALL
#AC_PROG_LN_S
#AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LEX
AC_PROG_YACC
AM_PROG_AR
#PKG_PROG_PKG_CONFIG

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK

LT_INIT

# prefix config.h
AX_PREFIX_CONFIG_H([rpg_config.h], [RPG], [config.h])
#AX_PREFIX_CONFIG_H([tools/XML2CppCode/XML2CppCode-config.h], [XML2CppCode], [tools/XML2CppCode/config.h])

## (re)set the cpp compiler flags
#CXXFLAGS="-fno-implicit-templates"
CXXFLAGS=
# --> enable debug support ?
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
              [--enable-debug=[no/yes] turn on debugging [default=$debug_default]],
              ,
              enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
  CXXFLAGS="${CXXFLAGS} -g -O0 -Wall -D_DEBUG -DDEBUG_DEBUGGER"
  AC_MSG_RESULT(yes)
else
#  CXXFLAGS="-O3 -march=native -Wall -Werror -Wno-unused-variable -Wno-sign-compare"
  CXXFLAGS="${CXXFLAGS} -O3 -march=native -DNDEBUG -Wall -Werror"
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
#PKG_CHECK_MODULES(pkg_libs, xerces-c libpng sdl SDL_ttf libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0 gthread-2.0 gtk+-2.0)
#AC_SEARCH_LIBS([Mix_Init], [SDL_mixer])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lACE':
#AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])

# checks for header files
#AC_HEADER_STDBOOL

# checks for typedefs, structures, and compiler characteristics.

# checks for library functions
#AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 scripts/Makefile
                 tools/Makefile])
AC_CONFIG_FILES([Yarp-${YARP_API_VERSION}.pc:scripts/Yarp.pc.in])

AC_OUTPUT
