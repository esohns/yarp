#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_NET_CLIENT_BUILD_DLL \
	-I$(srcdir)/../../common \
	-I$(srcdir)/.. \
	-I$(srcdir)/../stream

noinst_HEADERS = stdafx.h
include_HEADERS = \
	rpg_net_client_asynchconnector.h \
	rpg_net_client_common.h \
	rpg_net_client_connector.h \
	rpg_net_client_defines.h \
	rpg_net_client_exports.h \
        rpg_net_client_iconnector.h

lib_LTLIBRARIES = libRPG_Net_Client.la

libRPG_Net_Client_la_SOURCES = \
  rpg_net_client_asynchconnector.cpp \
  rpg_net_client_asynchconnector.h \
  rpg_net_client_common.h \
  rpg_net_client_connector.cpp \
  rpg_net_client_connector.h \
  rpg_net_client_defines.h \
  rpg_net_client_exports.h \
  rpg_net_client_iconnector.h
