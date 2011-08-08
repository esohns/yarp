
include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)

include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})

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
target_link_libraries(graphics_parser
                      pthread
                      ACE
                      xerces-c
                      RPG_Dice
                      RPG_Common
                      RPG_Graphics)

install(TARGETS graphics_parser RUNTIME DESTINATION bin)

########### next target ###############
set(SDL_gui_SRCS
    SDL_gui.cpp
    SDL_gui_defines.h
    SDL_gui_levelwindow.cpp
    SDL_gui_levelwindow.h
    SDL_gui_minimapwindow.cpp
    SDL_gui_minimapwindow.h
    SDL_gui_mainwindow.cpp
    SDL_gui_mainwindow.h)

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
                 RPG_Character_Monster
                 RPG_Combat
                 RPG_Map
                 RPG_Graphics
                 RPG_Engine
                 RPG_Client_GUI)

target_link_libraries(SDL_gui
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES})
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
                      RPG_Character_Monster
                      RPG_Combat
                      RPG_Map
                      RPG_Graphics
                      RPG_Engine
                      RPG_Client_GUI)

install(TARGETS SDL_gui RUNTIME DESTINATION bin)
