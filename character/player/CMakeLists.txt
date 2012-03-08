cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../combat)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Player_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_player_defines.h
    rpg_player_exports.h
    rpg_player.cpp
    rpg_player.h
    rpg_player_abilities.h
    rpg_player_XML_tree.cpp
    rpg_player_XML_tree.h
    rpg_player_base.cpp
    rpg_player_base.h
    rpg_player_basexml.h
    rpg_player_conditions.h
    rpg_player_equipment.cpp
    rpg_player_equipment.h
    rpg_player_equipment_common.h
    rpg_player_inventory.cpp
    rpg_player_inventory.h
    rpg_player_player_base.cpp
    rpg_player_player_base.h
    rpg_player_playerxml.h
    rpg_player_spells.h
    rpg_player_common.h
    rpg_player_common_tools.cpp
    rpg_player_common_tools.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Player_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Player_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS "RPG_PLAYER_BUILD_DLL;RPG_CHARACTER_HAS_DLL")

add_library(RPG_Player SHARED ${RPG_Player_LIB_SRCS})

# add_custom_target(player_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player_XML_tree.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player_XML_tree.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player.xsd
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Player RPG_Magic RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Player PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_player.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_XMLSchema_XML_tree.h
        rpg_player_defines.h
	rpg_player_exports.h
	rpg_player_abilities.h
	rpg_player_basexml.h
	rpg_player_conditions.h
	rpg_player_equipment.h
	rpg_player_equipment_common.h
	rpg_player_inventory.h
	rpg_player_player_base.h
	rpg_player_playerxml.h
	rpg_player_spells.h
        rpg_player.h
        rpg_player_XML_tree.h
        rpg_player_base.h
        rpg_player_common.h
        rpg_player_common_tools.h
        DESTINATION include)
install(TARGETS RPG_Player LIBRARY DESTINATION lib)
