METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dice

SUBDIRS = dice

include_HEADERS = rpg_chance_common_tools.h

lib_LTLIBRARIES = libRPG_Chance.la

libRPG_Chance_la_SOURCES = rpg_chance_common_tools.cpp \
	rpg_chance_common_tools.h
libRPG_Chance_la_LDFLAGS = -L/usr/local/lib
libRPG_Chance_la_LIBADD = $(top_builddir)/dice/libRPG_Dice.la

