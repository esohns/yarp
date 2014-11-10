METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/.. \
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
  $(builddir)/../../magic/libRPG_Magic.la \
  $(builddir)/../../character/libRPG_Character.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/libRPG_Chance.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS)
