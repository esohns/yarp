#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client_server \
  -I$(srcdir)/../../common \
  -I$(srcdir)/..

DEFS = \
  -DRPG_NET_SERVER_BUILD_DLL

noinst_HEADERS = stdafx.h

include_HEADERS = \
  rpg_net_server_common_tools.h \
  rpg_net_server_common.h \
  rpg_net_server_defines.h \
  rpg_net_server_exports.h

lib_LTLIBRARIES = \
  libRPG_Net_Server.la

libRPG_Net_Server_la_SOURCES = \
  rpg_net_server_common_tools.cpp \
  rpg_net_server_common_tools.h \
  rpg_net_server_common.h \
  rpg_net_server_defines.h \
  rpg_net_server_exports.h
