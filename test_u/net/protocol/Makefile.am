METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../../common \
  -I$(srcdir)/../../../net/protocol \
  -I$(srcdir)/../../../net \
  -I$(srcdir)/../../../net/stream \
  -I$(srcdir)/../../../chance/dice \
  -I$(srcdir)/../../../client \
  -I$(srcdir)/../../../3rd_party/bison

bin_PROGRAMS = IRC_client IRC_client_gui

noinst_HEADERS = stdafx.h

IRC_client_SOURCES = \
  IRC_client.cpp \
  IRC_client_defines.h \
  IRC_client_signalhandler.cpp \
  IRC_client_signalhandler.h \
  IRC_client_tools.cpp \
  IRC_client_tools.h \
  IRC_common.h
IRC_client_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#IRC_client_LDFLAGS = $(pkg_libs_LIBS)
IRC_client_LDADD = \
  -lRPG_Net_Protocol \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

IRC_client_gui_SOURCES = \
  IRC_client_defines.h \
  IRC_client_gui.cpp \
  IRC_client_gui_callbacks.cpp \
  IRC_client_gui_callbacks.h \
  IRC_client_gui_common.h \
  IRC_client_gui_connection.cpp \
  IRC_client_gui_connection.h \
  IRC_client_gui_defines.h \
  IRC_client_gui_messagehandler.cpp \
  IRC_client_gui_messagehandler.h \
  IRC_client_tools.cpp \
  IRC_client_tools.h \
  IRC_common.h
IRC_client_gui_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#IRC_client_gui_LDFLAGS = $(pkg_libs_LIBS)
IRC_client_gui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Engine \
  -lRPG_Graphics \
  -lRPG_Sound \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Net_Client \
  -lRPG_Net_Protocol \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  IRC_client.ini \
  IRC_client_channel_tab.glade \
  IRC_client_main.glade \
  IRC_client_server_page.glade \
  servers.ini

EXTRA_DIST = $(config_DATA)

