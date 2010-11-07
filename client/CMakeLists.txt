#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)
pkg_check_modules(PKG_CONFIG_MODULE_GMODULE REQUIRED gmodule-export-2.0)

include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})

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

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Client_GUI_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

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
