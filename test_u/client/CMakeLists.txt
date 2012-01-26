#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../item")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../character")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../magic")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../combat")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../character/player")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../map")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../engine")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../client")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../graphics")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../sound")

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_XERCES_C REQUIRED xerces-c)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
#pkg_check_modules(PKG_CONFIG_MODULE_SDL_GFX REQUIRED SDL_gfx)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)

include_directories(${PKG_CONFIG_MODULE_XERCES_C_INCLUDE_DIRS}
		    ${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
#                    ${PKG_CONFIG_MODULE_SDL_GFX_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

########### next target ###############
set(client_gui_SRCS rpg_client_gui.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${client_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${client_gui_SRCS} PROPERTIES COMPILE_DEFINITIONS "DATADIR=\"/usr/local/share/RPG\";RPG_GRAPHICS_HAS_DLL=1")

add_executable(client_gui ${client_gui_SRCS})

add_dependencies(client_gui RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Player RPG_Character_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Sound RPG_Engine RPG_Client_GUI)

target_link_libraries(client_gui
                      ${PKG_CONFIG_MODULE_XERCES_C_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
#                      ${PKG_CONFIG_MODULE_SDL_GFX_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES})

target_link_libraries(client_gui
                      ACE
                      pthread
                      SDL_gfx
                      RPG_Chance
                      RPG_Dice
                      RPG_Common
                      RPG_Magic
                      RPG_Item
                      RPG_Character
                      RPG_Character_Player
                      RPG_Character_Monster
                      RPG_Combat
                      RPG_Map
                      RPG_Graphics
                      RPG_Sound
                      RPG_Engine
                      RPG_Client_GUI)

install(TARGETS client_gui RUNTIME DESTINATION bin)
