########### next target ###############
set(map_generator_SRCS map_generator.cpp)

add_executable(map_generator ${map_generator_SRCS})

target_link_libraries(map_generator pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

install(TARGETS map_generator RUNTIME DESTINATION bin)

########### next target ###############
set(path_finder_SRCS path_finder.cpp)

add_executable(path_finder ${path_finder_SRCS})

target_link_libraries(path_finder pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

install(TARGETS path_finder RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = map_generator path_finder
#
#map_generator_SOURCES = map_generator.cpp
#map_generator_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common -lRPG_Map
#
#path_finder_SOURCES = path_finder.cpp
#path_finder_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common -lRPG_Map
