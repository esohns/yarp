cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../net
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../net/stream)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(protocol)

########### next target ###############
set(net_client_SRCS
    net_client.cpp
    net_client_signalhandler.cpp
    net_client_signalhandler.h
    net_client_timeouthandler.cpp
    net_client_timeouthandler.h
    net_common.h)

add_executable(net_client ${net_client_SRCS})

target_link_libraries(net_client pthread ACE xerces-c RPG_Dice RPG_Common RPG_Stream RPG_Net)

install(TARGETS net_client RUNTIME DESTINATION bin)

########### next target ###############
set(net_server_SRCS
    net_server.cpp
    net_server_signalhandler.cpp
    net_server_signalhandler.h)

add_executable(net_server ${net_server_SRCS})

target_link_libraries(net_server pthread ACE xerces-c RPG_Dice RPG_Common RPG_Stream RPG_Net)

install(TARGETS net_server RUNTIME DESTINATION bin)
