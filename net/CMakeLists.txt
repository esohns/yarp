cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/stream)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#add_subdirectory(stream)
add_subdirectory(protocol)

########### next target ###############
set(RPG_Net_LIB_SRCS
    rpg_net_common.h
    rpg_net_common_tools.cpp
    rpg_net_common_tools.h
    rpg_net_defines.h
 #   rpg_net_exports.h
    rpg_net_eventhandler.cpp
    rpg_net_eventhandler.h
    rpg_net_module_eventhandler.cpp
    rpg_net_module_eventhandler.h
    rpg_net_module_protocolhandler.cpp
    rpg_net_module_protocolhandler.h
    rpg_net_packet_headers.h
    rpg_net_remote_comm.h
    rpg_net_stream.cpp
    rpg_net_stream.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Net_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Net_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_NET_BUILD_DLL)

add_library(RPG_Net SHARED ${RPG_Net_LIB_SRCS})

add_dependencies(RPG_Net RPG_Stream)

set_target_properties(RPG_Net PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES
   #     rpg_net_client_sockethandler.h
        rpg_net_common.h
        rpg_net_common_tools.h
        rpg_net_defines.h
        rpg_net_eventhandler.h
      #  rpg_net_exports.h
        rpg_net_module_eventhandler.h
        rpg_net_module_protocolhandler.h
        rpg_net_packet_headers.h
        rpg_net_remote_comm.h
        rpg_net_stream.h
        DESTINATION include)
install(TARGETS RPG_Net LIBRARY DESTINATION lib)

