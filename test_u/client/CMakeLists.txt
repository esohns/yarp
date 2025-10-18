#include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/PreUIFramework.cmake)
#include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetUIFramework.cmake)

#if (CURSES_USE)
#elseif (GTK_USE)
# include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetGTKVersion.cmake)

# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  add_definitions (-DG_ERRORCHECK_MUTEXES)
# endif (CMAKE_BUILD_TYPE STREQUAL Debug)
#elseif (WXWIDGETS_USE)
# message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
# message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
# message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
# message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
## # *NOTE*: re-enable ANSI mode, iff available
## add_definitions (-DwxUSE_UNICODE=0)
# foreach (DEFINITION ${wxWidgets_DEFINITIONS})
#  add_definitions (-D${DEFINITION})
# endforeach ()
# if (WIN32)
##  add_definitions (-D__WXMSW__)
##  add_definitions (-DWXUSINGDLL)
#  if (MSVC)
#   add_definitions (-DwxMSVC_VERSION_AUTO)
#  endif (MSVC)
# endif (WIN32)
# add_definitions (-DwxUSE_GUI=1)
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
##   add_definitions (-D__WXDEBUG__)
# endif (CMAKE_BUILD_TYPE STREQUAL Debug)
#endif ()

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_XERCES_C REQUIRED xerces-c)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/libpng/include)
endif ()
if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
if (GTK_SUPPORT)
 include_directories (${GTK_INCLUDE_DIRS}
                      $ENV{LIB_ROOT}/gtkglarea)
endif (GTK_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/event)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/task)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/3rd_party/tree)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/misc)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/stat)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/client_server)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../combat
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/player
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../map
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../client
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../sound
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../net/protocol)

########### next target ###############
set (EXECUTABLE client_gui)
set (${EXECUTABLE}_SRCS
     rpg_client_gui.cpp
     rpg_client_gui_callbacks.cpp
     rpg_client_gui_callbacks.h
     stdafx.h)

if (UNIX)
 if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  set (CMAKE_EXE_LINKER_FLAGS "-Wl,-z,multidefs")
  set (CMAKE_STATIC_LINKER_FLAGS "-Wl,-z,multidefs")
 endif (CMAKE_COMPILER_IS_GNUCC)
endif (UNIX)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Chance RPG_Dice RPG_Common
                                RPG_Magic RPG_Item RPG_Character RPG_Combat 
                                RPG_Player RPG_Monster
                                RPG_Map RPG_Graphics RPG_Sound RPG_Engine
                                RPG_Client)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        ${PKG_CONFIG_MODULE_XERCES_C_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_GTHREAD_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                        X11
                        ${SDL_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonError CommonEvent CommonLog CommonTimer CommonUI CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Dice RPG_Common RPG_Sound
                        RPG_Engine RPG_Player RPG_Monster RPG_Player RPG_Item RPG_Map RPG_Common
                        RPG_Character RPG_Combat RPG_Magic RPG_Chance RPG_Dice
                        RPG_Net RPG_Net_Protocol RPG_Net RPG_Client RPG_Graphics RPG_Net_Protocol
                        ACEStream_Network)
elseif (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib
                        ${SDL_LIBRARIES}
                        ${GTK_LIBRARIES}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonError CommonEvent CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork ACENetwork_Server
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Sound
                        RPG_Engine RPG_Net RPG_Net_Protocol RPG_Client)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

########### install files ###############
install (TARGETS ${EXECUTABLE}
#         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)

########### launcher ###############
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -n -t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY}
                         DIRECTORY)
# set (ACE_DLL_FILE ACE${LIB_FILE_SUFFIX}.dll)
 set (ACE_DLL_FILE ACE.dll)
 set (ACE_DLL_FQPATH ${ACE_LIB_PATH}/${ACE_DLL_FILE})
 set (ACED_DLL_FILE ACEd.dll)
 set (ACED_DLL_FQPATH ${ACE_LIB_PATH}/${ACED_DLL_FILE})
 set (ACE_SSL_DLL_FILE ACE_SSLd.dll)
 set (ACE_SSL_DLL_FQPATH ${ACE_LIB_PATH}/${ACE_SSL_DLL_FILE})
 file (TO_NATIVE_PATH $ENV{LIB_ROOT} LIB_ROOT)
 string (REPLACE "\\" "\\\\" LIB_ROOT ${LIB_ROOT})
 install (FILES ${ACE_DLL_FQPATH} ${ACED_DLL_FQPATH}
                ${ACE_SSL_DLL_FQPATH}
                ${LIB_ROOT}/gtk3/bin/libatk-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libfreetype-6.dll
                ${LIB_ROOT}/gtk3/bin/libintl-8.dll
                ${LIB_ROOT}/gtk3/bin/libcairo-2.dll
                ${LIB_ROOT}/gtk3/bin/libcairo-gobject-2.dll
                ${LIB_ROOT}/gtk3/bin/libfontconfig-1.dll
                ${LIB_ROOT}/gtk3/bin/libgio-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libglib-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgmodule-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgobject-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgthread-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgdk_pixbuf-2.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libgdk-3-0.dll
                ${LIB_ROOT}/gtk3/bin/libgtk-3-0.dll
                ${LIB_ROOT}/gtk3/bin/libpango-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangocairo-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangoft2-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libpangowin32-1.0-0.dll
                ${LIB_ROOT}/gtk3/bin/libiconv-2.dll
                ${LIB_ROOT}/gtk3/bin/libpng15-15.dll
                ${LIB_ROOT}/gtk3/bin/liblzma-5.dll
                ${LIB_ROOT}/gtk3/bin/zlib1.dll
                ${LIB_ROOT}/gtk3/bin/libxml2-2.dll
                ${LIB_ROOT}/gtk3/bin/libffi-6.dll
                ${LIB_ROOT}/gtk3/bin/libwinpthread-1.dll
                ${LIB_ROOT}/gtk3/bin/libpixman-1-0.dll
                ${LIB_ROOT}/SDL2_mixer/lib/x64/SDL2_mixer.dll
                ${LIB_ROOT}/SDL2_ttf/lib/x64/SDL2_ttf.dll
                ${LIB_ROOT}/SDL2/lib/x64/SDL2.dll
                ${LIB_ROOT}/xerces-c/build/msvc/src/${CMAKE_BUILD_TYPE}/xerces-c_3_2D.dll
                ${LIB_ROOT}/libepoxy/bin/epoxy-0.dll
                ${LIB_ROOT}/libpng/build/${CMAKE_BUILD_TYPE}/libpng16d.dll
                ${LIB_ROOT}/openssl/libcrypto-3-x64.dll
                ${LIB_ROOT}/openssl/libssl-3-x64.dll
                ${LIB_ROOT}/zlib/build/${CMAKE_BUILD_TYPE}/zlibd.dll
          DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)
 install (FILES
          ${LIB_ROOT}/gtk3/share/glib-2.0/schemas/gschemas.compiled
          ${LIB_ROOT}/gtk3/share/glib-2.0/schemas/org.gtk.Settings.ColorChooser.gschema.xml
          ${LIB_ROOT}/gtk3/share/glib-2.0/schemas/org.gtk.Settings.FileChooser.gschema.xml
          DESTINATION ${INSTALL_SHARE_DIR}/glib-2.0/schemas COMPONENT config)

 file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 file (TO_CMAKE_PATH "${SDL_LIB_DIR}" SDL_LIB_PATH)
 file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -n -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${LIB_ROOT}/libepoxy/bin
                                              ${LIB_ROOT}/openssl
                                              ${SDL_LIB_PATH}
                                              ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                                              ${GTK_LIB_PATH}
#                                              ${PROJECT_SOURCE_DIR}/../Common/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/../Common/cmake/src/err/Debug
#                                              ${PROJECT_SOURCE_DIR}/../Common/cmake/src/ui/Debug
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/vis/Debug
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
endif ()
