#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(builddir)/../..

DEFS = \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"

bin_PROGRAMS = \
  IRC_client \
  IRC_client_gui

noinst_HEADERS = stdafx.h

IRC_client_SOURCES = \
  IRC_client.cpp \
  IRC_client_defines.h \
  IRC_client_signalhandler.cpp \
  IRC_client_signalhandler.h \
  IRC_client_tools.cpp \
  IRC_client_tools.h \
  IRC_common.h
IRC_client_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libCommon/src/ui \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client \
  -I$(top_srcdir)/../3rd_party/bison \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../net \
  -I$(top_srcdir)/../net/protocol \
  -I$(top_srcdir)/../client \
  $(pkg_libs_CFLAGS)
IRC_client_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../../modules/libCommon/src/ui/.libs \
  -L$(abs_builddir)/../../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../../chance/dice/.libs \
  -L$(abs_builddir)/../../../common/.libs \
  -L$(abs_builddir)/../../../net/.libs \
  -L$(abs_builddir)/../../../net/protocol/.libs \
  $(pkg_libs_LDFLAGS)
IRC_client_LDADD = \
  -lACE \
  -lCommon \
  -lCommon_UI \
  -lACEStream \
  -lACENetwork \
  -lRPG_Dice \
  -lRPG_Common \
  -lRPG_Net \
  -lRPG_Net_Protocol \
  $(pkg_libs_LIBS)

IRC_client_gui_SOURCES = \
  IRC_client_defines.h \
  IRC_client_gui.cpp \
  IRC_client_gui_callbacks.cpp \
  IRC_client_gui_callbacks.h \
  IRC_client_gui_common.h \
  IRC_client_gui_connection.cpp \
  IRC_client_gui_connection.h \
  IRC_client_gui_defines.h \
  IRC_client_gui_messagehandler.cpp \
  IRC_client_gui_messagehandler.h \
  IRC_client_tools.cpp \
  IRC_client_tools.h \
  IRC_common.h
IRC_client_gui_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libCommon/src/ui \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client \
  -I$(top_srcdir)/../3rd_party/bison \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../net \
  -I$(top_srcdir)/../net/protocol \
  -I$(top_srcdir)/../client \
  $(pkg_libs_CFLAGS)
IRC_client_gui_LDFLAGS = \
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
  -L$(abs_builddir)/../../../net/protocol/.libs \
  -L$(abs_builddir)/../../../engine/.libs \
  -L$(abs_builddir)/../../../sound/.libs \
  -L$(abs_builddir)/../../../graphics/.libs \
  -L$(abs_builddir)/../../../client/.libs \
  $(pkg_libs_LDFLAGS)
IRC_client_gui_LDADD = \
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
  -lRPG_Net_Protocol \
  -lRPG_Engine \
  -lRPG_Sound \
  -lRPG_Graphics \
  -lRPG_Client_GUI \
  $(pkg_libs_LIBS) \
  -lSDL_mixer

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  IRC_client.ini \
  IRC_client_channel_tab.glade \
  IRC_client_main.glade \
  IRC_client_server_page.glade \
  servers.ini

EXTRA_DIST = $(config_DATA)

