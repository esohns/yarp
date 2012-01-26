#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

add_subdirectory(monster)
add_subdirectory(player)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chance")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../magic")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../item")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../combat")

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
    rpg_character_base.cpp
    rpg_character_base.h
    rpg_character_check.h
    rpg_character_checktypeunion.h
    rpg_character_class_common.h
    rpg_character_class_common_tools.cpp
    rpg_character_class_common_tools.h
    rpg_character_classxml.h
    rpg_character_common.h
    rpg_character_common_tools.cpp
    rpg_character_common_tools.h
    rpg_character_equipment.cpp
    rpg_character_equipment.h
    rpg_character_equipment_common.h
    rpg_character_feat.h
    rpg_character_gender.h
    rpg_character_incl.h
    rpg_character_inventory.cpp
    rpg_character_inventory.h
    rpg_character_metaclass.h
    rpg_character_offhand.h
    rpg_character_race.h
    rpg_character_race_common.h
    rpg_character_race_common_tools.cpp
    rpg_character_race_common_tools.h
    rpg_character_skills_common.h
    rpg_character_skills_common_tools.cpp
    rpg_character_skills_common_tools.h)

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
install(TARGETS RPG_Character LIBRARY DESTINATION lib)

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
        rpg_character_base.h
        rpg_character_check.h
        rpg_character_checktypeunion.h
        rpg_character_class_common.h
        rpg_character_class_common_tools.h
        rpg_character_classxml.h
        rpg_character_common.h
        rpg_character_common_tools.h
        rpg_character_equipment.h
        rpg_character_equipment_common.h
        rpg_character_equipmentslot.h
        rpg_character_feat.h
        rpg_character_gender.h
        rpg_character_incl.h
        rpg_character_inventory.h
        rpg_character_inventory_common.h
        rpg_character_metaclass.h
        rpg_character_offhand.h
        rpg_character_race.h
        rpg_character_race_common.h
        rpg_character_race_common_tools.h
        rpg_character_skills_common.h
        rpg_character_skills_common_tools.h
        DESTINATION include)
