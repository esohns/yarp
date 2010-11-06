cmake_minimum_required(VERSION 2.8)

project(Yarp)

add_subdirectory(chance)
add_subdirectory(common)
add_subdirectory(magic)
add_subdirectory(item)
add_subdirectory(character)
add_subdirectory(monster)
add_subdirectory(combat)
add_subdirectory(map)
add_subdirectory(graphics)
add_subdirectory(sound)
add_subdirectory(engine)
add_subdirectory(stream)
add_subdirectory(net)
add_subdirectory(client)
add_subdirectory(test_u)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = chance common magic item character monster combat map graphics sound engine stream net client test_u
#
#EXTRA_DIST = Yarp.spec
