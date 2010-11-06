#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(data)

########### next target ###############
set(RPG_Sound_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_sound.h
    rpg_sound_XML_parser.cpp
    rpg_sound_XML_parser.h
    rpg_sound_XML_types.cpp
    rpg_sound_XML_types.h
    rpg_sound_category.h
    rpg_sound_common.h
    rpg_sound_common_tools.cpp
    rpg_sound_common_tools.h
    rpg_sound_defines.h
    rpg_sound_dictionary.cpp
    rpg_sound_dictionary.h
    rpg_sound_event.h
    rpg_sound_incl.h)

add_library(RPG_Sound SHARED ${RPG_Sound_LIB_SRCS})

set_target_properties(RPG_Sound PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Sound LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_sound.xml rpg_sound.xsd DESTINATION share/RPG/config)

install(FILES  rpg_sound.h rpg_sound_XML_parser.h rpg_sound_XML_types.h  	rpg_sound_category.h rpg_sound_common.h rpg_sound_common_tools.h rpg_sound_defines.h  	rpg_sound_dictionary.h rpg_sound_event.h rpg_sound_incl.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#include_HEADERS = rpg_sound.h rpg_sound_XML_parser.h rpg_sound_XML_types.h \
#	rpg_sound_category.h rpg_sound_common.h rpg_sound_common_tools.h rpg_sound_defines.h \
#	rpg_sound_dictionary.h rpg_sound_event.h rpg_sound_incl.h
#
#lib_LTLIBRARIES = libRPG_Sound.la
#
#libRPG_Sound_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_sound.h \
#	rpg_sound_XML_parser.cpp rpg_sound_XML_parser.h rpg_sound_XML_types.cpp rpg_sound_XML_types.h \
#	rpg_sound_category.h rpg_sound_common.h rpg_sound_common_tools.cpp rpg_sound_common_tools.h \
#	rpg_sound_defines.h rpg_sound_dictionary.cpp rpg_sound_dictionary.h rpg_sound_event.h \
#	rpg_sound_incl.h
#libRPG_Sound_la_CPPFLAGS = $(pkg_libs_CFLAGS)
#
#pkgdatadir = $(datadir)/@META_PACKAGE@
#configdir = $(pkgdatadir)/config
#config_DATA = rpg_sound.xml rpg_sound.xsd
#
#EXTRA_DIST = $(config_DATA)
#
#SUBDIRS = data
