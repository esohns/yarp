METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
#  -I$(srcdir)/../include
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../modules/libCommon/src/ui \
  -I$(top_srcdir)/../modules/libACEStream/src \
  -I$(top_srcdir)/../modules/libACENetwork/src \
  -I$(top_srcdir)/../modules/libACENetwork/src/client \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../character \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item \
  -I$(top_srcdir)/../character/player \
  -I$(top_srcdir)/../combat \
  -I$(top_srcdir)/../character/monster \
  -I$(top_srcdir)/../map \
  -I$(top_srcdir)/../net/client \
  -I$(top_srcdir)/../engine \
  -I$(top_srcdir)/../sound \
  -I$(top_srcdir)/../graphics \
  -I$(builddir)/.. \
  -DRPG_CLIENT_BUILD_DLL

SUBDIRS = scripts

noinst_HEADERS = stdafx.h
include_HEADERS = \
  rpg_client_callbacks.h \
  rpg_client_common.h \
  rpg_client_common_tools.h \
  rpg_client_defines.h \
  rpg_client_engine.h \
  rpg_client_entity_manager.h \
  rpg_client_exports.h \
  rpg_client_graphicsmode.h \
  rpg_client_incl.h \
  rpg_client_iwidget_ui.h \
  rpg_client_iwindow.h \
  rpg_client_iwindow_level.h \
  rpg_client_logger.h \
  rpg_client_runtimestate.h \
  rpg_client_ui_tools.h \
  rpg_client_window_level.h \
  rpg_client_window_main.h \
  rpg_client_window_message.h \
  rpg_client_window_minimap.h \
  rpg_client_XML_tree.h

lib_LTLIBRARIES = libRPG_Client_GUI.la
libRPG_Client_GUI_la_SOURCES = \
  rpg_client_callbacks.cpp \
  rpg_client_callbacks.h \
  rpg_client_common.h \
  rpg_client_common_tools.cpp \
  rpg_client_common_tools.h \
  rpg_client_defines.h \
  rpg_client_engine.cpp \
  rpg_client_engine.h \
  rpg_client_entity_manager.cpp \
  rpg_client_entity_manager.h \
  rpg_client_exports.h \
  rpg_client_graphicsmode.h \
  rpg_client_incl.h \
  rpg_client_iwidget_ui.h \
  rpg_client_iwindow.h \
  rpg_client_iwindow_level.h \
  rpg_client_logger.h \
  rpg_client_logger.cpp \
  rpg_client_runtimestate.h \
  rpg_client_ui_tools.cpp \
  rpg_client_ui_tools.h \
  rpg_client_window_level.cpp \
  rpg_client_window_level.h \
  rpg_client_window_main.cpp \
  rpg_client_window_main.h \
  rpg_client_window_message.cpp \
  rpg_client_window_message.h \
  rpg_client_window_minimap.cpp \
  rpg_client_window_minimap.h \
  rpg_client_XML_tree.cpp \
  rpg_client_XML_tree.h \
  stdafx.cpp \
  stdafx.h

libRPG_Client_GUI_la_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\" \
  $(pkg_libs_CFLAGS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = \
  rpg_client.glade \
  rpg_client.ini

EXTRA_DIST = $(config_DATA)
