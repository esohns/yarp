#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../common \
  -I$(srcdir)/.. \
  -DRPG_NET_CLIENT_BUILD_DLL

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
