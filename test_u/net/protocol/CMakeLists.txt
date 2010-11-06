########### next target ###############
set(IRC_client_SRCS
    IRC_client.cpp
    IRC_client_defines.h
    IRC_client_signalhandler.cpp
    IRC_client_signalhandler.h
    IRC_client_tools.cpp
    IRC_client_tools.h
    IRC_common.h)

add_executable(IRC_client ${IRC_client_SRCS})

target_link_libraries(IRC_client pthread ACE RPG_Dice RPG_Common RPG_Stream RPG_Net RPG_Net_Protocol)

install(TARGETS IRC_client RUNTIME DESTINATION bin)

########### next target ###############
set(IRC_client_gui_SRCS
    IRC_client_defines.h
    IRC_client_gui.cpp
    IRC_client_gui_callbacks.cpp
    IRC_client_gui_callbacks.h
    IRC_client_gui_common.h
    IRC_client_gui_connection.cpp
    IRC_client_gui_connection.h
    IRC_client_gui_defines.h
    IRC_client_gui_messagehandler.cpp
    IRC_client_gui_messagehandler.h
    IRC_client_tools.cpp
    IRC_client_tools.h
    IRC_common.h)

add_executable(IRC_client_gui ${IRC_client_gui_SRCS})

target_link_libraries(IRC_client_gui pthread ACE RPG_Dice RPG_Common RPG_Stream RPG_Net RPG_Net_Protocol)

install(TARGETS IRC_client_gui RUNTIME DESTINATION bin)

########### install files ###############

install(FILES  IRC_client.ini IRC_client_channel_tab.glade  	IRC_client_main.glade IRC_client_server_page.glade servers.ini DESTINATION share/RPG/config)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = IRC_client IRC_client_gui
#
#IRC_client_SOURCES = IRC_client.cpp IRC_client_defines.h \
#	IRC_client_signalhandler.cpp IRC_client_signalhandler.h IRC_client_tools.cpp IRC_client_tools.h \
#	IRC_common.h
#IRC_client_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#IRC_client_LDFLAGS = $(pkg_libs_LIBS)
#IRC_client_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
#	-lRPG_Net -lRPG_Net_Protocol
#
#IRC_client_gui_SOURCES = IRC_client_defines.h IRC_client_gui.cpp \
#	IRC_client_gui_callbacks.cpp IRC_client_gui_callbacks.h IRC_client_gui_common.h \
#	IRC_client_gui_connection.cpp IRC_client_gui_connection.h IRC_client_gui_defines.h \
#	IRC_client_gui_messagehandler.cpp IRC_client_gui_messagehandler.h IRC_client_tools.cpp IRC_client_tools.h \
#	IRC_common.h
#IRC_client_gui_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#IRC_client_gui_LDFLAGS = $(pkg_libs_LIBS)
#IRC_client_gui_LDADD = -lpthread -lACE -lRPG_Dice -lRPG_Common -lRPG_Stream \
#	-lRPG_Net -lRPG_Net_Protocol
#
#configdir = $(datadir)/@META_PACKAGE@/config
#config_DATA = IRC_client.ini IRC_client_channel_tab.glade \
#	IRC_client_main.glade IRC_client_server_page.glade servers.ini
#
#EXTRA_DIST = $(config_DATA)
