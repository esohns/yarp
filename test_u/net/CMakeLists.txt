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

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../magic)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../item)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../combat)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character/player)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../map)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../engine)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../sound)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../net)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../net/protocol)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../client)

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (GTK_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${GTK_LIBRARIES})
endif (GTK_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c)
 target_link_libraries (${EXECUTABLE}
                        ${ACE_LIBRARY})
 target_link_libraries (${EXECUTABLE}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK)
 target_link_libraries (${EXECUTABLE}
                        ACEStream ACEStream_Miscellaneous ACEStream_Network)
 target_link_libraries (${EXECUTABLE}
                        ACENetwork ACENetwork_Client)
 target_link_libraries (${EXECUTABLE}
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Dice RPG_Common RPG_Sound
                        RPG_Engine RPG_Player RPG_Monster RPG_Player RPG_Item RPG_Map RPG_Common
                        RPG_Character RPG_Combat RPG_Magic RPG_Chance RPG_Dice
                        RPG_Net RPG_Net_Protocol RPG_Net RPG_Client RPG_Graphics RPG_Sound RPG_Net_Protocol)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Graphics RPG_Sound RPG_Client)
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
if (GTK_SUPPORT)
 target_link_libraries (${EXECUTABLE_2}
                        ${GTK_LIBRARIES})
endif (GTK_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_2}
                        xerces-c)
 target_link_libraries (${EXECUTABLE_2}
                        ${ACE_LIBRARY})
 target_link_libraries (${EXECUTABLE_2}
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK)
 target_link_libraries (${EXECUTABLE_2}
                        ACEStream ACEStream_Miscellaneous ACEStream_Network)
 target_link_libraries (${EXECUTABLE_2}
                        ACENetwork ACENetwork_Server)
 target_link_libraries (${EXECUTABLE_2}
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Dice RPG_Common RPG_Sound
                        RPG_Engine RPG_Player RPG_Monster RPG_Player RPG_Item RPG_Map RPG_Common
                        RPG_Character RPG_Combat RPG_Magic RPG_Chance RPG_Dice
                        RPG_Net RPG_Net_Protocol RPG_Net RPG_Client RPG_Graphics RPG_Sound RPG_Net_Protocol)
elseif (WIN32)
# set_target_properties (${EXECUTABLE_2} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        Opengl32.lib)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ${XERCESC_LIBRARIES})
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ${ACE_LIBRARY})
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        Common CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ACEStream ACEStream_Miscellaneous ACEStream_Network)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        ACENetwork ACENetwork_Server)
 target_link_libraries (${EXECUTABLE_2} PUBLIC
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Combat RPG_Player RPG_Monster
                        RPG_Map RPG_Engine
                        RPG_Net RPG_Net_Protocol
                        RPG_Graphics RPG_Sound RPG_Client)
endif ()

set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})

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
