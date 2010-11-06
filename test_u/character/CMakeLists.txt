########### next target ###############
set(character_generator_SRCS character_generator.cpp)

add_executable(character_generator ${character_generator_SRCS})

target_link_libraries(character_generator ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Combat RPG_Monster RPG_Map RPG_Graphics RPG_Engine)

install(TARGETS character_generator RUNTIME DESTINATION bin)

########### next target ###############
set(character_generator_gui_SRCS
    character_generator_gui.cpp
    character_generator_gui_callbacks.cpp
    character_generator_gui_callbacks.h
    character_generator_gui_common.h)

add_executable(character_generator_gui ${character_generator_gui_SRCS})

target_link_libraries(character_generator_gui ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Combat RPG_Monster RPG_Map RPG_Graphics RPG_Engine RPG_Client_GUI)

install(TARGETS character_generator_gui RUNTIME DESTINATION bin)

########### next target ###############
set(character_parser_SRCS character_parser.cpp)

add_executable(character_parser ${character_parser_SRCS})

target_link_libraries(character_parser ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Combat RPG_Monster)

install(TARGETS character_parser RUNTIME DESTINATION bin)

########### install files ###############
install(FILES  character_generator_gui.glade DESTINATION share/RPG/config)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = character_generator character_generator_gui character_parser
#
#character_generator_SOURCES = character_generator.cpp
#character_generator_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#character_generator_LDFLAGS = $(pkg_libs_LIBS)
#character_generator_LDADD = -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster -lRPG_Map -lRPG_Graphics -lRPG_Engine
#
#character_parser_SOURCES = character_parser.cpp
#character_parser_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#character_parser_LDFLAGS = $(pkg_libs_LIBS)
#character_parser_LDADD = -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster
#
#character_generator_gui_SOURCES = character_generator_gui.cpp \
#	character_generator_gui_callbacks.cpp character_generator_gui_callbacks.h character_generator_gui_common.h
#character_generator_gui_CPPFLAGS = $(pkg_libs_CFLAGS) -DDATADIR="\"$(datadir)/@META_PACKAGE@\""
#character_generator_gui_LDFLAGS = $(pkg_libs_LIBS)
#character_generator_gui_LDADD = -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster -lRPG_Map -lRPG_Graphics -lRPG_Engine	-lRPG_Client_GUI
#
#configdir = $(datadir)/@META_PACKAGE@/config
#config_DATA = character_generator_gui.glade
#
#EXTRA_DIST = $(config_DATA)
