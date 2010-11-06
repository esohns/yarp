
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############

set(RPG_Magic_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_magic_XML_parser.cpp
    rpg_magic_XML_parser.h
    rpg_magic_XML_tree.cpp
    rpg_magic_XML_tree.h
    rpg_magic_XML_types.cpp
    rpg_magic_XML_types.h
    rpg_magic_abilityclass.h
    rpg_magic_abilitytype.h
    rpg_magic_casterclassunion.h
    rpg_magic_check.h
    rpg_magic_checktypeunion.h
    rpg_magic_common.h
    rpg_magic_common_tools.cpp
    rpg_magic_common_tools.h
    rpg_magic_countermeasure.h
    rpg_magic_defines.h
    rpg_magic_descriptor.h
    rpg_magic_dictionary.cpp
    rpg_magic_dictionary.h
    rpg_magic_domain.h
    rpg_magic_incl.h
    rpg_magic_school.h
    rpg_magic_spell_castingtime.h
    rpg_magic_spell_duration.h
    rpg_magic_spell_durationproperties.h
    rpg_magic_spell_effect.h
    rpg_magic_spell_effectproperties.h
    rpg_magic_spell_level.h
    rpg_magic_spell_precondition.h
    rpg_magic_spell_preconditionproperties.h
    rpg_magic_spell_propertiesxml.h
    rpg_magic_spell_rangeeffect.h
    rpg_magic_spell_rangeproperties.h
    rpg_magic_spell_target.h
    rpg_magic_spell_targetproperties.h
    rpg_magic_spell_type.h
    rpg_magic_spelllikeproperties.h
    rpg_magic_spelltype.h
    rpg_magic_subschool.h)

add_library(RPG_Magic SHARED ${RPG_Magic_LIB_SRCS})

set_target_properties(RPG_Magic PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS RPG_Magic LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_magic.xml rpg_magic.xsd DESTINATION share/RPG/config)

install(FILES  rpg_magic_XML_parser.h rpg_magic_XML_tree.h  	rpg_magic_XML_types.h rpg_magic_abilityclass.h rpg_magic_abilitytype.h  	rpg_magic_casterclassunion.h rpg_magic_check.h rpg_magic_checktypeunion.h rpg_magic_common.h  	rpg_magic_common_tools.h rpg_magic_countermeasure.h rpg_magic_defines.h rpg_magic_descriptor.h  	rpg_magic_dictionary.h rpg_magic_domain.h rpg_magic_incl.h rpg_magic_school.h  	rpg_magic_spell_castingtime.h rpg_magic_spell_duration.h rpg_magic_spell_durationproperties.h  	rpg_magic_spell_effect.h rpg_magic_spell_effectproperties.h rpg_magic_spell_level.h  	rpg_magic_spell_precondition.h rpg_magic_spell_preconditionproperties.h  	rpg_magic_spell_propertiesxml.h rpg_magic_spell_rangeeffect.h rpg_magic_spell_rangeproperties.h  	rpg_magic_spell_target.h rpg_magic_spell_targetproperties.h rpg_magic_spell_type.h  	rpg_magic_spelllikeproperties.h rpg_magic_spelltype.h rpg_magic_subschool.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#noinst_HEADERS = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h
#
#include_HEADERS = rpg_magic_XML_parser.h rpg_magic_XML_tree.h \
#	rpg_magic_XML_types.h rpg_magic_abilityclass.h rpg_magic_abilitytype.h \
#	rpg_magic_casterclassunion.h rpg_magic_check.h rpg_magic_checktypeunion.h rpg_magic_common.h \
#	rpg_magic_common_tools.h rpg_magic_countermeasure.h rpg_magic_defines.h rpg_magic_descriptor.h \
#	rpg_magic_dictionary.h rpg_magic_domain.h rpg_magic_incl.h rpg_magic_school.h \
#	rpg_magic_spell_castingtime.h rpg_magic_spell_duration.h rpg_magic_spell_durationproperties.h \
#	rpg_magic_spell_effect.h rpg_magic_spell_effectproperties.h rpg_magic_spell_level.h \
#	rpg_magic_spell_precondition.h rpg_magic_spell_preconditionproperties.h \
#	rpg_magic_spell_propertiesxml.h rpg_magic_spell_rangeeffect.h rpg_magic_spell_rangeproperties.h \
#	rpg_magic_spell_target.h rpg_magic_spell_targetproperties.h rpg_magic_spell_type.h \
#	rpg_magic_spelllikeproperties.h rpg_magic_spelltype.h rpg_magic_subschool.h
#
#lib_LTLIBRARIES = libRPG_Magic.la
#
#libRPG_Magic_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h \
#	rpg_magic_XML_parser.cpp rpg_magic_XML_parser.h rpg_magic_XML_tree.cpp rpg_magic_XML_tree.h \
#	rpg_magic_XML_types.cpp rpg_magic_XML_types.h rpg_magic_abilityclass.h rpg_magic_abilitytype.h \
#	rpg_magic_casterclassunion.h rpg_magic_check.h rpg_magic_checktypeunion.h rpg_magic_common.h \
#	rpg_magic_common_tools.cpp rpg_magic_common_tools.h rpg_magic_countermeasure.h rpg_magic_defines.h \
#	rpg_magic_descriptor.h rpg_magic_dictionary.cpp rpg_magic_dictionary.h rpg_magic_domain.h \
#	rpg_magic_incl.h rpg_magic_school.h rpg_magic_spell_castingtime.h \
#	rpg_magic_spell_duration.h rpg_magic_spell_durationproperties.h rpg_magic_spell_effect.h \
#	rpg_magic_spell_effectproperties.h rpg_magic_spell_level.h rpg_magic_spell_precondition.h \
#	rpg_magic_spell_preconditionproperties.h rpg_magic_spell_propertiesxml.h rpg_magic_spell_rangeeffect.h \
#	rpg_magic_spell_rangeproperties.h rpg_magic_spell_target.h rpg_magic_spell_targetproperties.h \
#	rpg_magic_spell_type.h rpg_magic_spelllikeproperties.h rpg_magic_spelltype.h \
#	rpg_magic_subschool.h
#
#pkgdatadir = $(datadir)/@META_PACKAGE@
#configdir = $(pkgdatadir)/config
#config_DATA = rpg_magic.xml rpg_magic.xsd
#
#EXTRA_DIST = $(config_DATA)
