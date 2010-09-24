METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = RPG_client_gui

RPG_client_gui_SOURCES = rpg_client_common.h rpg_client_defines.h \
	rpg_client_gui.cpp rpg_client_window_level.cpp rpg_client_window_level.h \
	rpg_client_window_main.cpp rpg_client_window_main.h
RPG_client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS)
RPG_client_gui_LDFLAGS = -export-dynamic $(pkg_libs_LIBS)
RPG_client_gui_LDADD = -lpthread -lACE -lSDL_ttf -lRPG_Graphics -lSDL_mixer \
	-lRPG_Sound -lRPG_Chance -lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character \
	-lRPG_Combat -lRPG_Monster -lRPG_Engine -lRPG_Map

bindata_DATA = rpg_client.glade rpg_client.ini
bindatadir = $(bindir)
