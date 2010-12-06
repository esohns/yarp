
########### next target ###############

set(RPG_Dice_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_dice.cpp
    rpg_dice.h
    rpg_dice_XML_parser.cpp
    rpg_dice_XML_parser.h
    rpg_dice_XML_tree.cpp
    rpg_dice_XML_tree.h
    rpg_dice_XML_types.cpp
    rpg_dice_XML_types.h
    rpg_dice_common.h
    rpg_dice_common_tools.cpp
    rpg_dice_common_tools.h
    rpg_dice_dietype.h
    rpg_dice_roll.h
    rpg_dice_valuerange.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Dice_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Dice SHARED ${RPG_Dice_LIB_SRCS})

set_target_properties(RPG_Dice PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS RPG_Dice LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_dice.xsd DESTINATION share/RPG/config)
install(FILES  rpg_dice.h rpg_dice_XML_parser.h rpg_dice_XML_tree.h  	rpg_dice_XML_types.h rpg_dice_common.h rpg_dice_common_tools.h rpg_dice_dietype.h  	rpg_dice_incl.h rpg_dice_roll.h rpg_dice_valuerange.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#noinst_HEADERS = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h
#
#include_HEADERS = rpg_dice.h rpg_dice_XML_parser.h rpg_dice_XML_tree.h \
#	rpg_dice_XML_types.h rpg_dice_common.h rpg_dice_common_tools.h rpg_dice_dietype.h \
#	rpg_dice_incl.h rpg_dice_roll.h rpg_dice_valuerange.h
#
#lib_LTLIBRARIES = libRPG_Dice.la
#
#libRPG_Dice_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h \
#	rpg_dice.cpp rpg_dice.h rpg_dice_XML_parser.cpp rpg_dice_XML_parser.h \
#	rpg_dice_XML_tree.cpp rpg_dice_XML_tree.h rpg_dice_XML_types.cpp rpg_dice_XML_types.h \
#	rpg_dice_common.h rpg_dice_common_tools.cpp rpg_dice_common_tools.h rpg_dice_dietype.h \
#	rpg_dice_roll.h rpg_dice_valuerange.h
##libRPG_Dice_la_CPPFLAGS	= -I$(top_srcdir)/dice
#
#pkgdatadir = $(datadir)/@META_PACKAGE@
#configdir = $(pkgdatadir)/config
#config_DATA = rpg_dice.xsd
#
#EXTRA_DIST = $(config_DATA)
