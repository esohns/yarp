METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(srcdir)/dice \
  -I$(top_srcdir)/../common

SUBDIRS = dice . scripts

include_HEADERS = rpg_chance_common_tools.h

lib_LTLIBRARIES = libRPG_Chance.la

libRPG_Chance_la_SOURCES = rpg_chance_common_tools.cpp \
	rpg_chance_common_tools.h
#libRPG_Chance_la_CPPFLAGS = -I$(top_srcdir)/dice
