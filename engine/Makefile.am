METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

include_HEADERS = rpg_engine_XML_tree.h rpg_engine_common.h \
	rpg_engine_common_tools.h rpg_engine_defines.h rpg_engine_level.h

lib_LTLIBRARIES = libRPG_Engine.la

libRPG_Engine_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_engine_XML_tree.cpp \
	rpg_engine_XML_tree.h rpg_engine_common.h rpg_engine_common_tools.cpp \
	rpg_engine_common_tools.h rpg_engine_defines.h rpg_engine_level.cpp rpg_engine_level.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_engine.xsd

EXTRA_DIST = $(config_DATA)
noinst_HEADERS = rpg_XMLSchema_XML_tree.h
