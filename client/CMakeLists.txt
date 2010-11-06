#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Client_GUI_LIB_SRCS
    rpg_client_callbacks.cpp
    rpg_client_callbacks.h
    rpg_client_common.h
    rpg_client_defines.h
    rpg_client_window_level.cpp
    rpg_client_window_level.h
    rpg_client_window_main.cpp
    rpg_client_window_main.h)

add_library(RPG_Client_GUI SHARED ${RPG_Client_GUI_LIB_SRCS})

set_target_properties(RPG_Client_GUI PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Client_GUI LIBRARY DESTINATION lib)

########### install files ###############
install(FILES  rpg_client.glade rpg_client.ini DESTINATION share/RPG/config)

install(FILES  rpg_client_callbacks.h rpg_client_common.h rpg_client_defines.h rpg_client_window_level.h rpg_client_window_main.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#include_HEADERS = rpg_client_callbacks.h rpg_client_common.h rpg_client_defines.h rpg_client_window_level.h rpg_client_window_main.h
#
#lib_LTLIBRARIES = libRPG_Client_GUI.la
#
#libRPG_Client_GUI_la_SOURCES = rpg_client_callbacks.cpp rpg_client_callbacks.h \
#	rpg_client_common.h rpg_client_defines.h rpg_client_window_level.cpp \
#	rpg_client_window_level.h rpg_client_window_main.cpp rpg_client_window_main.h
#libRPG_Client_GUI_la_CPPFLAGS	= $(pkg_libs_CFLAGS)
#
#configdir = $(datadir)/@META_PACKAGE@/config
#config_DATA = rpg_client.glade rpg_client.ini
#
#EXTRA_DIST = $(config_DATA)
