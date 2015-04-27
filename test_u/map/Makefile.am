#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../..

DEFS = \
  -DHAVE_CONFIG_H \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\" \
  -DDEBUG_DEBUGGER

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
  -I$(top_srcdir)/../modules/libCommon/src \
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
  -I$(top_srcdir)/../graphics \
  $(pkg_libs_CFLAGS)
map_generator_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
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
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
map_generator_LDADD = \
  -lACE \
  -lCommon \
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
  -lRPG_Graphics \
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
  -I$(top_srcdir)/../net/client \
  -I$(top_srcdir)/../engine \
  -I$(top_srcdir)/../sound \
  -I$(top_srcdir)/../graphics \
  -I$(top_srcdir)/../client \
  $(pkg_libs_CFLAGS)
map_generator_gui_LDFLAGS = \
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
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../sound/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  -L$(abs_builddir)/../../client/.libs \
  $(pkg_libs_LDFLAGS)
map_generator_gui_LDADD = \
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
  $(pkg_libs_LIBS)

map_vision_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  map_vision.cpp
map_vision_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
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
  -I$(top_srcdir)/../graphics \
  $(pkg_libs_CFLAGS)
map_vision_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
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
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
map_vision_LDADD = \
  -lACE \
  -lCommon \
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
  -lRPG_Graphics \
  $(pkg_libs_LIBS)

map_vision_ui_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  map_vision_ui.cpp
map_vision_ui_CPPFLAGS = \
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
  -I$(top_srcdir)/../net/client \
  -I$(top_srcdir)/../engine \
  -I$(top_srcdir)/../sound \
  -I$(top_srcdir)/../graphics \
  -I$(top_srcdir)/../client \
  $(pkg_libs_CFLAGS)
map_vision_ui_LDFLAGS = \
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
  -L$(abs_builddir)/../../engine/.libs \
  -L$(abs_builddir)/../../sound/.libs \
  -L$(abs_builddir)/../../graphics/.libs \
  -L$(abs_builddir)/../../client/.libs \
  $(pkg_libs_LDFLAGS)
map_vision_ui_LDADD = \
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
  $(pkg_libs_LIBS)

path_finder_SOURCES = \
  stdafx.cpp \
  stdafx.h \
  path_finder.cpp
path_finder_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../engine \
  $(pkg_libs_CFLAGS)
path_finder_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs \
  -L$(abs_builddir)/../../common/.libs \
  -L$(abs_builddir)/../../map/.libs \
  $(pkg_libs_LDFLAGS)
path_finder_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Common \
  -lRPG_Map \
  $(pkg_libs_LIBS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  map_generator_gui.glade

EXTRA_DIST = \
  $(config_DATA)

