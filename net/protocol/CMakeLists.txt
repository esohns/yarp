#add_subdirectory(scripts)

########### next target ###############
set(RPG_Net_Protocol_LIB_SRCS
    location.hh
    position.hh
    rpg_net_protocol_IRC_codes.h
    rpg_net_protocol_IRCbisect.cpp
    rpg_net_protocol_IRCbisect.h
    rpg_net_protocol_IRCmessage.cpp
    rpg_net_protocol_IRCmessage.h
    rpg_net_protocol_IRCparser.cpp
    rpg_net_protocol_IRCparser.h
    rpg_net_protocol_IRCparser_driver.cpp
    rpg_net_protocol_IRCparser_driver.h
    rpg_net_protocol_IRCscanner.cpp
    rpg_net_protocol_IRCscanner.h
    rpg_net_protocol_common.h
    rpg_net_protocol_defines.h
    rpg_net_protocol_iIRCControl.h
    rpg_net_protocol_inotify.h
    rpg_net_protocol_message.cpp
    rpg_net_protocol_message.h
    rpg_net_protocol_messageallocator.cpp
    rpg_net_protocol_messageallocator.h
    rpg_net_protocol_module_IRChandler.cpp
    rpg_net_protocol_module_IRChandler.h
    rpg_net_protocol_module_IRCparser.cpp
    rpg_net_protocol_module_IRCparser.h
    rpg_net_protocol_module_IRCsplitter.cpp
    rpg_net_protocol_module_IRCsplitter.h
    rpg_net_protocol_module_IRCstreamer.cpp
    rpg_net_protocol_module_IRCstreamer.h
    rpg_net_protocol_sessionmessage.cpp
    rpg_net_protocol_sessionmessage.h
    rpg_net_protocol_sockethandler.cpp
    rpg_net_protocol_sockethandler.h
    rpg_net_protocol_stream.cpp
    rpg_net_protocol_stream.h
    rpg_net_protocol_tools.cpp
    rpg_net_protocol_tools.h
    stack.hh)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Net_Protocol_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Net_Protocol SHARED ${RPG_Net_Protocol_LIB_SRCS})

add_dependencies(RPG_Net_Protocol RPG_Net RPG_Stream)

set_target_properties(RPG_Net_Protocol PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Net_Protocol LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  location.hh position.hh rpg_net_protocol_IRC_codes.h  	rpg_net_protocol_IRCmessage.h rpg_net_protocol_IRCparser.h rpg_net_protocol_IRCparser_driver.h  	rpg_net_protocol_common.h rpg_net_protocol_defines.h rpg_net_protocol_iIRCControl.h  	rpg_net_protocol_inotify.h rpg_net_protocol_message.h rpg_net_protocol_messageallocator.h  	rpg_net_protocol_module_IRChandler.h rpg_net_protocol_module_IRCparser.h  	rpg_net_protocol_module_IRCsplitter.h rpg_net_protocol_module_IRCstreamer.h rpg_net_protocol_sessionmessage.h  	rpg_net_protocol_sockethandler.h rpg_net_protocol_stream.h rpg_net_protocol_tools.h stack.hh DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = scripts
#
#include_HEADERS = location.hh position.hh rpg_net_protocol_IRC_codes.h \
#	rpg_net_protocol_IRCmessage.h rpg_net_protocol_IRCparser.h rpg_net_protocol_IRCparser_driver.h \
#	rpg_net_protocol_common.h rpg_net_protocol_defines.h rpg_net_protocol_iIRCControl.h \
#	rpg_net_protocol_inotify.h rpg_net_protocol_message.h rpg_net_protocol_messageallocator.h \
#	rpg_net_protocol_module_IRChandler.h rpg_net_protocol_module_IRCparser.h \
#	rpg_net_protocol_module_IRCsplitter.h rpg_net_protocol_module_IRCstreamer.h rpg_net_protocol_sessionmessage.h \
#	rpg_net_protocol_sockethandler.h rpg_net_protocol_stream.h rpg_net_protocol_tools.h stack.hh
#
#lib_LTLIBRARIES = libRPG_Net_Protocol.la
#
#libRPG_Net_Protocol_la_SOURCES = location.hh position.hh \
#	rpg_net_protocol_IRC_codes.h rpg_net_protocol_IRCbisect.cpp rpg_net_protocol_IRCbisect.h \
#	rpg_net_protocol_IRCmessage.cpp rpg_net_protocol_IRCmessage.h rpg_net_protocol_IRCparser.cpp \
#	rpg_net_protocol_IRCparser.h rpg_net_protocol_IRCparser_driver.cpp \
#	rpg_net_protocol_IRCparser_driver.h rpg_net_protocol_IRCscanner.cpp rpg_net_protocol_IRCscanner.h \
#	rpg_net_protocol_common.h rpg_net_protocol_defines.h rpg_net_protocol_iIRCControl.h \
#	rpg_net_protocol_inotify.h rpg_net_protocol_message.cpp rpg_net_protocol_message.h \
#	rpg_net_protocol_messageallocator.cpp rpg_net_protocol_messageallocator.h \
#	rpg_net_protocol_module_IRChandler.cpp rpg_net_protocol_module_IRChandler.h \
#	rpg_net_protocol_module_IRCparser.cpp rpg_net_protocol_module_IRCparser.h \
#	rpg_net_protocol_module_IRCsplitter.cpp rpg_net_protocol_module_IRCsplitter.h \
#	rpg_net_protocol_module_IRCstreamer.cpp rpg_net_protocol_module_IRCstreamer.h \
#	rpg_net_protocol_sessionmessage.cpp rpg_net_protocol_sessionmessage.h rpg_net_protocol_sockethandler.cpp \
#	rpg_net_protocol_sockethandler.h rpg_net_protocol_stream.cpp rpg_net_protocol_stream.h \
#	rpg_net_protocol_tools.cpp rpg_net_protocol_tools.h stack.hh
#libRPG_Net_Protocol_la_CPPFLAGS = -I$(srcdir)/..
