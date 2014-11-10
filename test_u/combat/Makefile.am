#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/.. \
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
  $(builddir)/../../engine/libRPG_Engine.la \
  $(builddir)/../../net/client/libRPG_Net_Client.la \
  $(builddir)/../../net/libRPG_Net.la \
  $(builddir)/../../net/stream/libRPG_Stream.la \
  $(builddir)/../../graphics/libRPG_Graphics.la \
  $(builddir)/../../sound/libRPG_Sound.la \
  $(builddir)/../../map/libRPG_Map.la \
  $(builddir)/../../character/monster/libRPG_Monster.la \
  $(builddir)/../../character/player/libRPG_Player.la \
  $(builddir)/../../combat/libRPG_Combat.la \
  $(builddir)/../../character/libRPG_Character.la \
  $(builddir)/../../item/libRPG_Item.la \
  $(builddir)/../../magic/libRPG_Magic.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/libRPG_Chance.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS) \
  -lSDL_mixer
