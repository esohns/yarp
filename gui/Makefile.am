METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = RPG_client_gui

RPG_client_gui_SOURCES = rpg_client_common.h rpg_client_defines.h \
	rpg_client_gui.cpp
RPG_client_gui_LDFLAGS = --export-dynamic -L/usr/local/lib
RPG_client_gui_LDADD = -lACE -lRPG_Chance -lRPG_Common -lRPG_Graphics \
	-lRPG_Sound -lRPG_Map -lSDL -lSDL_mixer -lSDL_ttf -lglade-2.0 -lgnome-2 -lgnomeui-2 \
	-lgtk-x11-2.0 -lpthread

bindata_DATA = rpg_client.glade rpg_client.ini
bindatadir = $(bindir)
