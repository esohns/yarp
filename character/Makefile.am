METASOURCES = AUTO

include_HEADERS = rpg_character_XML_parser.h rpg_character_XML_parser_base.h \
	rpg_character_XML_types.h rpg_character_base.h rpg_character_class_common.h \
	rpg_character_common.h rpg_character_dictionary.h rpg_character_equipment.h \
	rpg_character_equipment_common.h rpg_character_monster.h rpg_character_monster_common.h \
	rpg_character_player.h rpg_character_player_base.h rpg_character_race_common.h \
	rpg_character_skills_common.h

lib_LTLIBRARIES = libRPG_Character.la

libRPG_Character_la_SOURCES = rpg_XMLSchema.h rpg_character_XML_parser.cpp \
	rpg_character_XML_parser.h rpg_character_XML_parser_base.cpp rpg_character_XML_parser_base.h \
	rpg_character_XML_types.cpp rpg_character_XML_types.h rpg_character_base.cpp rpg_character_base.h \
	rpg_character_class_common.h rpg_character_common.h rpg_character_common_tools.cpp \
	rpg_character_common_tools.h rpg_character_dictionary.cpp rpg_character_equipment.cpp \
	rpg_character_equipment.h rpg_character_equipment_common.h rpg_character_inventory.cpp \
	rpg_character_inventory.h rpg_character_monster.cpp rpg_character_monster.h \
	rpg_character_monster_common.h rpg_character_monster_common_tools.cpp \
	rpg_character_monster_common_tools.h rpg_character_player.cpp rpg_character_player.h \
	rpg_character_player_base.cpp rpg_character_player_base.h rpg_character_race_common.h \
	rpg_character_skills_common.h rpg_character_skills_common_tools.cpp \
	rpg_character_skills_common_tools.h
libRPG_Character_la_LDFLAGS = -L/usr/local/lib

noinst_PROGRAMS = generator

generator_SOURCES = main.cpp
generator_LDADD = $(top_builddir)/libRPG_Character.la -lACE -lpthread \
	-lxerces-c -lRPG_Chance_Dice -lRPG_Item
generator_LDFLAGS = -L/usr/local/lib

bin_PROGRAMS = character_parser

character_parser_SOURCES = character_parser.cpp
character_parser_LDADD = $(top_builddir)/libRPG_Character.la -lACE -lpthread \
	-lRPG_Chance_Dice -lRPG_Item -lxerces-c
character_parser_LDFLAGS = -L/usr/local/lib

bindata_DATA = rpg_character.xsd rpg_character.xml
bindatadir = $(bindir)
