#include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

#include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Player)
set (${LIBRARY}_SRCS
  #  rpg_XMLSchema_XML_tree.h
     rpg_iplayer.h
     rpg_player.cpp
     rpg_player.h
     rpg_player_abilities.h
     rpg_player_base.cpp
     rpg_player_base.h
  #  rpg_player_basexml.h
     rpg_player_common.h
     rpg_player_common_tools.cpp
     rpg_player_common_tools.h
     rpg_player_conditions.h
     rpg_player_defines.h
     rpg_player_equipment.cpp
     rpg_player_equipment.h
     rpg_player_equipment_common.h
  #  rpg_player_exports.h
     rpg_player_inventory.cpp
     rpg_player_inventory.h
     rpg_player_player_base.cpp
     rpg_player_player_base.h
     rpg_player_playerxml.h
     rpg_player_spells.h
     rpg_player_XML_tree.cpp
     rpg_player_XML_tree.h
     rpg_player_XML_types.cpp
     rpg_player_XML_types.h)

#set_source_files_properties(${RPG_Player_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS "RPG_PLAYER_BUILD_DLL;RPG_CHARACTER_HAS_DLL")

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (UNIX)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/build/msvc/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../magic>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../item>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../combat>)

# add_custom_target(player_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_player.xsd
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Character RPG_Magic RPG_Item)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         rpg_player.xsd
         DESTINATION share/RPG/config)
install (FILES
         rpg_iplayer.h
         rpg_player.h
         rpg_player_abilities.h
         rpg_player_base.h
#	        rpg_player_basexml.h
         rpg_player_common.h
         rpg_player_common_tools.h
         rpg_player_conditions.h
         rpg_player_defines.h
         rpg_player_equipment.h
         rpg_player_equipment_common.h
#	        rpg_player_exports.h
         rpg_player_inventory.h
         rpg_player_player_base.h
         rpg_player_playerxml.h
         rpg_player_spells.h
         rpg_player_XML_tree.h
         rpg_player_XML_types.h
  #      rpg_XMLSchema_XML_tree.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
