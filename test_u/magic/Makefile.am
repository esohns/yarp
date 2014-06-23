METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character

bin_PROGRAMS = magic_parser

noinst_HEADERS = stdafx.h

magic_parser_SOURCES = magic_parser.cpp
magic_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#magic_parser_LDFLAGS = $(pkg_libs_LIBS)
magic_parser_LDADD = \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS)
