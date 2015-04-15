#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = item_parser

noinst_HEADERS = stdafx.h

item_parser_SOURCES = item_parser.cpp
item_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character \
  $(pkg_libs_CFLAGS)
item_parser_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../chance/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../magic/.libs \
  -L$(abs_builddir)/../../item/.libs \
  -L$(abs_builddir)/../../character/.libs \
  $(pkg_libs_LDFLAGS)
item_parser_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Magic \
  -lRPG_Item \
  -lRPG_Character \
  $(pkg_libs_LIBS)

