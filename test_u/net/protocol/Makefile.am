METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = IRC_client IRC_client_gui

IRC_client_SOURCES = IRC_client.cpp IRC_client_signalhandler.cpp \
	IRC_client_signalhandler.h IRC_common.h
IRC_client_LDFLAGS = -L/usr/local/lib
IRC_client_LDADD = -lpthread -lfl -lACE -lStream -lRPG_Common -lRPG_Net \
	-lRPG_Net_Protocol

IRC_client_gui_SOURCES = IRC_client_gui.cpp IRC_client_gui_messagehandler.cpp \
	IRC_client_gui_messagehandler.h IRC_common.h
IRC_client_gui_LDFLAGS = -L/usr/local/lib
IRC_client_gui_LDADD = -lpthread -lACE -lRPG_Common -lStream -lRPG_Net \
	-lRPG_Net_Protocol -lgtk-x11-2.0

bindata_DATA = IRC_client.glade IRC_client.ini
bindatadir = $(bindir)
