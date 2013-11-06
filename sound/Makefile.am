METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_SOUND_BUILD_DLL -I$(srcdir)/../common -I$(srcdir)/../chance/dice

SUBDIRS = data

noinst_HEADERS = rpg_XMLSchema_XML_types.h stdafx.h
include_HEADERS = rpg_sound.h rpg_sound_XML_parser.h rpg_sound_XML_types.h \
	rpg_sound_category.h rpg_sound_common.h rpg_sound_common_tools.h \
  rpg_sound_defines.h rpg_sound_dictionary.h rpg_sound_event.h rpg_sound_incl.h \
  rpg_sound_exports.h rpg_sound_event_manager.h

lib_LTLIBRARIES = libRPG_Sound.la

libRPG_Sound_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_sound.h \
	rpg_sound_XML_parser.cpp rpg_sound_XML_parser.h rpg_sound_XML_types.cpp \
  rpg_sound_XML_types.h rpg_sound_category.h rpg_sound_common.h \
  rpg_sound_common_tools.cpp rpg_sound_common_tools.h rpg_sound_defines.h \
  rpg_sound_dictionary.cpp rpg_sound_dictionary.h rpg_sound_event.h \
	rpg_sound_incl.h rpg_sound_exports.h rpg_sound_event_manager.h \
  rpg_sound_event_manager.cpp
libRPG_Sound_la_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS)

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_sound.xml rpg_sound.xsd

EXTRA_DIST = $(config_DATA)

