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
  $(builddir)/../../sound/libRPG_Sound.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS) \
  -lSDL_mixer
