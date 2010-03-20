METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

include_HEADERS = rpg_net_connection_manager.h rpg_net_defines.h \
	rpg_net_listener.h rpg_net_remote_comm.h rpg_net_sockethandler.h \
	rpg_net_sockethandler_base.h

lib_LTLIBRARIES = libRPG_Net.la
libRPG_Net_la_SOURCES = rpg_net_connection_manager.cpp \
	rpg_net_connection_manager.h rpg_net_defines.h rpg_net_listener.cpp rpg_net_listener.h \
	rpg_net_remote_comm.h rpg_net_sockethandler.cpp rpg_net_sockethandler.h \
	rpg_net_sockethandler_base.cpp rpg_net_sockethandler_base.h
libRPG_Net_la_LDFLAGS = -L/usr/local/lib
