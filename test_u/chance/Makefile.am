METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../chance/dice

bin_PROGRAMS = chance_dice chance_rangeToRoll

chance_dice_SOURCES = chance_dice.cpp
chance_dice_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice

chance_rangeToRoll_SOURCES = chance_rangeToRoll.cpp
chance_rangeToRoll_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice
