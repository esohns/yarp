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
    rpg_magic_spell_damagetypeunion.h
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
    rpg_magic_spell_areaofeffect.h
    rpg_magic_spell_targetproperties.h
    rpg_magic_spell_type.h
    rpg_magic_spelllikeproperties.h
    rpg_magic_spelltype.h
    rpg_magic_subschool.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Magic_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Magic SHARED ${RPG_Magic_LIB_SRCS})

# add_custom_target(magic_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_types.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.xsd ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Magic RPG_Common)

set_target_properties(RPG_Magic PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Magic LIBRARY
        DESTINATION lib)
install(FILES rpg_magic.xml rpg_magic.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_magic_XML_parser.h
        rpg_magic_XML_tree.h
        rpg_magic_XML_types.h
        rpg_magic_abilityclass.h
        rpg_magic_abilitytype.h
        rpg_magic_casterclassunion.h
        rpg_magic_check.h
        rpg_magic_checktypeunion.h
        rpg_magic_common.h
        rpg_magic_common_tools.h
        rpg_magic_countermeasure.h
        rpg_magic_defines.h
        rpg_magic_descriptor.h
        rpg_magic_dictionary.h
        rpg_magic_domain.h
        rpg_magic_incl.h
        rpg_magic_school.h
        rpg_magic_spell_castingtime.h
        rpg_magic_spell_damagetypeunion.h
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
        rpg_magic_spell_areaofeffect.h
        rpg_magic_spell_targetproperties.h
        rpg_magic_spell_type.h
        rpg_magic_spelllikeproperties.h
        rpg_magic_spelltype.h
        rpg_magic_subschool.h
        DESTINATION include)
