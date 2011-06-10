METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = magic_parser

magic_parser_SOURCES = magic_parser.cpp
magic_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
magic_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common \
	-lRPG_Magic
