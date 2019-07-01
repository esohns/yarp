########### next target ###############
set (EXECUTABLE net_client)
set (${EXECUTABLE}_SRCS
     net_callbacks.cpp
     net_callbacks.h
     net_client.cpp
     net_client_common.h
     net_client_signalhandler.cpp
     net_client_signalhandler.h
#    net_client_timeouthandler.cpp
#    net_client_timeouthandler.h
     net_defines.h
     net_eventhandler.cpp
     net_eventhandler.h)
#     net_module_eventhandler.cpp
#     net_module_eventhandler.h)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (WIN32)
# include_directories ($ENV{LIB_ROOT}/gtk2/include/gtk-2.0)
 include_directories (${GTK_INCLUDE_DIRS})
endif (WIN32)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net/protocol)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE
                       RPG_Dice RPG_Common RPG_Net)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE net_server)
set (${EXECUTABLE}_SRCS
     net_defines.h
     net_server.cpp
     net_server_common.h
     net_server_signalhandler.cpp
     net_server_signalhandler.h)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log>)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE
                       RPG_Dice RPG_Common RPG_Net)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
