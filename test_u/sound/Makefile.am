METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = sound_parser

sound_parser_SOURCES = sound_parser.cpp
sound_parser_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="$(datadir)/@META_PACKAGE@"
#sound_parser_LDFLAGS = $(pkg_libs_LIBS)
sound_parser_LDADD = -lRPG_Sound -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS) \
  -lSDL_mixer
