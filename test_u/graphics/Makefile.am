METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/../.. \
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
#SDL_gui_LDFLAGS = $(RPG_CPPLDFLAGS)
SDL_gui_LDADD = \
  $(builddir)/../../client/libRPG_Client_GUI.la \
  $(builddir)/../../graphics/libRPG_Graphics.la \
  $(builddir)/../../sound/libRPG_Sound.la \
  $(builddir)/../../engine/libRPG_Engine.la \
  $(builddir)/../../net/stream/libRPG_Stream.la \
  $(builddir)/../../net/libRPG_Net.la \
  $(builddir)/../../net/client/libRPG_Net_Client.la \
  $(builddir)/../../map/libRPG_Map.la \
  $(builddir)/../../character/monster/libRPG_Monster.la \
  $(builddir)/../../character/player/libRPG_Player.la \
  $(builddir)/../../combat/libRPG_Combat.la \
  $(builddir)/../../character/libRPG_Character.la \
  $(builddir)/../../item/libRPG_Item.la \
  $(builddir)/../../magic/libRPG_Magic.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/libRPG_Chance.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS) \
  -lSDL_mixer \
  -lGLU \
  -lGL
