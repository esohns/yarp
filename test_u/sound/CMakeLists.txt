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
set (EXECUTABLE sound_parser)
set (${EXECUTABLE}_SRCS
     sound_parser.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries(${EXECUTABLE} ${PKG_CONFIG_MODULE_XERCES_LIBRARIES}
                                    ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                                    ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES})

target_link_libraries (${EXECUTABLE}
                       $ENV{LIB_ROOT}/SDL/lib/x86/SDL.lib
                       $ENV{LIB_ROOT}/SDL_mixer/lib/x86/SDL_mixer.lib
                       $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       Common CommonLog CommonTimer
                       RPG_Dice RPG_Common RPG_Sound)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
