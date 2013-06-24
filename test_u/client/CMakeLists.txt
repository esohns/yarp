cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_XERCES_C REQUIRED xerces-c)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
#pkg_check_modules(PKG_CONFIG_MODULE_SDL_GFX REQUIRED SDL_gfx)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_GTHREAD REQUIRED gthread-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)

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
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../sound)
include_directories(${PKG_CONFIG_MODULE_XERCES_C_INCLUDE_DIRS}
		    ${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
#                    ${PKG_CONFIG_MODULE_SDL_GFX_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_GTHREAD_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(client_gui_SRCS rpg_client_gui.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${client_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${client_gui_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"/usr/local/share/RPG\";RPG_GRAPHICS_HAS_DLL=1")

add_executable(client_gui ${client_gui_SRCS})

add_dependencies(client_gui RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Player RPG_Monster
                            RPG_Combat RPG_Map RPG_Graphics RPG_Sound RPG_Engine RPG_Client)

target_link_libraries(client_gui
                      ${PKG_CONFIG_MODULE_XERCES_C_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
#                      ${PKG_CONFIG_MODULE_SDL_GFX_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                      ${PKG_CONFIG_MODULE_GTHREAD_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES})

target_link_libraries(client_gui
                      ACE
                      pthread
#                      SDL_gfx
                      RPG_Chance
                      RPG_Dice
                      RPG_Common
                      RPG_Magic
                      RPG_Item
                      RPG_Character
                      RPG_Player
                      RPG_Monster
                      RPG_Combat
                      RPG_Map
                      RPG_Graphics
                      RPG_Sound
                      RPG_Engine
                      RPG_Client)

install(TARGETS client_gui RUNTIME DESTINATION bin)
