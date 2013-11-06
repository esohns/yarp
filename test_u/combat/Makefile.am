METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = combat_simulator

combat_simulator_SOURCES = combat_simulator.cpp
combat_simulator_CPPFLAGS = -DDATADIR="$(datadir)/@META_PACKAGE@/config"
#combat_simulator_LDFLAGS = $(pkg_libs_LIBS)
combat_simulator_LDADD = -lRPG_Engine -lRPG_Graphics -lRPG_Sound \
  -lRPG_Map -lRPG_Monster -lRPG_Player -lRPG_Combat -lRPG_Character -lRPG_Item \
  -lRPG_Magic -lRPG_Common -lRPG_Chance -lRPG_Dice $(pkg_libs_LIBS)
