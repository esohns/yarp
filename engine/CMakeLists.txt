include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Engine)
set (${LIBRARY}_SRCS
     rpg_engine.cpp
     rpg_engine.h
     rpg_engine_command.h
     rpg_engine_common.h
     rpg_engine_common_tools.cpp
     rpg_engine_common_tools.h
     rpg_engine_defines.h
     rpg_engine_entitymode.h
     rpg_engine_event_common.h
     rpg_engine_event_manager.cpp
     rpg_engine_event_manager.h
   #  rpg_engine_exports.h
     rpg_engine_iclient.h
     rpg_engine_incl.h
     rpg_engine_level.cpp
     rpg_engine_level.h
     rpg_engine_messagequeue.cpp
     rpg_engine_messagequeue.h
     rpg_engine_XML_tree.cpp
     rpg_engine_XML_tree.h)
 #   rpg_XMLSchema_XML_tree.h
 #   rpg_XMLSchema_XML_types.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_ENGINE_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

#if (WIN32)
# *WORKAROUND*: 'DOMDocument' is declared multiple times, we want the xerces-c one
# add_definitions (-D__msxml_h__)
# add_definitions (-D__urlmon_h__)
#endif (WIN32)

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

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/error>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/parser>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/signal>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/timer>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/3rd_party/tree>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/lib>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/misc>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/net>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/stat>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src/client_server>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/bison>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../magic>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../item>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../combat>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character/player>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character/monster>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../map>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../graphics>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../net>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../net/protocol>)

# add_custom_target(engine_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_engine.xsd
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Graphics RPG_Map
                             RPG_Combat RPG_Character RPG_Player RPG_Monster
                             RPG_Item RPG_Magic RPG_Common RPG_Chance RPG_Dice)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES rpg_engine.xsd
         DESTINATION share/RPG/config)
install (FILES
         rpg_engine.h
         rpg_engine_command.h
         rpg_engine_common.h
         rpg_engine_common_tools.h
         rpg_engine_defines.h
         rpg_engine_entitymode.h
         rpg_engine_event_common.h
         rpg_engine_event_manager.h
#         rpg_engine_exports.h
         rpg_engine_iclient.h
         rpg_engine_incl.h
         rpg_engine_level.h
         rpg_engine_messagequeue.h
         rpg_engine_XML_tree.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
