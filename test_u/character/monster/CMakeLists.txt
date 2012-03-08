cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/monster)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(monster_parser_SRCS monster_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${monster_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${monster_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(monster_parser ${monster_parser_SRCS})

target_link_libraries(monster_parser ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic
                                     RPG_Item RPG_Character RPG_Monster RPG_Player RPG_Combat)

########### install files ###############
install(TARGETS monster_parser RUNTIME DESTINATION bin)
