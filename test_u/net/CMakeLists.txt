if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)
 pkg_check_modules (PKG_CONFIG_MODULE_GMODULE REQUIRED gmodule-export-2.0)
elseif (WIN32)
endif ()

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

if (UNIX)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG/config\"")
elseif (WIN32)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")
endif ()

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL/include
                      $ENV{LIB_ROOT}/SDL_mixer/include
                      $ENV{LIB_ROOT}/SDL_ttf/include)

# include_directories ($ENV{LIB_ROOT}/gtk2/include/gtk-2.0)
 include_directories (${GTK_INCLUDE_DIRS})
endif (WIN32)

if (GTKGLAREA_SUPPORT)
 include_directories ($ENV{LIB_ROOT}/gtkglarea)
endif (GTKGLAREA_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
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

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries (${EXECUTABLE}
                       Opengl32.lib legacy_stdio_definitions.lib
                       ${GTK_LIBRARIES}
                       $ENV{LIB_ROOT}/SDL/lib/x86/SDL.lib
                       $ENV{LIB_ROOT}/SDL/lib/x86/SDLmain.lib
                       $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                       ACEStream ACEStream_Miscellaneous ACEStream_Network
                       ACENetwork ACENetwork_Client
                       RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE net_server)
set (${EXECUTABLE}_SRCS
     net_callbacks.cpp
     net_callbacks.h
     net_defines.h
     net_server.cpp
     net_server_common.h
     net_server_signalhandler.cpp
     net_server_signalhandler.h
     net_eventhandler.cpp
     net_eventhandler.h)

if (UNIX)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG/config\"")
elseif (WIN32)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")
endif ()

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries (${EXECUTABLE}
                       Opengl32.lib legacy_stdio_definitions.lib
                       ${GTK_LIBRARIES}
                       $ENV{LIB_ROOT}/SDL/lib/x86/SDL.lib
                       $ENV{LIB_ROOT}/SDL/lib/x86/SDLmain.lib
                       $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                       ACEStream ACEStream_Miscellaneous ACEStream_Network
                       ACENetwork ACENetwork_Server
                       RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
