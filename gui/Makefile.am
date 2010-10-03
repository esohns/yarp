METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = RPG_client_gui

RPG_client_gui_SOURCES = rpg_client_common.h rpg_client_defines.h \
	rpg_client_gui.cpp rpg_client_window_level.cpp rpg_client_window_level.h \
	rpg_client_window_main.cpp rpg_client_window_main.h
RPG_client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
RPG_client_gui_LDFLAGS = $(pkg_libs_LIBS)
RPG_client_gui_LDADD = -lRPG_Dice -lACE -lRPG_Chance -lRPG_Character \
	-lRPG_Combat -lRPG_Common -lRPG_Engine -lRPG_Graphics -lRPG_Item -lRPG_Magic \
	-lRPG_Map -lRPG_Monster -lRPG_Sound -lSDL_mixer -lSDL_ttf -lpthread

configdir = $(datadir)/@META_PACKAGE@/config
config_DATA = rpg_client.glade rpg_client.ini

EXTRA_DIST = $(config_DATA)
