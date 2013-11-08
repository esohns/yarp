METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../sound \
	-I$(top_srcdir)/../chance/dice

bin_PROGRAMS = sound_parser

sound_parser_SOURCES = sound_parser.cpp
sound_parser_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS) \
	-DDATADIR="$(datadir)/@META_PACKAGE@"
#sound_parser_LDFLAGS = $(pkg_libs_LIBS)
sound_parser_LDADD = -lRPG_Sound -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS) \
  -lSDL_mixer
