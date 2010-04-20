METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = IRC_client

IRC_client_SOURCES = IRC_client.cpp IRC_client_signalhandler.cpp \
	IRC_client_signalhandler.h IRC_common.h
IRC_client_LDFLAGS = -L/usr/local/lib
IRC_client_LDADD = -lpthread -lfl -lACE -lStream -lRPG_Common -lRPG_Net \
	-lRPG_Net_Protocol
