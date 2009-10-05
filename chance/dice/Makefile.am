METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/dice

include_HEADERS = rpg_chance_dice_common.h rpg_chance_dice.h

lib_LTLIBRARIES = libRPG_Chance_Dice.la

libRPG_Chance_Dice_la_LDFLAGS = -L/usr/local/lib
libRPG_Chance_Dice_la_LIBADD = -lACE -lpthread
libRPG_Chance_Dice_la_SOURCES = rpg_chance_dice.cpp rpg_chance_dice.h \
	rpg_chance_dice_common.h

bin_PROGRAMS = dice

# the library search path / libraries
dice_LDADD = $(top_builddir)/dice/libRPG_Chance_Dice.la
dice_SOURCES = main.cpp
