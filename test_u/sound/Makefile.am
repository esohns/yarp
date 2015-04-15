#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = sound_parser

noinst_HEADERS = stdafx.h

sound_parser_SOURCES = sound_parser.cpp
sound_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../sound \
  $(pkg_libs_CFLAGS)
sound_parser_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../sound/.libs \
  $(pkg_libs_LDFLAGS)
sound_parser_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Common \
  -lRPG_Sound \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

