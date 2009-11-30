METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/dice

include_HEADERS = rpg_chance_dice.h rpg_chance_dice_XML_parser.h \
	rpg_chance_dice_XML_parser_base.h rpg_chance_dice_XML_types.h rpg_chance_dice_common.h \
	rpg_chance_dice_common_tools.h

lib_LTLIBRARIES = libRPG_Chance_Dice.la

libRPG_Chance_Dice_la_LDFLAGS = -L/usr/local/lib
libRPG_Chance_Dice_la_SOURCES = rpg_XMLSchema.h rpg_chance_dice.cpp \
	rpg_chance_dice.h rpg_chance_dice_XML_parser.cpp rpg_chance_dice_XML_parser.h \
	rpg_chance_dice_XML_parser_base.cpp rpg_chance_dice_XML_parser_base.h rpg_chance_dice_XML_types.cpp \
	rpg_chance_dice_XML_types.h rpg_chance_dice_common.h rpg_chance_dice_common_tools.cpp \
	rpg_chance_dice_common_tools.h

bin_PROGRAMS = chance_dice chance_rangeToRoll

# the library search path / libraries
chance_dice_LDADD = $(top_builddir)/dice/libRPG_Chance_Dice.la -lACE -lpthread \
	-lxerces-c
chance_dice_SOURCES = main.cpp
chance_dice_LDFLAGS = -L/usr/local/lib

chance_rangeToRoll_LDADD = $(top_builddir)/dice/libRPG_Chance_Dice.la -lACE -lpthread \
	-lxerces-c
chance_rangeToRoll_SOURCES = rangeToRoll.cpp
chance_rangeToRoll_LDFLAGS = -L/usr/local/lib

noinst_HEADERS = rpg_XMLSchema.h

bindata_DATA = rpg_chance_dice.xsd
bindatadir = $(bindir)
