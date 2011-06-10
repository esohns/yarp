METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = chance_dice chance_rangeToRoll

chance_dice_SOURCES = chance_dice.cpp
chance_dice_CPPFLAGS = $(pkg_libs_CFLAGS)
chance_dice_LDFLAGS = $(pkg_libs_LIBS)
chance_dice_LDADD = -lpthread -lxerces-c -lRPG_Dice

chance_rangeToRoll_SOURCES = chance_rangeToRoll.cpp
chance_rangeToRoll_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice
