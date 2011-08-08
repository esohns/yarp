
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

target_link_libraries(combat_simulator pthread ACE xerces-c RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

########### install files ###############
install(TARGETS combat_simulator RUNTIME DESTINATION bin)
