METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/../.. \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../sound \
  -I$(srcdir)/../../chance/dice

bin_PROGRAMS = sound_parser

noinst_HEADERS = stdafx.h

sound_parser_SOURCES = sound_parser.cpp
sound_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#sound_parser_LDFLAGS = $(pkg_libs_LIBS)
sound_parser_LDADD = \
  -lRPG_Sound \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer
