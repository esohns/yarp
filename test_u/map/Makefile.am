METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/player

bin_PROGRAMS = map_generator path_finder

noinst_HEADERS = stdafx.h

map_generator_SOURCES = map_generator.cpp
map_generator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS)
#map_generator_LDFLAGS = $(pkg_libs_LIBS)
map_generator_LDADD = \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

path_finder_SOURCES = path_finder.cpp
path_finder_CPPFLAGS = $(AM_CPPFLAGS)
#path_finder_LDFLAGS = $(pkg_libs_LIBS)
path_finder_LDADD = \
  -lRPG_Map \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)
