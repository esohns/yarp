METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = monster_parser

monster_parser_SOURCES = monster_parser.cpp
monster_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
monster_parser_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster
