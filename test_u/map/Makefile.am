METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/player

bin_PROGRAMS = map_generator \
  map_generator_gui \
  map_vision \
  map_vision_ui \
  path_finder

noinst_HEADERS = stdafx.h

map_generator_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  map_generator.cpp
map_generator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#map_generator_LDFLAGS = $(pkg_libs_LIBS)
map_generator_LDADD = \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

map_generator_gui_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  test_u_main.cpp \
  test_u_main.h \
  map_generator_gui_callbacks.cpp \
  map_generator_gui_callbacks.h \
  map_generator_gui_common.h \
  map_generator_gui.cpp
map_generator_gui_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#map_generator_gui_LDFLAGS = $(pkg_libs_LIBS)
map_generator_gui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Sound \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

map_vision_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  map_vision.cpp
map_vision_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#map_vision_LDFLAGS = $(pkg_libs_LIBS)
map_vision_LDADD = \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Player \
  -lRPG_Combat \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

map_vision_ui_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  map_vision_ui.cpp
map_vision_ui_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#map_vision_ui_LDFLAGS = $(pkg_libs_LIBS)
map_vision_ui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Sound \
  -lRPG_Graphics \
  -lRPG_Engine \
  -lRPG_Player \
  -lRPG_Monster \
  -lRPG_Combat \
  -lRPG_Map \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Character \
  -lRPG_Common \
  -lRPG_Chance \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

path_finder_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  path_finder.cpp
path_finder_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
#path_finder_LDFLAGS = $(pkg_libs_LIBS)
path_finder_LDADD = \
  -lRPG_Map \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  map_generator_gui.glade

EXTRA_DIST = $(config_DATA)
