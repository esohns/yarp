METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = monster_parser

monster_parser_SOURCES = monster_parser.cpp
monster_parser_LDFLAGS = -L/usr/local/lib
monster_parser_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster
