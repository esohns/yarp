METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

#INCLUDES = -I$(top_srcdir)/.

bin_PROGRAMS = IRC_client IRC_client_gui

IRC_client_SOURCES = IRC_client.cpp IRC_client_defines.h \
	IRC_client_signalhandler.cpp IRC_client_signalhandler.h IRC_common.h
IRC_client_LDFLAGS = -L/usr/local/lib
IRC_client_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
	-lRPG_Net -lRPG_Net_Protocol

IRC_client_gui_SOURCES = IRC_client_defines.h IRC_client_gui.cpp \
	IRC_client_gui_common.h IRC_client_gui_connection.cpp IRC_client_gui_connection.h \
	IRC_client_gui_defines.h IRC_client_gui_messagehandler.cpp IRC_client_gui_messagehandler.h \
	IRC_common.h
IRC_client_gui_CPPFLAGS = `pkg-config --cflags gtk+-2.0`
IRC_client_gui_LDFLAGS = -L/usr/local/lib `pkg-config --libs gtk+-2.0` \
	`pkg-config --libs gthread-2.0`
IRC_client_gui_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
	-lRPG_Net -lRPG_Net_Protocol

bindata_DATA = IRC_client.ini IRC_client_channel_tab.glade \
	IRC_client_main.glade IRC_client_server_page.glade servers.ini
bindatadir = $(bindir)
