METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../item \
	-I$(top_srcdir)/../chance/dice -I$(top_srcdir)/../character \
	-I$(top_srcdir)/../magic

bin_PROGRAMS = item_parser

item_parser_SOURCES = item_parser.cpp
item_parser_CPPFLAGS = $(AM_CPPFLAGS) -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#item_parser_LDFLAGS = $(pkg_libs_LIBS)
item_parser_LDADD = -lRPG_Item -lRPG_Magic -lRPG_Character -lRPG_Common \
  -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
