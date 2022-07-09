include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/PreUIFramework.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetUIFramework.cmake)

if (CURSES_USE)
elseif (GTK_USE)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetGTKVersion.cmake)

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

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

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

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
include_directories (${GTK_INCLUDE_DIRS})
if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL_mixer/include)
endif ()
#if (GTKGLAREA_SUPPORT)
# include_directories ($ENV{LIB_ROOT}/gtkglarea)
#endif (GTKGLAREA_SUPPORT)

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/include)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
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
                        Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol)
elseif (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib legacy_stdio_definitions.lib
                        ${GTK_LIBRARIES}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Client
                        RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol)
endif ()

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
                        Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol
                        ACEStream_Network
                        RPG_Net)
elseif (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib legacy_stdio_definitions.lib
                        ${GTK_LIBRARIES}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Miscellaneous ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Dice RPG_Common RPG_Net RPG_Net_Protocol)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
