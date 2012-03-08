cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)
pkg_check_modules(PKG_CONFIG_MODULE_GMODULE REQUIRED gmodule-export-2.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../character/player
                    ${CMAKE_CURRENT_SOURCE_DIR}/../character/monster
                    ${CMAKE_CURRENT_SOURCE_DIR}/../map
                    ${CMAKE_CURRENT_SOURCE_DIR}/../engine
                    ${CMAKE_CURRENT_SOURCE_DIR}/../graphics)
include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Client_LIB_SRCS
    rpg_client_callbacks.cpp
    rpg_client_callbacks.h
    rpg_client_common.h
    rpg_client_defines.h
    rpg_client_exports.h
    rpg_client_window_level.cpp
    rpg_client_window_level.h
    rpg_client_window_main.cpp
    rpg_client_window_main.h
    rpg_client_window_minimap.h
    rpg_client_window_minimap.cpp
    rpg_client_engine.h
    rpg_client_engine.cpp
    rpg_client_common_tools.h
    rpg_client_common_tools.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Client_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Client_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS "RPG_CLIENT_BUILD_DLL;RPG_GRAPHICS_HAS_DLL=1")

add_library(RPG_Client SHARED ${RPG_Client_LIB_SRCS})

add_dependencies(RPG_Client RPG_Graphics RPG_Engine RPG_Map)

set_target_properties(RPG_Client PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES
        rpg_client.glade
        rpg_client.ini
        DESTINATION share/RPG/config)
install(FILES
        rpg_client_callbacks.h
        rpg_client_common.h
        rpg_client_defines.h
        rpg_client_exports.h
        rpg_client_window_level.h
        rpg_client_window_main.h
        rpg_client_window_minimap.h
        rpg_client_engine.h
        rpg_client_common_tools.h
        DESTINATION include)
install(TARGETS RPG_Client LIBRARY DESTINATION lib)
