METASOURCES = AUTO

include_HEADERS = rpg_character_XML_parser.h rpg_character_XML_parser_base.h \
	rpg_character_XML_types.h rpg_character_ability.h rpg_character_alignment.h \
	rpg_character_alignmentcivic.h rpg_character_alignmentethic.h rpg_character_attackform.h \
	rpg_character_attribute.h rpg_character_attributes.h rpg_character_base.h rpg_character_class.h \
	rpg_character_class_common.h rpg_character_climate.h rpg_character_common.h \
	rpg_character_common_tools.h rpg_character_condition.h rpg_character_dictionary.h \
	rpg_character_environment.h rpg_character_equipment.h rpg_character_equipment_common.h \
	rpg_character_feat.h rpg_character_feats.h rpg_character_gender.h rpg_character_inventory.h \
	rpg_character_inventory_common.h rpg_character_metaclass.h rpg_character_monster.h \
	rpg_character_monster_common.h rpg_character_monsteradvancement.h \
	rpg_character_monsteradvancementstep.h rpg_character_monsterarmorclass.h rpg_character_monsterattack.h \
	rpg_character_monsterattackaction.h rpg_character_monstermetatype.h rpg_character_monsterpropertiesxml.h \
	rpg_character_monstersubtype.h rpg_character_monstertype.h rpg_character_monsterweapon.h \
	rpg_character_organization.h rpg_character_organizationslaverstep.h rpg_character_organizationstep.h \
	rpg_character_plane.h rpg_character_player.h rpg_character_player_base.h \
	rpg_character_player_common.h rpg_character_race.h rpg_character_race_common.h \
	rpg_character_savingthrowmodifiers.h rpg_character_size.h rpg_character_skill.h rpg_character_skills.h \
	rpg_character_skills_common.h rpg_character_skills_common_tools.h rpg_character_subclass.h \
	rpg_character_terrain.h

lib_LTLIBRARIES = libRPG_Character.la

libRPG_Character_la_SOURCES = rpg_XMLSchema.h rpg_character_XML_parser.cpp \
	rpg_character_XML_parser.h rpg_character_XML_parser_base.cpp rpg_character_XML_parser_base.h \
	rpg_character_XML_types.cpp rpg_character_XML_types.h rpg_character_base.cpp rpg_character_base.h \
	rpg_character_class_common.cpp rpg_character_class_common.h rpg_character_climate.h \
	rpg_character_common.h rpg_character_common_tools.cpp rpg_character_common_tools.h \
	rpg_character_dictionary.cpp rpg_character_equipment.cpp rpg_character_equipment.h \
	rpg_character_equipment_common.h rpg_character_inventory.cpp rpg_character_inventory.h \
	rpg_character_monster.cpp rpg_character_monster.h rpg_character_monster_common.h \
	rpg_character_monster_common_tools.cpp rpg_character_monster_common_tools.h rpg_character_organizationstep.h \
	rpg_character_plane.h rpg_character_player.cpp rpg_character_player.h \
	rpg_character_player_base.cpp rpg_character_player_base.h rpg_character_player_common.h \
	rpg_character_race_common.h rpg_character_skills_common.h rpg_character_skills_common_tools.cpp \
	rpg_character_skills_common_tools.h rpg_character_terrain.h
libRPG_Character_la_LDFLAGS = -L/usr/local/lib

bin_PROGRAMS = character_generator character_parser

character_generator_SOURCES = main.cpp
character_generator_LDADD = $(top_builddir)/libRPG_Character.la -lACE -lpthread \
	-lxerces-c -lRPG_Chance_Dice -lRPG_Chance -lRPG_Item
character_generator_LDFLAGS = -L/usr/local/lib

character_parser_SOURCES = character_parser.cpp
character_parser_LDADD = $(top_builddir)/libRPG_Character.la -lACE -lpthread \
	-lxerces-c -lRPG_Chance_Dice -lRPG_Chance -lRPG_Item
character_parser_LDFLAGS = -L/usr/local/lib

bindata_DATA = rpg_character.xsd rpg_character.xml
bindatadir = $(bindir)
