
include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)

include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

########### next target ###############
set(client_gui_SRCS rpg_client_gui.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${client_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(client_gui ${client_gui_SRCS})

target_link_libraries(client_gui ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES} ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES} ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES} ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES})

target_link_libraries(client_gui ACE pthread xerces-c SDL SDL_mixer RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Sound RPG_Engine RPG_Client_GUI)

install(TARGETS client_gui RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#bin_PROGRAMS = client_gui
#
#client_gui_SOURCES = rpg_client_gui.cpp
#client_gui_CPPFLAGS	= $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
#client_gui_LDFLAGS = $(pkg_libs_LIBS)
#client_gui_LDADD = -lpthread -lACE -lxerces-c -lSDL_mixer -lRPG_Chance -lRPG_Dice \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat \
#	-lRPG_Map -lRPG_Graphics -lRPG_Sound -lRPG_Engine -lRPG_Client_GUI
