METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_NET_SERVER_HAS_DLL \
  -DRPG_CLIENT_HAS_DLL \
  -DRPG_NET_CLIENT_HAS_DLL \
  -DRPG_NET_HAS_DLL \
  -DRPG_COMMON_HAS_DLL \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../character/player \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../net/stream \
  -I$(srcdir)/../../net \
  -I$(srcdir)/../../net/client \
  -I$(srcdir)/../../sound \
  -I$(srcdir)/../../client \
  -I$(srcdir)/../../net/server

SUBDIRS = protocol

bin_PROGRAMS = net_client net_server

noinst_HEADERS = stdafx.h

net_server_SOURCES = \
  net_server.cpp \
  net_server_signalhandler.cpp \
  net_server_signalhandler.h \
  net_eventhandler.cpp \
  net_eventhandler.h \
  net_callbacks.cpp \
  net_callbacks.h \
  net_defines.h \
  net_common.h
net_server_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#net_server_LDFLAGS = $(pkg_libs_LIBS)
net_server_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Engine \
  -lRPG_Graphics \
  -lRPG_Player \
  -lRPG_Monster \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Combat \
  -lRPG_Map \
  -lRPG_Sound \
  -lRPG_Net_Server \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

net_client_SOURCES = \
  net_client.cpp \
  net_client_signalhandler.cpp \
  net_client_signalhandler.h \
  net_client_timeouthandler.cpp \
  net_client_timeouthandler.h \
  net_eventhandler.cpp \
  net_eventhandler.h \
  net_callbacks.cpp \
  net_callbacks.h \
  net_defines.h \
  net_common.h
net_client_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#net_client_LDFLAGS = $(pkg_libs_LIBS)
net_client_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Engine \
  -lRPG_Graphics \
  -lRPG_Player \
  -lRPG_Monster \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Combat \
  -lRPG_Map \
  -lRPG_Sound \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

configdir = $(confdir)
config_DATA = \
  net_client.glade \
  net_server.glade

EXTRA_DIST = $(config_DATA)

