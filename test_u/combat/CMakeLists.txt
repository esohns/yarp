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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../graphics")

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)

include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})

########### next target ###############
set(combat_simulator_SRCS combat_simulator.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${combat_simulator_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${combat_simulator_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(combat_simulator ${combat_simulator_SRCS})

target_link_libraries(combat_simulator ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES} ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})

target_link_libraries(combat_simulator pthread ACE xerces-c RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Character_Player RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

########### install files ###############
install(TARGETS combat_simulator RUNTIME DESTINATION bin)
