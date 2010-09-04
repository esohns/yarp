METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

#INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = net_client net_server

net_server_SOURCES = net_server.cpp
net_server_LDFLAGS = -L/usr/local/lib
net_server_LDADD = -lpthread -lACE -lStream -lRPG_Dice -lRPG_Common -lRPG_Net

net_client_SOURCES = net_client.cpp net_client_signalhandler.cpp \
	net_client_signalhandler.h net_client_timeouthandler.cpp net_client_timeouthandler.h net_common.h
net_client_LDFLAGS = -L/usr/local/lib
net_client_LDADD = -lpthread -lACE -lStream -lRPG_Dice -lRPG_Common -lRPG_Net

SUBDIRS = protocol
