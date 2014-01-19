METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_CLIENT_BUILD_DLL -I$(srcdir)/../common \
              -I$(srcdir)/../chance/dice -I$(srcdir)/../character \
              -I$(srcdir)/../magic -I$(srcdir)/../item -I$(srcdir)/../combat \
              -I$(srcdir)/../graphics -I$(srcdir)/../net/client \
              -I$(srcdir)/../engine -I$(srcdir)/../map \
              -I$(srcdir)/../character/monster -I$(srcdir)/../character/player \
              -I$(srcdir)/../sound

noinst_HEADERS = stdafx.h
include_HEADERS = rpg_client_callbacks.h \
  rpg_client_common.h \
  rpg_client_defines.h \
  rpg_client_window_level.h \
  rpg_client_window_main.h \
  rpg_client_exports.h \
  rpg_client_common_tools.h \
  rpg_client_engine.h \
  rpg_client_entity_manager.h \
  rpg_client_GTK_manager.h \
  rpg_client_iwindow.h \
  rpg_client_iwindow_level.h \
  rpg_client_ui_tools.h \
  rpg_client_window_message.h \
  rpg_client_window_minimap.h

lib_LTLIBRARIES = libRPG_Client_GUI.la
libRPG_Client_GUI_la_SOURCES = rpg_client_callbacks.cpp \
  rpg_client_callbacks.h \
  rpg_client_common.h \
  rpg_client_defines.h \
  rpg_client_window_level.cpp \
  rpg_client_window_level.h \
  rpg_client_window_main.cpp \
  rpg_client_window_main.h \
  rpg_client_exports.h \
  rpg_client_common_tools.h \
  rpg_client_common_tools.cpp \
  rpg_client_engine.h \
  rpg_client_engine.cpp \
  rpg_client_entity_manager.h \
  rpg_client_entity_manager.cpp \
  rpg_client_GTK_manager.h \
  rpg_client_GTK_manager.cpp \
  rpg_client_iwindow.h \
  rpg_client_iwindow_level.h \
  rpg_client_ui_tools.h \
  rpg_client_ui_tools.cpp \
  rpg_client_window_message.h \
  rpg_client_window_message.cpp \
  rpg_client_window_minimap.h \
  rpg_client_window_minimap.cpp
libRPG_Client_GUI_la_CPPFLAGS	= $(AM_CPPFLAGS) $(pkg_libs_CFLAGS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = rpg_client.glade rpg_client.ini

EXTRA_DIST = $(config_DATA)
