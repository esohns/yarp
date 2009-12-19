METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/dice

include_HEADERS = rpg_dice.h rpg_dice_XML_parser.h rpg_dice_XML_parser_base.h \
	rpg_dice_XML_types.h rpg_dice_common.h rpg_dice_common_tools.h rpg_dice_dietype.h \
	rpg_dice_incl.h rpg_dice_roll.h rpg_dice_valuerange.h

lib_LTLIBRARIES = libRPG_Dice.la

libRPG_Dice_la_LDFLAGS = -L/usr/local/lib
libRPG_Dice_la_SOURCES = rpg_XMLSchema.h rpg_dice.cpp \
	rpg_dice.h rpg_dice_XML_parser.cpp rpg_dice_XML_parser.h \
	rpg_dice_XML_parser_base.cpp rpg_dice_XML_parser_base.h rpg_dice_XML_types.cpp \
	rpg_dice_XML_types.h rpg_dice_common_tools.cpp rpg_dice_common_tools.h \
	rpg_dice_roll.h rpg_dice_dietype.h rpg_dice_valuerange.h

noinst_HEADERS = rpg_XMLSchema.h

bindata_DATA = rpg_dice.xsd
bindatadir = $(bindir)
