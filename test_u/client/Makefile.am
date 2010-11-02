METASOURCES = AUTO

bin_PROGRAMS = client_gui

client_gui_SOURCES = rpg_client_gui.cpp
client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
client_gui_LDFLAGS = $(pkg_libs_LIBS)
client_gui_LDADD = -lpthread -lACE -lxerces-c -lSDL_mixer -lRPG_Chance -lRPG_Dice \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat \
	-lRPG_Map -lRPG_Graphics -lRPG_Sound -lRPG_Engine -lRPG_Client_GUI
