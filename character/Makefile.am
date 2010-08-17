METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.


include_HEADERS = rpg_character_XML_parser.h rpg_character_XML_types.h \
	rpg_character_ability.h rpg_character_alignment.h rpg_character_alignmentcivic.h \
	rpg_character_alignmentethic.h rpg_character_attributes.h rpg_character_base.h rpg_character_check.h \
	rpg_character_checktypeunion.h rpg_character_class_common.h rpg_character_classxml.h \
	rpg_character_common.h rpg_character_common_tools.h rpg_character_equipment.h \
	rpg_character_equipment_common.h rpg_character_equipmentslot.h rpg_character_feat.h \
	rpg_character_feats.h rpg_character_gender.h rpg_character_incl.h rpg_character_inventory.h \
	rpg_character_inventory_common.h rpg_character_metaclass.h rpg_character_offhand.h \
	rpg_character_player.h rpg_character_player_base.h rpg_character_player_common.h \
	rpg_character_race.h rpg_character_race_common.h rpg_character_skills.h \
	rpg_character_skills_common.h rpg_character_skills_common_tools.h rpg_character_skillvalue.h

lib_LTLIBRARIES = libRPG_Character.la

libRPG_Character_la_SOURCES = rpg_XMLSchema_XML_types.h \
	rpg_character_XML_parser.cpp rpg_character_XML_parser.h rpg_character_XML_types.cpp \
	rpg_character_XML_types.h rpg_character_ability.h rpg_character_alignment.h \
	rpg_character_attributes.h rpg_character_base.cpp rpg_character_base.h rpg_character_check.h \
	rpg_character_checktypeunion.h rpg_character_class_common.cpp rpg_character_class_common.h \
	rpg_character_classxml.h rpg_character_common.h rpg_character_common_tools.cpp \
	rpg_character_common_tools.h rpg_character_equipment.cpp rpg_character_equipment.h \
	rpg_character_equipment_common.h rpg_character_feat.h rpg_character_feats.h rpg_character_gender.h \
	rpg_character_incl.h rpg_character_inventory.cpp rpg_character_inventory.h \
	rpg_character_offhand.h rpg_character_player.cpp rpg_character_player.h \
	rpg_character_player_base.cpp rpg_character_player_base.h rpg_character_player_common.h \
	rpg_character_race.h rpg_character_race_common.h rpg_character_skills.h \
	rpg_character_skills_common.h rpg_character_skills_common_tools.cpp \
	rpg_character_skills_common_tools.h rpg_character_skillvalue.h
libRPG_Character_la_LDFLAGS = -L/usr/local/lib

bindata_DATA = rpg_character.xsd
bindatadir = $(bindir)
noinst_HEADERS = rpg_XMLSchema_XML_types.h
