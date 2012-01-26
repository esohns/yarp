#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chance")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../character")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../magic")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../item")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../combat")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../character/player")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../character/monster")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../map")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../graphics")

########### next target ###############
set(RPG_Engine_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_engine_XML_tree.cpp
    rpg_engine_XML_tree.h
    rpg_engine_common.h
    rpg_engine_iwindow.h
    rpg_engine_incl.h
    rpg_engine_command.h
    rpg_engine_common_tools.cpp
    rpg_engine_common_tools.h
    rpg_engine_defines.h
    rpg_engine_messagequeue.cpp
    rpg_engine_messagequeue.h
    rpg_engine_level.cpp
    rpg_engine_level.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Engine_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Engine SHARED ${RPG_Engine_LIB_SRCS})

# add_custom_target(engine_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine.xsd
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Engine RPG_Graphics RPG_Map RPG_Combat RPG_Character RPG_Character_Monster RPG_Character_Player RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Engine PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Engine LIBRARY
        DESTINATION lib)
install(FILES rpg_engine.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_engine_XML_tree.h
        rpg_engine_common.h
        rpg_engine_incl.h
        rpg_engine_iwindow.h
        rpg_engine_command.h
        rpg_engine_common_tools.h
        rpg_engine_defines.h
        rpg_engine_messagequeue.h
        rpg_engine_level.h
        DESTINATION include)
