METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = item_parser

item_parser_SOURCES = item_parser.cpp
item_parser_CPPFLAGS = -DDATADIR="$(datadir)/@META_PACKAGE@/config"
#item_parser_LDFLAGS = $(pkg_libs_LIBS)
item_parser_LDADD = -lRPG_Item -lRPG_Magic -lRPG_Character -lRPG_Common \
  -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
