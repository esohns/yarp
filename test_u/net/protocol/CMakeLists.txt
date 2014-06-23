cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_GTHREAD REQUIRED gthread-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../net
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../net/stream
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../net/protocol)
include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_GTHREAD_INCLUDE_DIRS}
                    ${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS})

########### next target ###############
set(IRC_client_SRCS
    IRC_client.cpp
    IRC_client_defines.h
    IRC_client_signalhandler.cpp
    IRC_client_signalhandler.h
    IRC_client_tools.cpp
    IRC_client_tools.h
    IRC_common.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${IRC_client_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${IRC_client_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(IRC_client ${IRC_client_SRCS})

target_link_libraries(IRC_client pthread ACE xerces-c RPG_Dice RPG_Common RPG_Stream RPG_Net RPG_Net_Protocol)

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

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${IRC_client_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${IRC_client_gui_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(IRC_client_gui ${IRC_client_gui_SRCS})

target_link_libraries(IRC_client_gui
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                      ${PKG_CONFIG_MODULE_GTHREAD_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES})
target_link_libraries(IRC_client_gui pthread ACE xerces-c
                                     RPG_Chance RPG_Dice RPG_Common RPG_Stream RPG_Net RPG_Net_Protocol RPG_Character RPG_Magic RPG_Item RPG_Monster RPG_Player RPG_Combat RPG_Graphics RPG_Map RPG_Engine RPG_Sound RPG_Client)

########### install files ###############
install(TARGETS IRC_client_gui RUNTIME DESTINATION bin)
install(FILES
        IRC_client.ini
        IRC_client_channel_tab.glade
        IRC_client_main.glade
        IRC_client_server_page.glade
        servers.ini
        DESTINATION share/RPG/config)
