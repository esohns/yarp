if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_XERCES_C REQUIRED xerces-c)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
#pkg_check_modules(PKG_CONFIG_MODULE_SDL_GFX REQUIRED SDL_gfx)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
 pkg_check_modules(PKG_CONFIG_MODULE_GTHREAD REQUIRED gthread-2.0)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBGTK REQUIRED gtk+-2.0)
elseif(WIN32)
endif ()

include_directories (${GTK_INCLUDE_DIRS}
                     $ENV{LIB_ROOT}/gtkglarea)
if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_XERCES_C_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
#                    ${PKG_CONFIG_MODULE_SDL_GFX_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_GTHREAD_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})
elseif (WIN32)
 include_directories($ENV{LIB_ROOT}/xerces-c/src
                     $ENV{LIB_ROOT}/SDL/include
                     $ENV{LIB_ROOT}/SDL_mixer/include
                     $ENV{LIB_ROOT}/SDL_ttf/include)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
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

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set (EXECUTABLE client_gui)
set (${EXECUTABLE}_SRCS
     rpg_client_gui.cpp
     rpg_client_gui_callbacks.cpp
     rpg_client_gui_callbacks.h)

if (UNIX)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"/usr/local/share/RPG\";RPG_GRAPHICS_HAS_DLL=1")
elseif (WIN32)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"\"")
endif ()

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies(${EXECUTABLE} RPG_Chance RPG_Dice RPG_Common
                               RPG_Magic RPG_Item RPG_Character RPG_Combat 
                               RPG_Player RPG_Monster
                               RPG_Map RPG_Graphics RPG_Sound RPG_Engine
                               RPG_Client)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        ${PKG_CONFIG_MODULE_XERCES_C_LIBRARIES}
                        ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
#                      ${PKG_CONFIG_MODULE_SDL_GFX_LIBRARIES}
                        ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                        ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        ${PKG_CONFIG_MODULE_GTHREAD_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                        X11
                        ACE
                        Common CommonLog CommonTimer CommonUI CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Dice RPG_Common RPG_Sound
                        RPG_Engine RPG_Player RPG_Monster RPG_Player RPG_Item RPG_Map RPG_Common
                        RPG_Character RPG_Combat RPG_Magic RPG_Chance RPG_Dice
                        RPG_Net RPG_Net_Protocol RPG_Net RPG_Client RPG_Graphics
                        ACEStream_Network)
elseif (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Opengl32.lib
                        $ENV{LIB_ROOT}/SDL/lib/x86/SDLmain.lib
                        $ENV{LIB_ROOT}/SDL_mixer/lib/x86/SDL_mixer.lib
#                      SDL_gfx
                        ${GTK_LIBRARIES}
                        $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonLog CommonSignal CommonTimer CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item RPG_Player RPG_Monster
                        RPG_Combat RPG_Map RPG_Graphics RPG_Sound
                        RPG_Engine RPG_Client RPG_Net RPG_Net_Protocol)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### launcher ###############
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
create_target_launcher (client_gui
                        ARGS "-t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/cmake/Debug
                                             ${LIB_ROOT}/libpng/cmake/Debug
                                             ${LIB_ROOT}/openssl
                                             ${LIB_ROOT}/gtk2/bin
                                             ${LIB_ROOT}/SDL/lib/x86
                                             ${LIB_ROOT}/SDL_mixer/lib/x86
                                             ${LIB_ROOT}/SDL_ttf/lib/x86
                                             ${LIB_ROOT}/xerces-c/build/src/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${PROJECT_SOURCE_DIR}/../Common/cmake/src/Debug
                                             ${PROJECT_SOURCE_DIR}/../Common/cmake/src/err/Debug
                                             ${PROJECT_SOURCE_DIR}/../Common/cmake/src/ui/Debug
                                             ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                                             ${PROJECT_SOURCE_DIR}/cmake/src/modules/vis/Debug
                        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../..
                        ENVIRONMENT foo=bar)
