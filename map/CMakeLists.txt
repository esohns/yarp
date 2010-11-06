#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(scripts)

########### next target ###############

set(RPG_Map_LIB_SRCS
    location.hh
    position.hh
    rpg_map_common.h
    rpg_map_common_tools.cpp
    rpg_map_common_tools.h
    rpg_map_defines.h
    rpg_map_level.cpp
    rpg_map_level.h
    rpg_map_parser.cpp
    rpg_map_parser.h
    rpg_map_parser_driver.cpp
    rpg_map_parser_driver.h
    rpg_map_pathfinding_tools.cpp
    rpg_map_pathfinding_tools.h
    rpg_map_scanner.cpp
    rpg_map_scanner.h
    stack.hh)

add_library(RPG_Map SHARED ${RPG_Map_LIB_SRCS})

set_target_properties(RPG_Map PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Map LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_map_common.h rpg_map_common_tools.h rpg_map_defines.h  	rpg_map_level.h rpg_map_parser.h rpg_map_parser_driver.h rpg_map_pathfinding_tools.h  	rpg_map_scanner.h location.hh position.hh stack.hh DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = scripts
#
#include_HEADERS = rpg_map_common.h rpg_map_common_tools.h rpg_map_defines.h \
#	rpg_map_level.h rpg_map_parser.h rpg_map_parser_driver.h rpg_map_pathfinding_tools.h \
#	rpg_map_scanner.h location.hh position.hh stack.hh
#
#lib_LTLIBRARIES = libRPG_Map.la
#
#libRPG_Map_la_SOURCES = location.hh position.hh rpg_map_common.h \
#	rpg_map_common_tools.cpp rpg_map_common_tools.h rpg_map_defines.h rpg_map_level.cpp \
#	rpg_map_level.h rpg_map_parser.cpp rpg_map_parser.h rpg_map_parser_driver.cpp \
#	rpg_map_parser_driver.h rpg_map_pathfinding_tools.cpp rpg_map_pathfinding_tools.h \
#	rpg_map_scanner.cpp rpg_map_scanner.h stack.hh
