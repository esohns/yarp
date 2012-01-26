#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../character")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../magic")

########### next target ###############
set(magic_parser_SRCS magic_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${magic_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${magic_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(magic_parser ${magic_parser_SRCS})

target_link_libraries(magic_parser pthread ACE xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Item RPG_Character RPG_Magic)

install(TARGETS magic_parser RUNTIME DESTINATION bin)
