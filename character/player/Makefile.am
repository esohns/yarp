METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_PLAYER_BUILD_DLL -I$(srcdir)/../../common -I$(srcdir)/.. \
  -I$(srcdir)/../../magic -I$(srcdir)/../../item -I$(srcdir)/../../combat \
  -I$(srcdir)/../../chance/dice

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_iplayer.h \
  rpg_player.h \
  rpg_player_abilities.h \
  rpg_player_base.h \
  rpg_player_characterbasexml.h \
  rpg_player_characterxml.h \
  rpg_player_common.h \
  rpg_player_common_tools.h \
  rpg_player_conditions.h \
  rpg_player_defines.h \
  rpg_player_equipment.h \
  rpg_player_equipment_common.h \
  rpg_player_exports.h \
  rpg_player_incl.h \
  rpg_player_inventory.h \
  rpg_player_player_base.h \
  rpg_player_playerxml.h \
  rpg_player_spells.h \
  rpg_player_state.h \
  rpg_player_XML_tree.h \
  rpg_player_XML_types.h

lib_LTLIBRARIES = libRPG_Player.la

libRPG_Player_la_SOURCES = \
  rpg_iplayer.h \
  rpg_player.cpp \
  rpg_player.h \
  rpg_player_abilities.h \
  rpg_player_base.cpp \
  rpg_player_base.h \
  rpg_player_characterbasexml.h \
  rpg_player_characterxml.h \
  rpg_player_common.h \
  rpg_player_common_tools.cpp \
  rpg_player_common_tools.h \
  rpg_player_conditions.h \
  rpg_player_defines.h \
  rpg_player_equipment.cpp \
  rpg_player_equipment.h \
  rpg_player_equipment_common.h \
  rpg_player_exports.h \
  rpg_player_incl.h \
  rpg_player_inventory.cpp \
  rpg_player_inventory.h \
  rpg_player_player_base.cpp \
  rpg_player_player_base.h \
  rpg_player_playerxml.h \
  rpg_player_spells.h \
  rpg_player_state.h \
  rpg_player_XML_tree.cpp \
  rpg_player_XML_tree.h \
  rpg_player_XML_types.cpp \
  rpg_player_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_player.xsd

EXTRA_DIST = \
 $(config_DATA) \
 rpg_player.map
