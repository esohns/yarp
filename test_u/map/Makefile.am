METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = map_generator path_finder

map_generator_SOURCES = map_generator.cpp
#map_generator_LDFLAGS = $(pkg_libs_LIBS)
map_generator_LDADD = -lRPG_Engine -lRPG_Graphics -lRPG_Map -lRPG_Monster \
  -lRPG_Player -lRPG_Combat -lRPG_Magic -lRPG_Character -lRPG_Item -lRPG_Chance \
  -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS)

path_finder_SOURCES = path_finder.cpp
#path_finder_LDFLAGS = $(pkg_libs_LIBS)
path_finder_LDADD = -lRPG_Map -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS)
