METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_ENGINE_BUILD_DLL \
  -DRPG_NET_CLIENT_HAS_DLL \
  -I$(srcdir)/../common \
  -I$(srcdir)/../character/player \
  -I$(srcdir)/../chance/dice \
  -I$(srcdir)/../magic \
  -I$(srcdir)/../character \
  -I$(srcdir)/../item \
  -I$(srcdir)/../map \
  -I$(srcdir)/../graphics \
  -I$(srcdir)/../combat \
  -I$(srcdir)/../character/monster \
  -I$(srcdir)/../chance \
  -I$(srcdir)/../net/stream \
  -I$(srcdir)/../net \
  -I$(srcdir)/../net/client \
  -I$(srcdir)/../3rd_party/bison

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h

include_HEADERS = \
  rpg_engine_XML_tree.h \
  rpg_engine_common.h \
  rpg_engine_common_tools.h \
  rpg_engine_defines.h \
  rpg_engine_level.h \
  rpg_engine_exports.h \
  rpg_engine.h \
  rpg_engine_command.h \
  rpg_engine_entitymode.h \
  rpg_engine_event_common.h \
  rpg_engine_event_manager.h \
  rpg_engine_iclient.h \
  rpg_engine_messagequeue.h

lib_LTLIBRARIES = libRPG_Engine.la

libRPG_Engine_la_SOURCES = \
  rpg_engine_XML_tree.cpp \
  rpg_engine_XML_tree.h \
  rpg_engine_common.h \
  rpg_engine_common_tools.cpp \
  rpg_engine_common_tools.h \
  rpg_engine_defines.h \
  rpg_engine_level.cpp \
  rpg_engine_level.h \
  rpg_engine_exports.h \
  rpg_engine.h \
  rpg_engine.cpp \
  rpg_engine_command.h \
  rpg_engine_entitymode.h \
  rpg_engine_event_common.h \
  rpg_engine_event_manager.h \
  rpg_engine_event_manager.cpp \
  rpg_engine_iclient.h \
  rpg_engine_messagequeue.h \
  rpg_engine_messagequeue.cpp

libRPG_Engine_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS)

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_engine.xsd

EXTRA_DIST = $(config_DATA)

