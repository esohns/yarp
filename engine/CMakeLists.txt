#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############

set(RPG_Engine_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_engine_XML_tree.cpp
    rpg_engine_XML_tree.h
    rpg_engine_common.h
    rpg_engine_common_tools.cpp
    rpg_engine_common_tools.h
    rpg_engine_defines.h
    rpg_engine_level.cpp
    rpg_engine_level.h)

add_library(RPG_Engine SHARED ${RPG_Engine_LIB_SRCS})

set_target_properties(RPG_Engine PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Engine LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_engine.xsd DESTINATION share/RPG/config)

install(FILES  rpg_engine_XML_tree.h rpg_engine_common.h  	rpg_engine_common_tools.h rpg_engine_defines.h rpg_engine_level.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#include_HEADERS = rpg_engine_XML_tree.h rpg_engine_common.h \
#	rpg_engine_common_tools.h rpg_engine_defines.h rpg_engine_level.h
#
#lib_LTLIBRARIES = libRPG_Engine.la
#
#libRPG_Engine_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_engine_XML_tree.cpp \
#	rpg_engine_XML_tree.h rpg_engine_common.h rpg_engine_common_tools.cpp \
#	rpg_engine_common_tools.h rpg_engine_defines.h rpg_engine_level.cpp rpg_engine_level.h
#
#pkgdatadir = $(datadir)/@META_PACKAGE@
#configdir = $(pkgdatadir)/config
#config_DATA = rpg_engine.xsd
#
#EXTRA_DIST = $(config_DATA)
#noinst_HEADERS = rpg_XMLSchema_XML_tree.h
