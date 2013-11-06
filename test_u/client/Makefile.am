METASOURCES = AUTO

bin_PROGRAMS = client_gui

client_gui_SOURCES = rpg_client_gui.cpp
client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS) -DDATADIR="$(datadir)/@META_PACKAGE@"
#client_gui_LDFLAGS = $(pkg_libs_LIBS)
client_gui_LDADD = -lRPG_Client_GUI -lRPG_Graphics -lRPG_Sound -lRPG_Engine \
  -lRPG_Map -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character -lRPG_Item \
  -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS) -lSDL_mixer
