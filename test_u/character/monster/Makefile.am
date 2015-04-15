#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = monster_parser

noinst_HEADERS = stdafx.h

monster_parser_SOURCES = monster_parser.cpp
monster_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character \
  -I$(top_srcdir)/../character/player \
  -I$(top_srcdir)/../combat \
  -I$(top_srcdir)/../character/monster \
  $(pkg_libs_CFLAGS)
monster_parser_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../../chance/dice/.libs \
  -L$(abs_builddir)/../../../chance/.libs \
  -L$(abs_builddir)/../../../common/.libs \
  -L$(abs_builddir)/../../../magic/.libs \
  -L$(abs_builddir)/../../../item/.libs \
  -L$(abs_builddir)/../../../character/.libs \
  -L$(abs_builddir)/../../../character/player/.libs \
  -L$(abs_builddir)/../../../combat/.libs \
  -L$(abs_builddir)/../../../character/monster/.libs \
  $(pkg_libs_LDFLAGS)
monster_parser_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Magic \
  -lRPG_Item \
  -lRPG_Character \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Monster \
  $(pkg_libs_LIBS)

