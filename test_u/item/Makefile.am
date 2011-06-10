METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = item_parser

item_parser_SOURCES = item_parser.cpp
item_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
item_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common \
	-lRPG_Magic -lRPG_Item
