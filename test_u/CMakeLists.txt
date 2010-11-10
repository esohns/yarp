#cmake_minimum_required(VERSION 2.8)

#project(test_u CXX)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/rpg_config.h)

add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(chance)
add_subdirectory(character)
add_subdirectory(combat)
add_subdirectory(graphics)
add_subdirectory(client)
add_subdirectory(item)
add_subdirectory(magic)
add_subdirectory(map)
add_subdirectory(monster)
add_subdirectory(net)
add_subdirectory(sound)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = chance character combat graphics client item magic map monster net sound
