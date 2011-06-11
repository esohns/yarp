cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dice)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(dice)

########### next target ###############

set(RPG_Chance_LIB_SRCS rpg_chance_common_tools.cpp rpg_chance_common_tools.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Chance_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Chance SHARED ${RPG_Chance_LIB_SRCS})

set_target_properties(RPG_Chance PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS RPG_Chance LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_chance_common_tools.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = dice
#
#include_HEADERS = rpg_chance_common_tools.h
#
#lib_LTLIBRARIES = libRPG_Chance.la
#
#libRPG_Chance_la_SOURCES = rpg_chance_common_tools.cpp \
#	rpg_chance_common_tools.h
#libRPG_Chance_la_CPPFLAGS = -I$(top_srcdir)/dice
