########### next target ###############
set(client_gui_SRCS rpg_client_gui.cpp)

add_executable(client_gui ${client_gui_SRCS})

target_link_libraries(client_gui ACE pthread xerces-c SDL SDL_mixer RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Sound RPG_Engine RPG_Client_GUI)

install(TARGETS client_gui RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#bin_PROGRAMS = client_gui
#
#client_gui_SOURCES = rpg_client_gui.cpp
#client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
#client_gui_LDFLAGS = $(pkg_libs_LIBS)
#client_gui_LDADD = -lpthread -lACE -lxerces-c -lSDL_mixer -lRPG_Chance -lRPG_Dice \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat \
#	-lRPG_Map -lRPG_Graphics -lRPG_Sound -lRPG_Engine -lRPG_Client_GUI
