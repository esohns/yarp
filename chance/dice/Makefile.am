METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

noinst_HEADERS = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h

include_HEADERS = rpg_dice.h rpg_dice_XML_parser.h rpg_dice_XML_tree.h \
	rpg_dice_XML_types.h rpg_dice_common.h rpg_dice_common_tools.h rpg_dice_dietype.h \
	rpg_dice_incl.h rpg_dice_roll.h rpg_dice_valuerange.h

lib_LTLIBRARIES = libRPG_Dice.la

libRPG_Dice_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h \
	rpg_dice.cpp rpg_dice.h rpg_dice_XML_parser.cpp rpg_dice_XML_parser.h \
	rpg_dice_XML_tree.cpp rpg_dice_XML_tree.h rpg_dice_XML_types.cpp rpg_dice_XML_types.h \
	rpg_dice_common.h rpg_dice_common_tools.cpp rpg_dice_common_tools.h rpg_dice_dietype.h \
	rpg_dice_roll.h rpg_dice_valuerange.h
#libRPG_Dice_la_CPPFLAGS	= -I$(top_srcdir)/dice

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_dice.xsd

EXTRA_DIST = $(config_DATA)
