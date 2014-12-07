#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_NET_CLIENT_HAS_DLL \
  -DRPG_PLAYER_HAS_DLL \
  -I$(builddir)/../../.. \
  -I$(srcdir)/../../../common \
  -I$(srcdir)/../../../chance/dice \
  -I$(srcdir)/../../../magic \
  -I$(srcdir)/../../../item \
  -I$(srcdir)/../../../character \
  -I$(srcdir)/../../../combat \
  -I$(srcdir)/../../../character/player \
  -I$(srcdir)/../../../character/monster \
  -I$(srcdir)/../../../map \
  -I$(srcdir)/../../../graphics \
  -I$(srcdir)/../../../sound \
  -I$(srcdir)/../../../net/client \
  -I$(srcdir)/../../../engine \
  -I$(srcdir)/../../../client

bin_PROGRAMS = \
  character_generator \
  character_generator_gui \
  character_parser

noinst_HEADERS = stdafx.h

character_generator_SOURCES = character_generator.cpp
character_generator_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
character_generator_LDFLAGS = $(pkg_libs_LDFLAGS)
character_generator_LDADD = \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Combat \
  -lRPG_Player \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

character_parser_SOURCES = character_parser.cpp
character_parser_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
character_parser_LDFLAGS = $(pkg_libs_LDFLAGS)
character_parser_LDADD = \
  -lRPG_Graphics \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
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
  $(pkg_libs_CFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
character_generator_gui_LDFLAGS = $(pkg_libs_LDFLAGS)
character_generator_gui_LDADD = \
  -lRPG_Client_GUI \
  -lRPG_Engine \
  -lRPG_Net_Client \
  -lRPG_Net \
  -lRPG_Stream \
  -lRPG_Graphics \
  -lRPG_Sound \
  -lRPG_Map \
  -lRPG_Monster \
  -lRPG_Combat \
  -lRPG_Player \
  -lRPG_Character \
  -lRPG_Item \
  -lRPG_Magic \
  -lRPG_Chance \
  -lRPG_Common \
  -lRPG_Dice \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = character_generator_gui.glade

EXTRA_DIST = $(config_DATA)

