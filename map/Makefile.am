METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/../chance/dice -I$(srcdir)/../common

SUBDIRS = scripts

noinst_HEADERS = rpg_XMLSchema_XML_tree.h stdafx.h
include_HEADERS = rpg_map_common.h rpg_map_common_tools.h rpg_map_defines.h \
	rpg_map_level.h rpg_map_parser.h rpg_map_parser_driver.h rpg_map_pathfinding_tools.h \
	rpg_map_scanner.h location.hh position.hh stack.hh rpg_map_XML_tree.h \
	rpg_map_position.h rpg_map_incl.h rpg_map_doorstate.h rpg_map_exports.h

lib_LTLIBRARIES = libRPG_Map.la

libRPG_Map_la_SOURCES = location.hh position.hh stack.hh rpg_map_common.h \
	rpg_map_common_tools.cpp rpg_map_common_tools.h rpg_map_defines.h rpg_map_level.cpp \
	rpg_map_level.h rpg_map_parser.cpp rpg_map_parser.h rpg_map_parser_driver.cpp \
	rpg_map_parser_driver.h rpg_map_pathfinding_tools.cpp rpg_map_pathfinding_tools.h \
	rpg_map_scanner.cpp rpg_map_scanner.h rpg_map_XML_tree.h \
	rpg_map_XML_tree.cpp rpg_map_position.h rpg_map_incl.h rpg_map_exports.h \
  rpg_map_doorstate.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_map.xsd

EXTRA_DIST = $(config_DATA)
