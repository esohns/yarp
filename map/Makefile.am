METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

include_HEADERS = rpg_map_common.h rpg_map_common_tools.h rpg_map_defines.h

lib_LTLIBRARIES = libRPG_Map.la

libRPG_Map_la_SOURCES = rpg_map_common.h rpg_map_common_tools.cpp \
	rpg_map_common_tools.h rpg_map_defines.h
libRPG_Map_la_LDFLAGS = -L/usr/local/lib
