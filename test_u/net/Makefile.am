INCLUDES = 
METASOURCES = AUTO
net_server_LDFLAGS = -L/usr/local/lib
bin_PROGRAMS = net_client net_server
net_server_SOURCES = net_server.cpp
net_client_LDFLAGS = -L/usr/local/lib
net_client_SOURCES = net_client.cpp net_client_timeouthandler.cpp \
	net_client_timeouthandler.h
net_client_LDADD = -lACE -lpthread -lRPG_Common -lRPG_Net
net_server_LDADD = -lpthread -lACE -lRPG_Common -lRPG_Net
