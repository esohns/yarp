include (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui/scripts/PreUIFramework.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui/scripts/SetUIFramework.cmake)

if (CURSES_USE)
elseif (GTK_USE)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui/scripts/SetGTKVersion.cmake)

 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-DG_ERRORCHECK_MUTEXES)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
elseif (WXWIDGETS_USE)
 message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
 message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
 message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
 message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
# # *NOTE*: re-enable ANSI mode, iff available
# add_definitions (-DwxUSE_UNICODE=0)
 foreach (DEFINITION ${wxWidgets_DEFINITIONS})
  add_definitions (-D${DEFINITION})
 endforeach ()
 if (WIN32)
#  add_definitions (-D__WXMSW__)
#  add_definitions (-DWXUSINGDLL)
  if (MSVC)
   add_definitions (-DwxMSVC_VERSION_AUTO)
  endif (MSVC)
 endif (WIN32)
 add_definitions (-DwxUSE_GUI=1)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#   add_definitions (-D__WXDEBUG__)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)
endif ()

include (${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/sdl.cmake)

if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
include_directories (${GTK_INCLUDE_DIRS})
if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL_mixer/include)
endif ()

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/include)
endif (WIN32)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/ui/gtk)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd_party/xsd
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

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG\"")

# add_definitions(-DDATADIR="/usr/local/share/RPG/config")

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
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
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

set_source_files_properties (${${EXECUTABLE_2}_SRCS} PROPERTIES COMPILE_DEFINITIONS BASEDIR="/usr/local/share/RPG/config")

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
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
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

set_source_files_properties (${${EXECUTABLE_3}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"/usr/local/share/RPG\";CONFIGDIR=\".\"")

add_executable (${EXECUTABLE_3}
                ${${EXECUTABLE_3}_SRCS})

#add_definitions (-D_WIN32_WINNT=0x0602)
#add_definitions (-DWINVER=0x0602)

include_directories (${GTK_INCLUDE_DIRS}
#                     $ENV{LIB_ROOT}/gtkglarea
                     ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

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
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
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
                         ARGS "-t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND)
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-t"
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
                         ARGS "-t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${SDL_LIB_PATH}
                                              ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${SDL_LIB_PATH}
                                              ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
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
                                              ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                                              ${GTK_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
endif ()
