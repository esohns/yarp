INCLUDES = 
METASOURCES = AUTO
graphics_parser_LDFLAGS = -L/usr/local/lib
bin_PROGRAMS = SDL_gui graphics_parser
graphics_parser_SOURCES = graphics_parser.cpp
graphics_parser_LDADD = -lpthread -lACE -lpng -lSDL -lSDL_ttf -lRPG_Dice \
	-lRPG_Common -lRPG_Graphics
SDL_gui_LDFLAGS = -L/usr/local/lib
SDL_gui_SOURCES = SDL_gui.cpp SDL_gui_defines.h SDL_gui_levelwindow.cpp \
	SDL_gui_levelwindow.h SDL_gui_mainwindow.cpp SDL_gui_mainwindow.h
SDL_gui_LDADD = -lpthread -lACE -lpng -lSDL -lSDL_ttf -lRPG_Common -lRPG_Dice \
	-lRPG_Graphics -lRPG_Map
