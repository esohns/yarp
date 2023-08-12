include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Map)
set (${LIBRARY}_SRCS
#     location.hh
#     position.hh
     rpg_map_common.h
     rpg_map_common_tools.cpp
     rpg_map_common_tools.h
     rpg_map_defines.h
     rpg_map_doorstate.h
 #    rpg_map_exports.h
     rpg_map_incl.h
     rpg_map_level.cpp
     rpg_map_level.h
     rpg_map_parser.cpp
     rpg_map_parser.h
     rpg_map_parser_driver.cpp
     rpg_map_parser_driver.h
     rpg_map_pathfinding_tools.cpp
     rpg_map_pathfinding_tools.h
     rpg_map_position.h
     rpg_map_scanner.cpp
     rpg_map_scanner.h
 #    stack.hh
     rpg_map_XML_tree.h
     rpg_map_XML_tree.cpp
     stdafx.h)
#     rpg_XMLSchema_XML_tree.h

set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS "YY_NO_UNISTD_H;BASEDIR=\"C:\\\\\"")
#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS "YY_NO_UNISTD_H;RPG_MAP_BUILD_DLL")

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
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/error>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/bison>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../engine>)

# add_custom_target(map_scanner
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_scanner.l)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_scanner.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_scanner.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/scanner.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_scanner.l
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating flex scanner...")

# add_custom_target(map_parser
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_parser.y)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_parser.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_parser.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/location.hh
#                           ${CMAKE_CURRENT_SOURCE_DIR}/position.hh
#                           ${CMAKE_CURRENT_SOURCE_DIR}/stack.hh
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parser.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/map_parser.y
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating bison parser...")

# add_dependencies(map_parser map_scanner)

# add_custom_target(map_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_map.xsd
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

#add_dependencies(RPG_Map map_parser map_bindings RPG_Common RPG_Dice)
add_dependencies (${LIBRARY} RPG_Dice RPG_Common)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         etc/rpg_map.xsd
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
         rpg_map_common.h
         rpg_map_common_tools.h
         rpg_map_defines.h
         rpg_map_doorstate.h
 #        rpg_map_exports.h
         rpg_map_incl.h
         rpg_map_level.h
         rpg_map_parser.h
         rpg_map_parser_driver.h
         rpg_map_pathfinding_tools.h
         rpg_map_position.h
         rpg_map_scanner.h
#         location.hh
#         position.hh
#         stack.hh
         rpg_map_XML_tree.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)

add_subdirectory (share)
