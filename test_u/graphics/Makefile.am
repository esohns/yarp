METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../graphics \
	-I$(top_srcdir)/../map -I$(top_srcdir)/../chance/dice \
	-I$(top_srcdir)/../engine -I$(top_srcdir)/../net/client \
	-I$(top_srcdir)/../character/monster -I$(top_srcdir)/../character \
	-I$(top_srcdir)/../magic -I$(top_srcdir)/../item -I$(top_srcdir)/../combat \
	-I$(top_srcdir)/../character/player -I$(top_srcdir)/../client \
	-I$(top_srcdir)/../sound

bin_PROGRAMS = graphics_parser SDL_gui

graphics_parser_SOURCES = graphics_parser.cpp
graphics_parser_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#graphics_parser_LDFLAGS = $(pkg_libs_LIBS)
graphics_parser_LDADD = -lRPG_Graphics -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS)

SDL_gui_SOURCES = SDL_gui.cpp SDL_gui_defines.h SDL_gui_levelwindow.cpp \
	SDL_gui_levelwindow.h SDL_gui_mainwindow.cpp SDL_gui_mainwindow.h \
  SDL_gui_minimapwindow.h SDL_gui_minimapwindow.cpp
SDL_gui_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#SDL_gui_LDFLAGS = $(pkg_libs_LIBS)
SDL_gui_LDADD = -lRPG_Client_GUI -lRPG_Engine -lRPG_Graphics -lRPG_Sound \
  -lRPG_Map -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character -lRPG_Item \
  -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS) -lSDL_mixer
