
if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
elseif (WIN32)
endif ()

if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories($ENV{LIB_ROOT}/SDL/include
                     $ENV{LIB_ROOT}/SDL_ttf/include
                     $ENV{LIB_ROOT}/gtk2/include)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

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
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(combat_simulator_SRCS combat_simulator.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${combat_simulator_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${combat_simulator_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(combat_simulator ${combat_simulator_SRCS})

target_link_libraries(combat_simulator ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                                       ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries(combat_simulator pthread ACE xerces-c RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item
                                               RPG_Character RPG_Monster RPG_Player RPG_Combat RPG_Map
                                               RPG_Graphics RPG_Engine)

########### install files ###############
install(TARGETS combat_simulator RUNTIME DESTINATION bin)
