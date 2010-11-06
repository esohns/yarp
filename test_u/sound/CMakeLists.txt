########### next target ###############
set(sound_parser_SRCS sound_parser.cpp)

add_executable(sound_parser ${sound_parser_SRCS})

target_link_libraries(sound_parser pthread ACE SDL_mixer RPG_Dice RPG_Common RPG_Sound)

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
