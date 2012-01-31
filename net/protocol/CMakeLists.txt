#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

########### next target ###############
set(RPG_Net_Protocol_LIB_SRCS
    location.hh
    position.hh
    rpg_net_protocol_IRC_codes.h
    rpg_net_protocol_IRCmessage.cpp
    rpg_net_protocol_IRCmessage.h
    rpg_net_protocol_IRCparser.cpp
    rpg_net_protocol_IRCparser.h
    rpg_net_protocol_IRCparser_driver.cpp
    rpg_net_protocol_IRCparser_driver.h
    rpg_net_protocol_IRCbisect.cpp
    rpg_net_protocol_IRCbisect.h
    rpg_net_protocol_IRCscanner.cpp
    rpg_net_protocol_IRCscanner.h
    rpg_net_protocol_common.h
    rpg_net_protocol_defines.h
    rpg_net_protocol_iIRCControl.h
    rpg_net_protocol_inotify.h
    rpg_net_protocol_message.cpp
    rpg_net_protocol_message.h
    rpg_net_protocol_messageallocator.cpp
    rpg_net_protocol_messageallocator.h
    rpg_net_protocol_module_IRChandler.cpp
    rpg_net_protocol_module_IRChandler.h
    rpg_net_protocol_module_IRCparser.cpp
    rpg_net_protocol_module_IRCparser.h
    rpg_net_protocol_module_IRCsplitter.cpp
    rpg_net_protocol_module_IRCsplitter.h
    rpg_net_protocol_module_IRCstreamer.cpp
    rpg_net_protocol_module_IRCstreamer.h
    rpg_net_protocol_sessionmessage.cpp
    rpg_net_protocol_sessionmessage.h
    rpg_net_protocol_sockethandler.cpp
    rpg_net_protocol_sockethandler.h
    rpg_net_protocol_stream.cpp
    rpg_net_protocol_stream.h
    rpg_net_protocol_tools.cpp
    rpg_net_protocol_tools.h
    stack.hh)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Net_Protocol_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Net_Protocol_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_PROTOCOL_BUILD_DLL)

add_library(RPG_Net_Protocol SHARED ${RPG_Net_Protocol_LIB_SRCS})

# add_custom_target(protocol_scanner
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/protocol_scanner.l)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCbisect.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCbisect.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCscanner.cpp
${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCscanner.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/scanner.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCbisect.l ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCscanner.l
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating flex scanners...")

# add_custom_target(protocol_parser
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/protocol_parser.y)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCparser.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_net_protocol_IRCparser.h ${CMAKE_CURRENT_SOURCE_DIR}/location.hh ${CMAKE_CURRENT_SOURCE_DIR}/position.hh ${CMAKE_CURRENT_SOURCE_DIR}/stack.hh
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parser.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/IRCparser.y
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating bison parser...")

add_dependencies(RPG_Net_Protocol RPG_Net RPG_Stream)

set_target_properties(RPG_Net_Protocol PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Net_Protocol LIBRARY
        DESTINATION lib)
install(FILES
        location.hh
        position.hh
        rpg_net_protocol_IRC_codes.h
        rpg_net_protocol_IRCmessage.h
        rpg_net_protocol_IRCparser.h
        rpg_net_protocol_IRCparser_driver.h
        rpg_net_protocol_common.h
        rpg_net_protocol_defines.h
        rpg_net_protocol_iIRCControl.h
        rpg_net_protocol_inotify.h
        rpg_net_protocol_message.h
        rpg_net_protocol_messageallocator.h
        rpg_net_protocol_module_IRChandler.h
        rpg_net_protocol_module_IRCparser.h
        rpg_net_protocol_module_IRCsplitter.h
        rpg_net_protocol_module_IRCstreamer.h
        rpg_net_protocol_sessionmessage.h
        rpg_net_protocol_sockethandler.h
        rpg_net_protocol_stream.h
        rpg_net_protocol_tools.h
        stack.hh
        DESTINATION include)
