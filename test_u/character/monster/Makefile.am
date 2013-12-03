METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../character/monster \
  -I$(top_srcdir)/../chance/dice -I$(top_srcdir)/../character \
	-I$(top_srcdir)/../magic -I$(top_srcdir)/../item -I$(top_srcdir)/../combat \
	-I$(top_srcdir)/../character/player

bin_PROGRAMS = monster_parser

monster_parser_SOURCES = monster_parser.cpp
monster_parser_CPPFLAGS = $(AM_CPPFLAGS) -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
monster_parser_LDADD = -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character \
  -lRPG_Item -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
