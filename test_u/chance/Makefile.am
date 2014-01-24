METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../chance/dice

bin_PROGRAMS = chance_dice chance_rangeToRoll

noinst_HEADERS = stdafx.h

chance_dice_SOURCES = chance_dice.cpp
#chance_dice_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS)
chance_dice_LDADD = \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

chance_rangeToRoll_SOURCES = chance_rangeToRoll.cpp
chance_rangeToRoll_LDADD = \
  -lRPG_Dice \
  $(pkg_libs_LIBS)
