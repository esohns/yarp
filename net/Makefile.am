#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -DRPG_NET_BUILD_DLL \
  -DMETA_PACKAGE=\"@META_PACKAGE@\"

SUBDIRS = \
  scripts \
  . \
  protocol \
  server

# define dependencies
protocol: .
client: .
server: .

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_net_common.h \
  rpg_net_common_tools.h \
  rpg_net_defines.h \
  rpg_net_exports.h \
  rpg_net_module_eventhandler.h \
  rpg_net_module_protocolhandler.h \
  rpg_net_remote_comm.h \
  rpg_net_stream.h

lib_LTLIBRARIES = libRPG_Net.la

libRPG_Net_la_SOURCES = \
  rpg_net_common.h \
  rpg_net_common_tools.cpp \
  rpg_net_common_tools.h \
  rpg_net_defines.h \
  rpg_net_exports.h \
  rpg_net_module_eventhandler.cpp \
  rpg_net_module_eventhandler.h \
  rpg_net_module_protocolhandler.cpp \
  rpg_net_module_protocolhandler.h \
  rpg_net_remote_comm.h \
  rpg_net_stream.cpp \
  rpg_net_stream.h \
  stdafx.cpp \
  stdafx.h
