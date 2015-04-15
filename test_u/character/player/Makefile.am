#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../../..

DEFS = \
  -DRPG_PLAYER_HAS_DLL \
  -DRPG_NET_CLIENT_HAS_DLL \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = \
  character_generator \
  character_generator_gui \
  character_parser

noinst_HEADERS = stdafx.h

character_generator_SOURCES = \
  character_generator.cpp
character_generator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
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
character_generator_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../../chance/dice/.libs \
  -L$(abs_builddir)/../../../chance/.libs \
  -L$(abs_builddir)/../../../common/.libs \
  -L$(abs_builddir)/../../../magic/.libs \
  -L$(abs_builddir)/../../../item/.libs \
  -L$(abs_builddir)/../../../character/.libs \
  -L$(abs_builddir)/../../../character/player/.libs \
  -L$(abs_builddir)/../../../combat/.libs \
  -L$(abs_builddir)/../../../character/monster/.libs \
  -L$(abs_builddir)/../../../map/.libs \
  -L$(abs_builddir)/../../../net/.libs \
  -L$(abs_builddir)/../../../engine/.libs \
  -L$(abs_builddir)/../../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
character_generator_LDADD = \
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

character_generator_gui_SOURCES = \
  character_generator_gui.cpp \
  character_generator_gui_callbacks.cpp \
  character_generator_gui_callbacks.h \
  character_generator_gui_common.h \
  test_u_main.cpp \
  test_u_main.h
character_generator_gui_CPPFLAGS = \
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
character_generator_gui_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../../modules/libCommon/src/ui/.libs \
  -L$(abs_builddir)/../../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../../chance/dice/.libs \
  -L$(abs_builddir)/../../../chance/.libs \
  -L$(abs_builddir)/../../../common/.libs \
  -L$(abs_builddir)/../../../magic/.libs \
  -L$(abs_builddir)/../../../item/.libs \
  -L$(abs_builddir)/../../../character/.libs \
  -L$(abs_builddir)/../../../character/player/.libs \
  -L$(abs_builddir)/../../../combat/.libs \
  -L$(abs_builddir)/../../../character/monster/.libs \
  -L$(abs_builddir)/../../../map/.libs \
  -L$(abs_builddir)/../../../net/.libs \
  -L$(abs_builddir)/../../../engine/.libs \
  -L$(abs_builddir)/../../../sound/.libs \
  -L$(abs_builddir)/../../../graphics/.libs \
  -L$(abs_builddir)/../../../client/.libs \
  $(pkg_libs_LDFLAGS)
character_generator_gui_LDADD = \
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
  -lSDL_mixer

character_parser_SOURCES = character_parser.cpp
character_parser_CPPFLAGS = \
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
character_parser_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../../chance/dice/.libs \
  -L$(abs_builddir)/../../../chance/.libs \
  -L$(abs_builddir)/../../../common/.libs \
  -L$(abs_builddir)/../../../magic/.libs \
  -L$(abs_builddir)/../../../item/.libs \
  -L$(abs_builddir)/../../../character/.libs \
  -L$(abs_builddir)/../../../character/player/.libs \
  -L$(abs_builddir)/../../../graphics/.libs \
  $(pkg_libs_LDFLAGS)
character_parser_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Magic \
  -lRPG_Item \
  -lRPG_Character \
  -lRPG_Player \
  -lRPG_Graphics \
  $(pkg_libs_LIBS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = character_generator_gui.glade

EXTRA_DIST = $(config_DATA)

