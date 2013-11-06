METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = monster_parser

monster_parser_SOURCES = monster_parser.cpp
monster_parser_CPPFLAGS = -DDATADIR="$(datadir)/@META_PACKAGE@/config"
monster_parser_LDADD = -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character \
  -lRPG_Item -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
