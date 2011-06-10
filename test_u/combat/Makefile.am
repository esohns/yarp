METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = combat_simulator

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
combat_simulator_LDFLAGS = $(pkg_libs_LIBS)
combat_simulator_LDADD = -lpthread -lACE -lxerces-c -lRPG_Chance -lRPG_Dice \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat \
	-lRPG_Map -lRPG_Graphics -lRPG_Engine
