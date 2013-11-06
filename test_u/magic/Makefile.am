METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = magic_parser

magic_parser_SOURCES = magic_parser.cpp
magic_parser_CPPFLAGS = -DDATADIR="$(datadir)/@META_PACKAGE@/config"
#magic_parser_LDFLAGS = $(pkg_libs_LIBS)
magic_parser_LDADD = -lRPG_Magic -lRPG_Character -lRPG_Common -lRPG_Chance \
  -lRPG_Dice $(pkg_libs_LIBS)
