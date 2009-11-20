METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/dice

include_HEADERS = rpg_chance_dice.h rpg_chance_dice_common.h \
	rpg_chance_dice_common_tools.h

lib_LTLIBRARIES = libRPG_Chance_Dice.la

libRPG_Chance_Dice_la_LDFLAGS = -L/usr/local/lib
libRPG_Chance_Dice_la_SOURCES = rpg_chance_dice.cpp rpg_chance_dice.h \
	rpg_chance_dice_common.h rpg_chance_dice_common_tools.cpp rpg_chance_dice_common_tools.h

noinst_PROGRAMS = dice rangeToRoll

# the library search path / libraries
dice_LDADD = $(top_builddir)/dice/libRPG_Chance_Dice.la -lACE -lpthread
dice_SOURCES = main.cpp
dice_LDFLAGS = -L/usr/local/lib
rangeToRoll_SOURCES = rangeToRoll.cpp
rangeToRoll_LDADD = $(top_builddir)/dice/libRPG_Chance_Dice.la
