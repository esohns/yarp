METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

noinst_HEADERS = rpg_XMLSchema_XML_tree.h

include_HEADERS = rpg_character_player.h \
		  rpg_character_player_XML_tree.h \
		  rpg_character_player_base.h \
		  rpg_character_player_common.h \
		  rpg_character_player_common_tools.h

lib_LTLIBRARIES = libRPG_Character_Player.la

libRPG_Character_Player_la_SOURCES = rpg_XMLSchema_XML_tree.h \
			             rpg_character_player.cpp \
			             rpg_character_player.h \
				     rpg_character_player_XML_tree.cpp \
				     rpg_character_player_XML_tree.h \
				     rpg_character_player_base.cpp \
				     rpg_character_player_base.h \
				     rpg_character_player_common.h \
				     rpg_character_player_common_tools.cpp \
				     rpg_character_player_common_tools.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_character_player.xsd

EXTRA_DIST = $(config_DATA)
