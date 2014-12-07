#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(builddir)/../.. \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../client \
  -I$(srcdir)/../../engine \
  -I$(srcdir)/../../graphics \
  -I$(srcdir)/../../map \
  -I$(srcdir)/../../character/monster \
  -I$(srcdir)/../../chance/dice \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../../character/player \
  -I$(srcdir)/../../net/client \
  -I$(srcdir)/../../sound

bin_PROGRAMS = \
  map_generator \
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
  $(builddir)/../../engine/libRPG_Engine.la \
  $(builddir)/../../net/client/libRPG_Net_Client.la \
  $(builddir)/../../net/libRPG_Net.la \
  $(builddir)/../../net/stream/libRPG_Stream.la \
  $(builddir)/../../graphics/libRPG_Graphics.la \
  $(builddir)/../../map/libRPG_Map.la \
  $(builddir)/../../character/monster/libRPG_Monster.la \
  $(builddir)/../../character/player/libRPG_Player.la \
  $(builddir)/../../combat/libRPG_Combat.la \
  $(builddir)/../../magic/libRPG_Magic.la \
  $(builddir)/../../character/libRPG_Character.la \
  $(builddir)/../../item/libRPG_Item.la \
  $(builddir)/../../chance/libRPG_Chance.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
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
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\" \
  -DDEBUG_DEBUGGER
#map_generator_gui_LDFLAGS = $(pkg_libs_LIBS)
map_generator_gui_LDADD = \
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
  $(builddir)/../../graphics/libRPG_Graphics.la \
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
  $(builddir)/../../map/libRPG_Map.la \
  $(builddir)/../../common/libRPG_Common.la \
  $(builddir)/../../chance/dice/libRPG_Dice.la \
  $(pkg_libs_LIBS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  map_generator_gui.glade

EXTRA_DIST = \
  $(config_DATA)
