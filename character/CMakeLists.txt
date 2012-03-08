cmake_minimum_required(VERSION 2.8)

add_subdirectory(monster)
add_subdirectory(player)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../chance
                    ${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../combat)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Character_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_character_XML_parser.cpp
    rpg_character_XML_parser.h
    rpg_character_XML_tree.cpp
    rpg_character_XML_tree.h
    rpg_character_XML_types.cpp
    rpg_character_XML_types.h
    rpg_character_ability.h
    rpg_character_skillvalue.h
    rpg_character_skills.h
    rpg_character_feats.h
    rpg_character_alignment.h
    rpg_character_alignmentcivic.h
    rpg_character_alignmentethic.h
    rpg_character_attributes.h
    rpg_character_exports.h
    rpg_character_check.h
    rpg_character_checktypeunion.h
    rpg_character_class_common.h
    rpg_character_class_common_tools.cpp
    rpg_character_class_common_tools.h
    rpg_character_classxml.h
    rpg_character_common.h
    rpg_character_common_tools.cpp
    rpg_character_common_tools.h
    rpg_character_XML_tools.cpp
    rpg_character_XML_tools.h
    rpg_character_feat.h
    rpg_character_gender.h
    rpg_character_incl.h
    rpg_character_equipmentslot.h
    rpg_character_metaclass.h
    rpg_character_offhand.h
    rpg_character_race.h
    rpg_character_race_common.h
    rpg_character_race_common_tools.cpp
    rpg_character_race_common_tools.h
    rpg_character_skills_common.h
    rpg_character_skills_common_tools.cpp
    rpg_character_skills_common_tools.h)

set_source_files_properties(${RPG_Character_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_CHARACTER_BUILD_DLL)
if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Character_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Character SHARED ${RPG_Character_LIB_SRCS})

# add_custom_target(character_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_types.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.xsd ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Character PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_character.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_XMLSchema_XML_tree.h
        rpg_character_XML_parser.h
        rpg_character_XML_tree.h
        rpg_character_XML_types.h
        rpg_character_ability.h
        rpg_character_skillvalue.h
        rpg_character_skills.h
        rpg_character_feats.h
        rpg_character_alignment.h
        rpg_character_alignmentcivic.h
        rpg_character_alignmentethic.h
        rpg_character_attributes.h
        rpg_character_exports.h
        rpg_character_check.h
        rpg_character_checktypeunion.h
        rpg_character_class_common.h
        rpg_character_class_common_tools.h
        rpg_character_classxml.h
        rpg_character_common.h
        rpg_character_common_tools.h
        rpg_character_XML_tools.h
        rpg_character_equipmentslot.h
        rpg_character_feat.h
        rpg_character_gender.h
        rpg_character_incl.h
        rpg_character_metaclass.h
        rpg_character_offhand.h
        rpg_character_race.h
        rpg_character_race_common.h
        rpg_character_race_common_tools.h
        rpg_character_skills_common.h
        rpg_character_skills_common_tools.h
        DESTINATION include)
install(TARGETS RPG_Character LIBRARY DESTINATION lib)
