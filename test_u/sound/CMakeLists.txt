if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_XERCES REQUIRED xerces-c)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
elseif (WIN32)
endif ()

if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_XERCES_INCLUDE_DIRS}
                      ${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                      ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/xerces-c/src
                      $ENV{LIB_ROOT}/SDL/include
                      $ENV{LIB_ROOT}/SDL_mixer/include)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../sound)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(sound_parser_SRCS sound_parser.cpp)

set_source_files_properties (${sound_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS BASEDIR="C:\\")
set_source_files_properties (${sound_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG")

add_executable(sound_parser ${sound_parser_SRCS})

target_link_libraries(sound_parser ${PKG_CONFIG_MODULE_XERCES_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES})

target_link_libraries(sound_parser pthread ACE RPG_Dice RPG_Common RPG_Sound)

########### install files ###############
install(TARGETS sound_parser RUNTIME DESTINATION bin)
