METASOURCES = AUTO

include_HEADERS = rpg_combat_common.h rpg_combat_common_tools.h

lib_LTLIBRARIES = libRPG_Combat.la

libRPG_Combat_la_SOURCES = rpg_combat_common.h rpg_combat_common_tools.cpp
libRPG_Combat_la_LDFLAGS = -L/usr/local/lib

bin_PROGRAMS = combat_simulator

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_LDADD = $(top_builddir)/libRPG_Combat.la -lRPG_Character \
	-lRPG_Item -lRPG_Chance_Dice -lACE -lpthread -lxerces-c
combat_simulator_LDFLAGS = -L/usr/local/lib
