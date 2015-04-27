#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(builddir)/..

bin_PROGRAMS = \
  chance_dice \
  chance_rangeToRoll

noinst_HEADERS = stdafx.h

chance_dice_SOURCES = chance_dice.cpp
#chance_dice_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS)
chance_dice_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../chance/dice/.libs
chance_dice_LDADD = \
  -lACE \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

chance_rangeToRoll_SOURCES = chance_rangeToRoll.cpp
chance_rangeToRoll_LDFLAGS = \
  -L$(abs_top_srcdir)/../../ATCD/ACE/build/linux/lib \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../chance/dice/.libs
chance_rangeToRoll_LDADD = \
  -lACE \
  -lCommon \
  -lRPG_Dice \
  $(pkg_libs_LIBS)

