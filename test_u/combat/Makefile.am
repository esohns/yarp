#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = combat_simulator

noinst_HEADERS = stdafx.h

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character \
  -I$(top_srcdir)/../character/player \
  -I$(top_srcdir)/../combat \
  -I$(top_srcdir)/../character/monster \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../engine \
  -I$(top_srcdir)/../graphics \
  $(pkg_libs_CFLAGS)
combat_simulator_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../chance/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../magic/.libs \
  -L$(abs_builddir)/../../item/.libs \
  -L$(abs_builddir)/../../character/.libs \
  -L$(abs_builddir)/../../character/player/.libs \
  -L$(abs_builddir)/../../combat/.libs \
  -L$(abs_builddir)/../../character/monster/.libs \
  -L$(abs_builddir)/../../map/.libs \
  -L$(abs_builddir)/../../net/.libs \
  -L$(abs_builddir)/../../net/protocol/.libs \
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
combat_simulator_LDADD = \
  -lACE \
  -lCommon \
  -lACEStream \
  -lACENetwork \
  -lRPG_Dice \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Magic \
  -lRPG_Item \
  -lRPG_Character \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Monster \
  -lRPG_Map \
  -lRPG_Net \
  -lRPG_Net_Protocol \
  -lRPG_Engine \
  -lRPG_Graphics \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

