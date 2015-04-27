#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# version
m4_define([M4_YARP_VERSION_MAJOR], [0])
m4_define([M4_YARP_VERSION_MINOR], [0])
m4_define([M4_YARP_VERSION_MICRO], [1])
m4_define([M4_YARP_VERSION_DEVEL], [devel])
m4_define([M4_YARP_VERSION],
          [M4_YARP_VERSION_MAJOR.M4_YARP_VERSION_MINOR.M4_YARP_VERSION_MICRO[]m4_ifset([M4_YARP_VERSION_DEVEL],-[M4_YARP_VERSION_DEVEL])])
m4_define([M4_YARP_VERSION_API], [1])

# libtool versioning
m4_define([M4_YARP_LT_VERSION_AGE], [0])
m4_define([M4_YARP_LT_VERSION_CURRENT], [M4_YARP_VERSION_MAJOR])
m4_define([M4_YARP_LT_VERSION_REVISION], [m4_eval(100 * M4_YARP_VERSION_MAJOR + 10 * M4_YARP_VERSION_MINOR + M4_YARP_VERSION_MICRO)])
m4_define([M4_YARP_LT_VERSION_INFO],
          [M4_YARP_LT_VERSION_CURRENT:M4_YARP_LT_VERSION_REVISION:M4_YARP_LT_VERSION_AGE])

# version suffix
m4_ifset([M4_YARP_VERSION_DEVEL],
         [AC_DEFINE(HAVE_VERSION_DEVEL, [1], [define if a version suffix is present])])

# (meta-)package name
m4_define([M4_META_PACKAGE_NAME], [Yarp])

AC_INIT([M4_META_PACKAGE_NAME],
        [M4_YARP_VERSION],
        [eriksohns@123mail.org],
        [M4_META_PACKAGE_NAME],
        [http://www.github.com/esohns/Yarp])
AC_PREREQ(2.63)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_HEADERS([config.h])
#                   ../tools/XML2CppCode/config.h:../tools/XML2CppCode/config.h.in])
#AC_CONFIG_LINKS([../common/rpg_common.xsd:../engine/rpg_common.xsd])
AC_CONFIG_SUBDIRS([modules/libCommon
                   modules/libACEStream
                   modules/libACENetwork
                   tools/XML2CppCode
                   chance
                   common
                   magic
                   item
                   character
                   combat
                   map
                   net
                   engine
                   sound
                   graphics
                   client
                   test_u])
AC_CONFIG_MACRO_DIR([m4])

YARP_VERSION_API=M4_LIBCOMMON_VERSION_API
AC_SUBST([YARP_VERSION_API])

YARP_LT_VERSION_INFO=M4_YARP_LT_VERSION_INFO
AC_SUBST([YARP_LT_VERSION_INFO])

YARP_VERSION=M4_YARP_VERSION
AC_SUBST([YARP_VERSION])
YARP_VERSION_MAJOR=M4_YARP_VERSION_MAJOR
AC_SUBST([YARP_VERSION_MAJOR])
YARP_VERSION_MINOR=M4_YARP_VERSION_MINOR
AC_SUBST([YARP_VERSION_MINOR])
YARP_VERSION_MICRO=M4_YARP_VERSION_MICRO
AC_SUBST([YARP_VERSION_MICRO])

#AC_USE_SYSTEM_EXTENSIONS

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

# initialize other tools
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# build time sanity check...
AM_SANITY_CHECK
LT_INIT

# prefix config.h
m4_define([YARP_CONFIG_H], [rpg_config.h])
AX_PREFIX_CONFIG_H([YARP_CONFIG_H], [M4_META_PACKAGE_NAME], [config.h])

#AC_LANG([C++])

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

CXXFLAGS_TRACING=
# --> enable tracing support ?
AC_MSG_CHECKING([enable tracing])
tracing_default="no"
AC_ARG_ENABLE([tracing],
              [AS_HELP_STRING([--enable-tracing], [turn on tracing [default="${tracing_default}"]])],
              [case "${enableval}" in
                 yes) tracing=yes ;;
                 no)  tracing=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-tracing]) ;;
               esac],
              [tracing=$tracing_default])
AM_CONDITIONAL([TRACING], [test x$tracing = xyes])
if test x$tracing = xyes; then
#  CXXFLAGS_TRACING="-DACE_NTRACE=0 -DRPG_NTRACE=0"
  CXXFLAGS_TRACING="-DRPG_NTRACE=0"
  AC_MSG_RESULT([yes])
else
  CXXFLAGS_TRACING="-DACE_NTRACE=1 -DRPG_NTRACE=1"
  AC_MSG_RESULT([no])
fi

CXXFLAGS_VALGRIND=
# --> enable valgrind support ?
AC_MSG_CHECKING([enable valgrind support])
valgrind_support_default="no"
AC_ARG_ENABLE([valgrind-support],
              [AS_HELP_STRING([--enable-valgrind_support], [turn on valgrind support [default="$valgrind_support_default"]])],
              [case "${enableval}" in
                 yes) valgrind_support=yes ;;
                 no)  valgrind_support=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-valgrind-support]) ;;
               esac],
              [valgrind_support=$valgrind_support_default])
AM_CONDITIONAL([VALGRIND_SUPPORT], [test x$valgrind_support = xyes])
if test x$valgrind_support = xyes; then
  CXXFLAGS_VALGRIND="-DRPG_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# export variables (so Makefile substitutions can be made)
CXXFLAGS="${CXXFLAGS_DEBUG} ${CXXFLAGS_TRACING} ${CXXFLAGS_VALGRIND}"
AC_SUBST(CXXFLAGS)
#AC_SUBST(META_PACKAGE_NAME)

# checks for typedefs, structures, and compiler characteristics

# checks for header files
AM_COND_IF([VALGRIND_SUPPORT],
           [AC_CHECK_HEADER([valgrind/valgrind.h])])
#AC_CHECK_HEADER_STDBOOL

# checks for library functions
#AC_CHECK_FUNCS([memset])

# checks for libraries
AM_COND_IF([VALGRIND_SUPPORT],
           [PKG_CHECK_MODULES([PKGVALGRIND], [valgrind])])
#PKG_CHECK_MODULES([PKGLIB], [gtk+-2.0 libglade-2.0])
#PKG_CHECK_MODULES([PKGLIB], [gtk+-2.0])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [pthread_create])
AC_SEARCH_LIBS([pthread_create], [pthread], , AC_MSG_ERROR([unable to find pthread library]))
# FIXME: Replace `main' with a function in `-lACE':
#AC_CHECK_LIB([ACE], [ACE_OS::memset])
#AC_SEARCH_LIBS([ACE_OS::memset], [ACE], , AC_MSG_ERROR([unable to find ACE library]))

# checks for tools

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 m4/Makefile
                 modules/Makefile
                 scripts/Makefile
                 tools/Makefile])
# *TODO*: for some reason, the m4 macro expansion does not work here...
#AC_CONFIG_FILES(Yarp-[M4_YARP_VERSION_API].pc:scripts/Yarp.pc.in)
AC_CONFIG_FILES(Yarp.pc:scripts/Yarp.pc.in)

AC_OUTPUT
