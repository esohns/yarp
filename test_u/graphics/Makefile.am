METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../net/client \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/player \
  -I$(srcdir)/../../client \
  -I$(srcdir)/../../sound

bin_PROGRAMS = graphics_parser SDL_gui

noinst_HEADERS = stdafx.h

graphics_parser_SOURCES = graphics_parser.cpp
graphics_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#graphics_parser_LDFLAGS = $(pkg_libs_LIBS)
graphics_parser_LDADD = \
  -lRPG_Graphics \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

SDL_gui_SOURCES = \
  SDL_gui.cpp \
  SDL_gui_common.h \
  SDL_gui_defines.h \
  SDL_gui_levelwindow_isometric.cpp \
  SDL_gui_levelwindow_isometric.h \
  SDL_gui_levelwindow_3d.cpp \
  SDL_gui_levelwindow_3d.h \
  SDL_gui_mainwindow.cpp \
  SDL_gui_mainwindow.h \
  SDL_gui_minimapwindow.h \
  SDL_gui_minimapwindow.cpp
SDL_gui_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#SDL_gui_LDFLAGS = $(pkg_libs_LIBS)
SDL_gui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Sound \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

