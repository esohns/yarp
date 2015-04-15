#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -DRPG_SOUND_BUILD_DLL

SUBDIRS = scripts data

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_sound.h \
  rpg_sound_category.h \
  rpg_sound_common.h \
  rpg_sound_common_tools.h \
  rpg_sound_defines.h \
  rpg_sound_dictionary.h \
  rpg_sound_event.h \
  rpg_sound_event_manager.h \
  rpg_sound_exports.h \
  rpg_sound_incl.h \
  rpg_sound_XML_parser.h \
  rpg_sound_XML_types.h

lib_LTLIBRARIES = libRPG_Sound.la

libRPG_Sound_la_SOURCES = \
  rpg_sound.h \
  rpg_sound_category.h \
  rpg_sound_common.h \
  rpg_sound_common_tools.cpp \
  rpg_sound_common_tools.h \
  rpg_sound_defines.h \
  rpg_sound_dictionary.cpp \
  rpg_sound_dictionary.h \
  rpg_sound_event.h \
  rpg_sound_exports.h \
  rpg_sound_event_manager.cpp \
  rpg_sound_event_manager.h \
  rpg_sound_incl.h \
  rpg_sound_XML_parser.cpp \
  rpg_sound_XML_parser.h \
  rpg_sound_XML_types.cpp \
  rpg_sound_XML_types.h \
  stdafx.cpp \
  stdafx.h

libRPG_Sound_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS)

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_sound.xml \
  rpg_sound.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_sound.map

