SUBDIRS = scripts

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(srcdir)/..

DEFS = \
  -DRPG_PROTOCOL_BUILD_DLL

noinst_HEADERS = \
  stdafx.h

include_HEADERS = \
  rpg_net_protocol_common_modules.h \
  rpg_net_protocol_common.h \
  rpg_net_protocol_configuration.h \
  rpg_net_protocol_defines.h \
  rpg_net_protocol_exports.h \
  rpg_net_protocol_iIRCControl.h \
  rpg_net_protocol_IRC_codes.h \
  rpg_net_protocol_IRCbisect.h \
  rpg_net_protocol_IRCmessage.h \
  rpg_net_protocol_IRCparser_driver.h \
  rpg_net_protocol_IRCparser.h \
  rpg_net_protocol_IRCscanner.h \
  rpg_net_protocol_message.h \
  rpg_net_protocol_messageallocator.h \
  rpg_net_protocol_module_IRChandler.h \
  rpg_net_protocol_module_IRCparser.h \
  rpg_net_protocol_module_IRCsplitter.h \
  rpg_net_protocol_module_IRCstreamer.h \
  rpg_net_protocol_network.h \
  rpg_net_protocol_sessionmessage.h \
  rpg_net_protocol_stream.h \
  rpg_net_protocol_stream_common.h \
  rpg_net_protocol_tools.h

lib_LTLIBRARIES = libRPG_Net_Protocol.la

libRPG_Net_Protocol_la_SOURCES = \
  rpg_net_protocol_common.h \
  rpg_net_protocol_common_modules.h \
  rpg_net_protocol_configuration.h \
  rpg_net_protocol_defines.h \
  rpg_net_protocol_exports.h \
  rpg_net_protocol_iIRCControl.h \
  rpg_net_protocol_IRC_codes.h \
  rpg_net_protocol_IRCbisect.cpp \
  rpg_net_protocol_IRCbisect.h \
  rpg_net_protocol_IRCmessage.cpp \
  rpg_net_protocol_IRCmessage.h \
  rpg_net_protocol_IRCparser.cpp \
  rpg_net_protocol_IRCparser.h \
  rpg_net_protocol_IRCparser_driver.cpp \
  rpg_net_protocol_IRCparser_driver.h \
  rpg_net_protocol_IRCscanner.cpp \
  rpg_net_protocol_IRCscanner.h \
  rpg_net_protocol_message.cpp \
  rpg_net_protocol_message.h \
  rpg_net_protocol_messageallocator.cpp \
  rpg_net_protocol_messageallocator.h \
  rpg_net_protocol_module_IRChandler.cpp \
  rpg_net_protocol_module_IRChandler.h \
  rpg_net_protocol_module_IRCparser.cpp \
  rpg_net_protocol_module_IRCparser.h \
  rpg_net_protocol_module_IRCsplitter.cpp \
  rpg_net_protocol_module_IRCsplitter.h \
  rpg_net_protocol_module_IRCstreamer.cpp \
  rpg_net_protocol_module_IRCstreamer.h \
  rpg_net_protocol_network.h \
  rpg_net_protocol_sessionmessage.cpp \
  rpg_net_protocol_sessionmessage.h \
  rpg_net_protocol_stream.cpp \
  rpg_net_protocol_stream.h \
  rpg_net_protocol_stream_common.h \
  rpg_net_protocol_tools.cpp \
  rpg_net_protocol_tools.h

libRPG_Net_Protocol_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../3rd_party/bison \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client_server \
  -I$(top_srcdir)/../common
