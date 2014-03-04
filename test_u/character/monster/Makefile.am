METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../../common \
  -I$(srcdir)/../../../character/monster \
  -I$(srcdir)/../../../chance/dice \
  -I$(srcdir)/../../../character \
  -I$(srcdir)/../../../magic \
  -I$(srcdir)/../../../item \
  -I$(srcdir)/../../../combat \
  -I$(srcdir)/../../../character/player

bin_PROGRAMS = monster_parser

noinst_HEADERS = stdafx.h

monster_parser_SOURCES = monster_parser.cpp
monster_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
monster_parser_LDADD = \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS)
