#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#add_subdirectory(scripts)

########### next target ###############

set(RPG_Map_LIB_SRCS
    location.hh
    position.hh
    rpg_map_common.h
    rpg_map_common_tools.cpp
    rpg_map_common_tools.h
    rpg_map_XML_tree.h
    rpg_map_XML_tree.cpp
    rpg_XMLSchema_XML_tree.h
    rpg_map_position.h
    rpg_map_incl.h
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

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Map_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Map SHARED ${RPG_Map_LIB_SRCS})

# add_custom_target(map_scanner
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_scanner.l)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_scanner.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_scanner.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/scanner.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_scanner.l
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating flex scanner...")

# add_custom_target(map_parser
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_parser.y)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_parser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_parser.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parser.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_parser.y
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating bison parser...")

# add_dependencies(map_parser map_scanner)

# add_custom_target(map_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map.xsd
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

#add_dependencies(RPG_Map map_parser map_bindings RPG_Common RPG_Dice)
add_dependencies(RPG_Map RPG_Common RPG_Dice)

set_target_properties(RPG_Map PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Map LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_map.xsd DESTINATION share/RPG/config)

install(FILES  rpg_map_common.h rpg_map_common_tools.h rpg_map_defines.h  	rpg_map_level.h rpg_map_parser.h rpg_map_parser_driver.h rpg_map_pathfinding_tools.h  	rpg_map_scanner.h location.hh position.hh stack.hh rpg_map_XML_tree.h rpg_map_position.h rpg_map_incl.h DESTINATION include)

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
