METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(srcdir)/../common -I$(srcdir)/dice

SUBDIRS = dice . scripts

include_HEADERS = rpg_chance_common_tools.h

lib_LTLIBRARIES = libRPG_Chance.la

libRPG_Chance_la_SOURCES = rpg_chance_common_tools.cpp \
	rpg_chance_common_tools.h
#libRPG_Chance_la_CPPFLAGS = -I$(top_srcdir)/dice
