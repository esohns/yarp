########### next target ###############
set(graphics_parser_SRCS graphics_parser.cpp)

add_executable(graphics_parser ${graphics_parser_SRCS})

target_link_libraries(graphics_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Graphics)

install(TARGETS graphics_parser RUNTIME DESTINATION bin)

########### next target ###############
set(SDL_gui_SRCS
    SDL_gui.cpp
    SDL_gui_defines.h
    SDL_gui_levelwindow.cpp
    SDL_gui_levelwindow.h
    SDL_gui_mainwindow.cpp
    SDL_gui_mainwindow.h)

add_executable(SDL_gui ${SDL_gui_SRCS})

target_link_libraries(SDL_gui pthread ACE xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

install(TARGETS SDL_gui RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = graphics_parser SDL_gui
#
#graphics_parser_SOURCES = graphics_parser.cpp
#graphics_parser_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#graphics_parser_LDFLAGS = $(pkg_libs_LIBS)
#graphics_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice \
#	-lRPG_Common -lRPG_Graphics
#
#SDL_gui_SOURCES = SDL_gui.cpp SDL_gui_defines.h SDL_gui_levelwindow.cpp \
#	SDL_gui_levelwindow.h SDL_gui_mainwindow.cpp SDL_gui_mainwindow.h
#SDL_gui_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
#SDL_gui_LDFLAGS = $(pkg_libs_LIBS)
#SDL_gui_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Chance -lRPG_Common \
#	-lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat -lRPG_Map \
#	-lRPG_Graphics -lRPG_Engine
