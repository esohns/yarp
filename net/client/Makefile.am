METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/../../common -I$(srcdir)/.. -I$(srcdir)/../stream

noinst_HEADERS = stdafx.h
include_HEADERS = rpg_net_client_sockethandler.h \
	rpg_net_client_common.h \
  rpg_net_client_connector.h \
  rpg_net_client_asynchconnector.h

lib_LTLIBRARIES = libRPG_Net_Client.la

libRPG_Net_Client_la_SOURCES = rpg_net_client_sockethandler.cpp \
	rpg_net_client_sockethandler.h \
  rpg_net_client_common.h \
  rpg_net_client_connector.h \
  rpg_net_client_connector.cpp \
  rpg_net_client_asynchconnector.h \
  rpg_net_client_asynchconnector.cpp
