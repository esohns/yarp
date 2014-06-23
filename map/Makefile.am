METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../3rd_party/bison \
  -I$(srcdir)/../chance/dice \
  -I$(srcdir)/../common

SUBDIRS = scripts . data

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_map_common.h \
  rpg_map_common_tools.h \
  rpg_map_data.h \
  rpg_map_defines.h \
  rpg_map_direction.h \
  rpg_map_doorstate.h \
  rpg_map_exports.h \
  rpg_map_incl.h \
  rpg_map_level.h \
  rpg_map_parser.h \
  rpg_map_parser_driver.h \
  rpg_map_pathfinding_tools.h \
  rpg_map_position.h \
  rpg_map_scanner.h \
  rpg_map_XML_tree.h

lib_LTLIBRARIES = libRPG_Map.la

libRPG_Map_la_SOURCES = \
  rpg_map_common.h \
  rpg_map_common_tools.cpp \
  rpg_map_common_tools.h \
  rpg_map_data.h \
  rpg_map_defines.h \
  rpg_map_doorstate.h \
  rpg_map_exports.h \
  rpg_map_incl.h \
  rpg_map_level.cpp \
  rpg_map_level.h \
  rpg_map_parser.cpp \
  rpg_map_parser.h \
  rpg_map_parser_driver.cpp \
  rpg_map_parser_driver.h \
  rpg_map_pathfinding_tools.cpp \
  rpg_map_pathfinding_tools.h \
  rpg_map_position.h \
  rpg_map_scanner.cpp \
  rpg_map_scanner.h \
  rpg_map_XML_tree.cpp \
  rpg_map_XML_tree.h \
  stdafx.cpp \
  stdafx.h
libRPG_Map_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_map.xsd

EXTRA_DIST = $(config_DATA)

