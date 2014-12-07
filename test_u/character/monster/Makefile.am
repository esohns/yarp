#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/../.. \
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
  $(builddir)/../../../character/monster/libRPG_Monster.la \
  $(builddir)/../../../character/player/libRPG_Player.la \
  $(builddir)/../../../combat/libRPG_Combat.la \
  $(builddir)/../../../character/libRPG_Character.la \
  $(builddir)/../../../item/libRPG_Item.la \
  $(builddir)/../../../magic/libRPG_Magic.la \
  $(builddir)/../../../common/libRPG_Common.la \
  $(builddir)/../../../chance/libRPG_Chance.la \
  $(builddir)/../../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS)
