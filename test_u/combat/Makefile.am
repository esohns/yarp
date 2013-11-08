METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../engine \
	-I$(top_srcdir)/../graphics -I$(top_srcdir)/../map \
	-I$(top_srcdir)/../character/monster -I$(top_srcdir)/../chance/dice \
	-I$(top_srcdir)/../character -I$(top_srcdir)/../magic -I$(top_srcdir)/../item \
	-I$(top_srcdir)/../combat -I$(top_srcdir)/../character/player

bin_PROGRAMS = combat_simulator

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_CPPFLAGS = $(AM_CPPFLAGS) -DDATADIR="$(datadir)/@META_PACKAGE@/config"
#combat_simulator_LDFLAGS = $(pkg_libs_LIBS)
combat_simulator_LDADD = -lRPG_Engine -lRPG_Graphics -lRPG_Sound \
  -lRPG_Map -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character -lRPG_Item \
  -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
