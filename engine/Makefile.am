ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../3rd_party/bison \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client_server \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../chance \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character \
  -I$(top_srcdir)/../character/player \
  -I$(top_srcdir)/../combat \
  -I$(top_srcdir)/../character/monster \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../net \
  -I$(top_srcdir)/../net/protocol

DEFS = \
  -DRPG_ENGINE_BUILD_DLL

SUBDIRS = data scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_engine.h \
  rpg_engine_command.h \
  rpg_engine_common.h \
  rpg_engine_common_tools.h \
  rpg_engine_defines.h \
  rpg_engine_entitymode.h \
  rpg_engine_event_common.h \
  rpg_engine_event_manager.h \
  rpg_engine_exports.h \
  rpg_engine_iclient.h \
  rpg_engine_level.h \
  rpg_engine_messagequeue.h \
  rpg_engine_XML_tree.h

lib_LTLIBRARIES = libRPG_Engine.la

libRPG_Engine_la_SOURCES = \
  rpg_engine.cpp \
  rpg_engine.h \
  rpg_engine_command.h \
  rpg_engine_common.h \
  rpg_engine_common_tools.cpp \
  rpg_engine_common_tools.h \
  rpg_engine_defines.h \
  rpg_engine_exports.h \
  rpg_engine_entitymode.h \
  rpg_engine_event_common.h \
  rpg_engine_event_manager.cpp \
  rpg_engine_event_manager.h \
  rpg_engine_iclient.h \
  rpg_engine_level.cpp \
  rpg_engine_level.h \
  rpg_engine_messagequeue.cpp \
  rpg_engine_messagequeue.h \
  rpg_engine_XML_tree.cpp \
  rpg_engine_XML_tree.h \
  stdafx.cpp \
  stdafx.h

libRPG_Engine_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS)

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_engine.xsd

EXTRA_DIST = $(config_DATA)
