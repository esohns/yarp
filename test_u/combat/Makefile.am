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

bin_PROGRAMS = combat_simulator

noinst_HEADERS = stdafx.h

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#combat_simulator_LDFLAGS = $(pkg_libs_LIBS)
combat_simulator_LDADD = \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Sound \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

