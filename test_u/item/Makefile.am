#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/.. \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic

bin_PROGRAMS = item_parser

noinst_HEADERS = stdafx.h

item_parser_SOURCES = item_parser.cpp
item_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#item_parser_LDFLAGS = $(pkg_libs_LIBS)
item_parser_LDADD = \
  $(builddir)/../../item/libRPG_Item.la \
  $(builddir)/../../magic/libRPG_Magic.la \
  $(builddir)/../../character/libRPG_Character.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/libRPG_Chance.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS)
