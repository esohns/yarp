METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = graphics_parser SDL_gui

graphics_parser_SOURCES = graphics_parser.cpp
graphics_parser_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
graphics_parser_LDFLAGS = $(pkg_libs_LIBS)
graphics_parser_LDADD = -lpthread -lACE -lRPG_Dice \
	-lRPG_Common -lRPG_Graphics

SDL_gui_SOURCES = SDL_gui.cpp SDL_gui_defines.h SDL_gui_levelwindow.cpp \
	SDL_gui_levelwindow.h SDL_gui_mainwindow.cpp SDL_gui_mainwindow.h
SDL_gui_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
SDL_gui_LDFLAGS = $(pkg_libs_LIBS)
SDL_gui_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Chance -lRPG_Common \
	-lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat -lRPG_Map \
	-lRPG_Graphics -lRPG_Engine
