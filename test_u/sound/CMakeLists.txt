
include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_XERCES REQUIRED xerces-c)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)

include_directories(${PKG_CONFIG_MODULE_XERCES_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS})

########### next target ###############
set(sound_parser_SRCS sound_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${sound_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${sound_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG")

add_executable(sound_parser ${sound_parser_SRCS})

target_link_libraries(sound_parser ${PKG_CONFIG_MODULE_XERCES_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES})

target_link_libraries(sound_parser pthread ACE RPG_Dice RPG_Common RPG_Sound)

install(TARGETS sound_parser RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = sound_parser
#
#sound_parser_SOURCES = sound_parser.cpp
#sound_parser_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
#sound_parser_LDFLAGS = $(pkg_libs_LIBS)
#sound_parser_LDADD = -lpthread -lACE -lSDL_mixer -lRPG_Dice -lRPG_Common \
#	-lRPG_Sound
