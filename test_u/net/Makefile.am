METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/../common -I$(top_srcdir)/../net/client \
	-I$(top_srcdir)/../net -I$(top_srcdir)/../net/stream \
	-I$(top_srcdir)/../chance/dice

SUBDIRS = protocol

bin_PROGRAMS = net_client net_server

net_server_SOURCES = net_server.cpp net_server_signalhandler.cpp \
  net_server_signalhandler.h
net_server_CPPFLAGS = $(AM_CPPFLAGS)
#net_server_LDFLAGS = $(pkg_libs_LIBS)
net_server_LDADD = -lRPG_Net -lRPG_Stream -lRPG_Common -lRPG_Dice $(pkg_libs_LIBS)

net_client_SOURCES = net_client.cpp net_client_signalhandler.cpp \
	net_client_signalhandler.h net_client_timeouthandler.cpp \
  net_client_timeouthandler.h net_common.h
net_client_CPPFLAGS = $(AM_CPPFLAGS)
#net_client_LDFLAGS = $(pkg_libs_LIBS)
net_client_LDADD = -lRPG_Net_Client -lRPG_Net -lRPG_Stream -lRPG_Common \
	-lRPG_Dice $(pkg_libs_LIBS)
