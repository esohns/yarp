include (${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/sdl.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
if (GTK_SUPPORT)
 include_directories (${GTK_INCLUDE_DIRS})
endif (GTK_SUPPORT)
if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                      ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL_mixer/include
                      $ENV{LIB_ROOT}/libpng/include)
endif ()

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/include)
endif (WIN32)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd_party/xsd
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../item
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../character
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../magic
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../combat
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/player
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/monster
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../map
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../engine
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../client
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../graphics
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../sound)

########### next target ###############
set (EXECUTABLE character_parser)
set (${EXECUTABLE}_SRCS
     character_parser.cpp
     stdafx.h)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        pthread
#                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Graphics RPG_Engine
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Common
                        Common)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item
                        RPG_Character RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Graphics RPG_Engine)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_2 character_generator)
set (${EXECUTABLE_2}_SRCS
     character_generator.cpp
     stdafx.h)

add_executable (${EXECUTABLE_2}
                ${${EXECUTABLE_2}_SRCS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE_2}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_2}
#                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Graphics RPG_Engine
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Common RPG_Monster RPG_Combat RPG_Map
                        RPG_Player)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_2}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster RPG_Combat
                        RPG_Map RPG_Graphics RPG_Net RPG_Net_Protocol RPG_Engine)
endif ()

set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})

########### install files ###############
install (TARGETS ${EXECUTABLE_2} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_3 character_generator_gui)
set (${EXECUTABLE_3}_SRCS
     character_generator_gui.cpp
     character_generator_gui_callbacks.cpp
     character_generator_gui_callbacks.h
     character_generator_gui_common.h
     test_u_main.cpp
     test_u_main.h
     stdafx.h)

add_executable (${EXECUTABLE_3}
                ${${EXECUTABLE_3}_SRCS})

#include_directories (${GTK_INCLUDE_DIRS}
#                     ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE_3}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_3}
                        ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        X11
#                        ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES}
                        ${GTK_LIBRARIES}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Graphics RPG_Engine RPG_Client
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Dice RPG_Common RPG_Character RPG_Monster
                        RPG_Combat RPG_Map
                        RPG_Player RPG_Engine RPG_Graphics RPG_Sound
                        RPG_Monster RPG_Combat RPG_Map RPG_Net_Protocol RPG_Net
                        Common
                        ACEStream_Network)
elseif (WIN32)
# set_target_properties (${EXECUTABLE_3} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_3}
                        Opengl32.lib Ntdll.lib
                        $ENV{LIB_ROOT}/SDL_mixer/lib/${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}/SDL_mixer.lib
                        ${GTK_LIBRARIES}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonError CommonLog CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item
                        RPG_Character RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Sound RPG_Graphics RPG_Engine RPG_Client
                        RPG_Net RPG_Net_Protocol)
endif ()

set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_3}
                           REUSE_FROM ${EXECUTABLE})

########### install files ###############
install (TARGETS ${EXECUTABLE_3} RUNTIME DESTINATION bin)
install (FILES ${EXECUTABLE_3}.glade DESTINATION share/RPG/config)

########### launcher ###############
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND)
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}" COMMAND)
 create_target_launcher (${EXECUTABLE_3}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY}
                         DIRECTORY)
# file (TO_NATIVE_PATH $ENV{LIB_ROOT} LIB_ROOT)
# string (REPLACE "\\" "\\\\" LIB_ROOT ${LIB_ROOT})
 file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
 file (TO_CMAKE_PATH "${SDL_LIB_DIR}" SDL_LIB_PATH)
 file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${SDL_LIB_PATH}
                                              ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${SDL_LIB_PATH}
                                              ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE_3}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${LIB_ROOT}/libepoxy/bin
                                              ${LIB_ROOT}/openssl
                                              ${SDL_LIB_PATH}
                                              ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                                              ${GTK_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
endif ()
