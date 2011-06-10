METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = net_client net_server

net_server_SOURCES = net_server.cpp
net_server_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
	-lRPG_Net

net_client_SOURCES = net_client.cpp net_client_signalhandler.cpp \
	net_client_signalhandler.h net_client_timeouthandler.cpp net_client_timeouthandler.h net_common.h
net_client_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
	-lRPG_Net

SUBDIRS = protocol
