include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Net)
set (${LIBRARY}_SRCS
     stdafx.h
     rpg_net_common.h
     rpg_net_common_tools.cpp
     rpg_net_common_tools.h
     rpg_net_defines.h
     rpg_net_eventhandler.cpp
     rpg_net_eventhandler.h
     rpg_net_module_eventhandler.cpp
     rpg_net_module_eventhandler.h
     rpg_net_module_protocolhandler.cpp
     rpg_net_module_protocolhandler.h
     rpg_net_packet_headers.h
     rpg_net_remote_comm.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_NET_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

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
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/event>)
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
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src/protocol/irc>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
                     #${CMAKE_CURRENT_SOURCE_DIR}/stream)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocol>)

#add_dependencies (RPG_Net RPG_Stream)

#set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

if (WIN32)
 target_link_libraries (${LIBRARY}
                        CommonError)
endif (WIN32)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         rpg_net_common.h
         rpg_net_common_tools.h
         rpg_net_defines.h
         rpg_net_eventhandler.h
         rpg_net_module_eventhandler.h
         rpg_net_module_protocolhandler.h
         rpg_net_packet_headers.h
         rpg_net_remote_comm.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY DESTINATION lib)

#add_subdirectory(stream)
add_subdirectory (protocol)
