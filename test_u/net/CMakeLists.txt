include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

########### next target ###############
set (EXECUTABLE net_client)
set (${EXECUTABLE}_SRCS
     stdafx.h
     net_callbacks.cpp
     net_callbacks.h
     net_client.cpp
     net_client_common.h
     net_client_eventhandler.cpp
     net_client_eventhandler.h
     net_client_signalhandler.cpp
     net_client_signalhandler.h
     net_defines.h)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
include_directories (${GTK_INCLUDE_DIRS})
if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL_mixer/include)
endif ()

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/include)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/event)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/parser)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../combat
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/player
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../map
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../sound
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net/protocol
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../client)

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        ${GTK_LIBRARIES}
                        X11
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Client)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib legacy_stdio_definitions.lib
                        ${GTK_LIBRARIES}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Client)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_2 net_server)
set (${EXECUTABLE_2}_SRCS
     stdafx.h
     net_callbacks.cpp
     net_callbacks.h
     net_defines.h
     net_server.cpp
     net_server_common.h
     net_server_eventhandler.cpp
     net_server_eventhandler.h
     net_server_signalhandler.cpp
     net_server_signalhandler.h)

add_executable (${EXECUTABLE_2}
                ${${EXECUTABLE_2}_SRCS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ${GTK_LIBRARIES}
                        X11
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Graphics RPG_Sound RPG_Client
                        ACEStream_Network
                        RPG_Net)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        Opengl32.lib legacy_stdio_definitions.lib
                        ${GTK_LIBRARIES}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Graphics RPG_Sound RPG_Client)
endif ()

set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           PRIVATE stdafx.h)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### launcher #####################
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
file (TO_CMAKE_PATH "${SDL_LIB_DIR}" SDL_LIB_PATH)
file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
endif ()
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS "-l -t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${XERCESC_LIB_DIR}
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${LIB_ROOT}/openssl
                                             ${SDL_LIB_PATH}
                                             ${GTK_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_2}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
endif ()
create_target_launcher (${EXECUTABLE_2}
                        COMMAND ${COMMAND}
                        ARGS "-l -t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${XERCESC_LIB_DIR}
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${LIB_ROOT}/openssl
                                             ${SDL_LIB_PATH}
                                             ${GTK_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
