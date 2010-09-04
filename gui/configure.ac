#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gui],
        [0.1],
        [erik.sohns@web.de],
        [gui],
        [http://www.rpg.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# get system information
AC_CANONICAL_SYSTEM

AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
# AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
AM_PROG_LIBTOOL

# init other tools.
AM_INIT_AUTOMAKE(gui, 0.1)
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

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
  CXXFLAGS="${CXXFLAGS} -g3 -O0 -march=i686 -Wall"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -O3 -march=i686"
  AC_MSG_RESULT(no)
fi

# export these variable (so Makefile substitutions
# can be made.
AC_SUBST(CXXFLAGS)

PKG_CHECK_MODULES(gui, sdl libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0)
dnl AC_SUBST(MYPROG_CFLAGS)
dnl AC_SUBST(MYPROG_LIBS)

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
