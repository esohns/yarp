#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = \
  graphics_parser \
  SDL_gui

noinst_HEADERS = stdafx.h

graphics_parser_SOURCES = graphics_parser.cpp
graphics_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../graphics \
  $(pkg_libs_CFLAGS)
graphics_parser_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
graphics_parser_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Common \
  -lRPG_Graphics \
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
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libCommon/src/ui \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character \
  -I$(top_srcdir)/../character/player \
  -I$(top_srcdir)/../combat \
  -I$(top_srcdir)/../character/monster \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../engine \
  -I$(top_srcdir)/../sound \
  -I$(top_srcdir)/../graphics \
  -I$(top_srcdir)/../client \
  $(pkg_libs_CFLAGS)
SDL_gui_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../modules/libCommon/src/ui/.libs \
  -L$(abs_builddir)/../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../chance/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../magic/.libs \
  -L$(abs_builddir)/../../item/.libs \
  -L$(abs_builddir)/../../character/.libs \
  -L$(abs_builddir)/../../character/player/.libs \
  -L$(abs_builddir)/../../combat/.libs \
  -L$(abs_builddir)/../../character/monster/.libs \
  -L$(abs_builddir)/../../map/.libs \
  -L$(abs_builddir)/../../net/.libs \
  -L$(abs_builddir)/../../net/protocol/.libs \
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../sound/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  -L$(abs_builddir)/../../client/.libs \
  $(pkg_libs_LDFLAGS)
SDL_gui_LDADD = \
  -lACE \
  -lCommon \
  -lCommon_UI \
  -lACEStream \
  -lACENetwork \
  -lRPG_Dice \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Magic \
  -lRPG_Item \
  -lRPG_Character \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Monster \
  -lRPG_Map \
  -lRPG_Net \
  -lRPG_Engine \
  -lRPG_Sound \
  -lRPG_Graphics \
  -lRPG_Client_GUI \
  $(pkg_libs_LIBS) \
  -lSDL_mixer \
  -lGLU \
  -lGL

