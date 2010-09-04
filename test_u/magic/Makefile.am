METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = magic_parser

magic_parser_SOURCES = magic_parser.cpp
magic_parser_LDFLAGS = -L/usr/local/lib
magic_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Chance -lRPG_Common \
	-lRPG_Item -lRPG_Combat -lRPG_Monster -lRPG_Character -lRPG_Magic
