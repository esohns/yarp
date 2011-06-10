METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

include_HEADERS = rpg_client_callbacks.h rpg_client_common.h rpg_client_defines.h rpg_client_window_level.h rpg_client_window_main.h

lib_LTLIBRARIES = libRPG_Client_GUI.la

libRPG_Client_GUI_la_SOURCES = rpg_client_callbacks.cpp rpg_client_callbacks.h \
	rpg_client_common.h rpg_client_defines.h rpg_client_window_level.cpp \
	rpg_client_window_level.h rpg_client_window_main.cpp rpg_client_window_main.h
libRPG_Client_GUI_la_CPPFLAGS	= $(pkg_libs_CFLAGS)

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = rpg_client.glade rpg_client.ini

EXTRA_DIST = $(config_DATA)
