#METASOURCES = AUTO

AM_CPPFLAGS = \
  -I$(builddir)/../.. \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../client \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../net/stream \
  -I$(srcdir)/../../net \
  -I$(srcdir)/../../net/client \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/player \
  -I$(srcdir)/../../sound

bin_PROGRAMS = rpg_client_gui

noinst_HEADERS = stdafx.h

rpg_client_gui_SOURCES = rpg_client_gui.cpp
rpg_client_gui_CPPFLAGS	= \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DRPG_NET_HAS_DLL=1 \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#rpg_client_gui_LDFLAGS = $(pkg_libs_LIBS)
#rpg_client_gui_LDFLAGS = \
#  $(RPG_CPPLDFLAGS) \
#  -L$(builddir)/../../lib
rpg_client_gui_LDADD = \
  $(builddir)/../../client/libRPG_Client_GUI.la \
  $(builddir)/../../graphics/libRPG_Graphics.la \
  $(builddir)/../../sound/libRPG_Sound.la \
  $(builddir)/../../engine/libRPG_Engine.la \
  $(builddir)/../../net/client/libRPG_Net_Client.la \
  $(builddir)/../../net/libRPG_Net.la \
  $(builddir)/../../net/stream/libRPG_Stream.la \
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
  $(pkg_libs_LIBS)
