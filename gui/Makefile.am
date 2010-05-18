METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = RPG_client_gui

RPG_client_gui_SOURCES = rpg_client_common.h rpg_client_gui.cpp
RPG_client_gui_LDFLAGS = --export-dynamic -L/usr/local/lib
RPG_client_gui_LDADD = -lpthread -lACE -lgtk-x11-2.0 -lgnome-2 -lgnomeui-2 \
	-lglade-2.0 -lSDL -lRPG_Common

bindata_DATA = rpg_client.glade rpg_client.ini
bindatadir = $(bindir)
