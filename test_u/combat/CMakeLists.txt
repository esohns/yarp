########### next target ###############
set(combat_simulator_SRCS combat_simulator.cpp)

add_executable(combat_simulator ${combat_simulator_SRCS})

target_link_libraries(combat_simulator pthread ACE xerces-c RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

install(TARGETS combat_simulator RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = combat_simulator
#
#combat_simulator_SOURCES = combat_simulator.cpp
#combat_simulator_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#combat_simulator_LDFLAGS = $(pkg_libs_LIBS)
#combat_simulator_LDADD = -lpthread -lACE -lxerces-c -lRPG_Chance -lRPG_Dice \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat \
#	-lRPG_Map -lRPG_Graphics -lRPG_Engine
