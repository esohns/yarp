cmake_minimum_required(VERSION 2.8)

if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
elseif (WIN32)
endif ()

if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})
elseif (WIN32)
 include_directories($ENV{LIB_ROOT}/SDL/include
                     $ENV{LIB_ROOT}/SDL_mixer/include
                     $ENV{LIB_ROOT}/SDL_ttf/include
                     $ENV{LIB_ROOT}/gtk2/include)
endif ()

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
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(graphics_parser_SRCS graphics_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${graphics_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${graphics_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(graphics_parser ${graphics_parser_SRCS})

target_link_libraries(graphics_parser
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries(graphics_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Graphics)

install(TARGETS graphics_parser RUNTIME DESTINATION bin)

########### next target ###############
set(SDL_gui_SRCS
    SDL_gui.cpp
    SDL_gui_common.h
    SDL_gui_defines.h
    SDL_gui_levelwindow_3d.cpp
    SDL_gui_levelwindow_3d.h
    SDL_gui_levelwindow_isometric.cpp
    SDL_gui_levelwindow_isometric.h
    SDL_gui_mainwindow.cpp
    SDL_gui_mainwindow.h
    SDL_gui_minimapwindow.cpp
    SDL_gui_minimapwindow.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${SDL_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${SDL_gui_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG")

add_executable(SDL_gui ${SDL_gui_SRCS})

add_dependencies(SDL_gui
                 RPG_Dice
                 RPG_Chance
                 RPG_Common
                 RPG_Magic
                 RPG_Item
                 RPG_Character
                 RPG_Monster
                 RPG_Player
                 RPG_Combat
                 RPG_Map
                 RPG_Engine
                 RPG_Sound
                 RPG_Graphics
                 RPG_Client)

target_link_libraries(SDL_gui
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES})
target_link_libraries(SDL_gui
                      pthread
                      ACE
                      xerces-c
                      RPG_Dice
                      RPG_Chance
                      RPG_Common
                      RPG_Magic
                      RPG_Item
                      RPG_Character
                      RPG_Monster
                      RPG_Player
                      RPG_Combat
                      RPG_Map
                      RPG_Engine
                      RPG_Sound
                      RPG_Graphics
                      RPG_Client)

install(TARGETS SDL_gui RUNTIME DESTINATION bin)

