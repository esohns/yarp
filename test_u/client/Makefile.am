METASOURCES = AUTO

AM_CPPFLAGS = \
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
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#rpg_client_gui_LDFLAGS = $(pkg_libs_LIBS)
rpg_client_gui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Graphics \
  -lRPG_Sound \
  -lRPG_Engine \
  -lRPG_Stream \
  -lRPG_Net \
  -lRPG_Net_Client \
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

