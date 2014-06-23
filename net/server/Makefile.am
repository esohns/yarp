METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_NET_SERVER_BUILD_DLL \
	-I$(srcdir)/../../common \
	-I$(srcdir)/.. \
	-I$(srcdir)/../stream

noinst_HEADERS = stdafx.h
include_HEADERS = \
  rpg_net_server_asynchlistener.h \
  rpg_net_server_common_tools.h \
	rpg_net_server_common.h \
  rpg_net_server_defines.h \
  rpg_net_server_exports.h \
  rpg_net_server_ilistener.h \
  rpg_net_server_listener.h \
  rpg_net_server_sockethandler.h

lib_LTLIBRARIES = libRPG_Net_Server.la

libRPG_Net_Server_la_SOURCES = \
  rpg_net_server_asynchlistener.cpp \
  rpg_net_server_asynchlistener.h \
  rpg_net_server_common_tools.cpp \
  rpg_net_server_common_tools.h \
  rpg_net_server_common.h \
  rpg_net_server_defines.h \
  rpg_net_server_exports.h \
  rpg_net_server_ilistener.h \
  rpg_net_server_listener.cpp \
  rpg_net_server_listener.h \
	rpg_net_server_sockethandler.cpp \
	rpg_net_server_sockethandler.h
