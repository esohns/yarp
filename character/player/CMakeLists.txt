#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Character_Player_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_character_player.cpp
    rpg_character_player.h
    rpg_character_player_XML_tree.cpp
    rpg_character_player_XML_tree.h
    rpg_character_player_base.cpp
    rpg_character_player_base.h
    rpg_character_player_common.h
    rpg_character_player_common_tools.cpp
    rpg_character_player_common_tools.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Character_Player_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Character_Player SHARED ${RPG_Character_Player_LIB_SRCS})

# add_custom_target(player_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_player.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_player_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_player_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_player.xsd
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Character_Player RPG_Magic RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Character_Player PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Character_Player LIBRARY
        DESTINATION lib)
install(FILES rpg_character_player.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_XMLSchema_XML_tree.h
        rpg_character_player.h
        rpg_character_player_XML_tree.h
        rpg_character_player_base.h
        rpg_character_player_common.h
        rpg_character_player_common_tools.h
        DESTINATION include)
