string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)
find_package (ACEStream
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES ACEStream/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
else ()
 message (STATUS "found package \"ACEStream\": \"${ACEStream_DIR}\"")
endif ()

find_package (ACENetwork
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES ACENetwork/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
else ()
 message (STATUS "found package \"ACENetwork\": \"${ACENetwork_DIR}\"")
endif ()

########### next target ###############
set (LIBRARY RPG_Net_Protocol)
set (${LIBRARY}_SRCS
     stdafx.h
     rpg_net_protocol_common.h
     rpg_net_protocol_common_modules.h
     rpg_net_protocol_configuration.h
     rpg_net_protocol_connection_manager.cpp
     rpg_net_protocol_connection_manager.h
     rpg_net_protocol_defines.h
     rpg_net_protocol_eventhandler.cpp
     rpg_net_protocol_eventhandler.h
#  rpg_net_protocol_exports.h
     rpg_net_protocol_handler.cpp
     rpg_net_protocol_handler.h
     rpg_net_protocol_iparser.h
     rpg_net_protocol_message.cpp
     rpg_net_protocol_message.h
     rpg_net_protocol_messageallocator.cpp
     rpg_net_protocol_messageallocator.h
     rpg_net_protocol_messagehandler.cpp
     rpg_net_protocol_messagehandler.h
     rpg_net_protocol_network.h
     rpg_net_protocol_parser.cpp
     rpg_net_protocol_parser.h
     rpg_net_protocol_parser_driver.cpp
     rpg_net_protocol_parser_driver.h
     rpg_net_protocol_scanner.cpp
     rpg_net_protocol_scanner.h
     rpg_net_protocol_session_message.cpp
     rpg_net_protocol_session_message.h
     rpg_net_protocol_stream.cpp
     rpg_net_protocol_stream.h
     rpg_net_protocol_stream_common.h
     rpg_net_protocol_streamer.cpp
     rpg_net_protocol_streamer.h
     rpg_net_protocol_tools.cpp
     rpg_net_protocol_tools.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_PROTOCOL_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/error>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/event>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/task>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/3rd_party/tree>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/misc>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/stat>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/client_server>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/protocol/irc>)

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/bison>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../character>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../magic>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../item>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../combat>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../character/player>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../map>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../engine>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../graphics>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../sound>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# add_custom_target(protocol_scanner
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/protocol_scanner.l)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCbisect.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCbisect.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCscanner.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCscanner.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/scanner.sh
#                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCbisect.l
#                            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCscanner.l
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating flex scanners...")

# add_custom_target(protocol_parser
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/protocol_parser.y)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCparser.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCparser.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/location.hh
#                           ${CMAKE_CURRENT_SOURCE_DIR}/position.hh
#                           ${CMAKE_CURRENT_SOURCE_DIR}/stack.hh
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parser.sh
#                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCparser.y
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating bison parser...")

add_dependencies (${LIBRARY} RPG_Net)
#RPG_Stream)

#set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

target_link_libraries (${LIBRARY}
                       ACEStream_Network)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         rpg_net_protocol_common.h
         rpg_net_protocol_common_modules.h
         rpg_net_protocol_configuration.h
         rpg_net_protocol_connection_manager.h
         rpg_net_protocol_defines.h
         rpg_net_protocol_eventhandler.h
#         rpg_net_protocol_exports.h
         rpg_net_protocol_handler.h
         rpg_net_protocol_iparser.h
         rpg_net_protocol_message.h
         rpg_net_protocol_messageallocator.h
         rpg_net_protocol_messagehandler.h
         rpg_net_protocol_network.h
         rpg_net_protocol_parser.h
         rpg_net_protocol_parser_driver.h
         rpg_net_protocol_scanner.h
         rpg_net_protocol_session_message.h
         rpg_net_protocol_stream.h
         rpg_net_protocol_stream_common.h
         rpg_net_protocol_streamer.h
         rpg_net_protocol_tools.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
