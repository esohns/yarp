METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/stream -I$(top_srcdir)

noinst_HEADERS = stdafx.h
include_HEADERS = rpg_net_server_sockethandler.h \
	rpg_net_server_common.h

lib_LTLIBRARIES = libRPG_Net_Server.la

libRPG_Net_Server_la_SOURCES = rpg_net_server_sockethandler.cpp \
	rpg_net_server_sockethandler.h \
  rpg_net_server_common.h
