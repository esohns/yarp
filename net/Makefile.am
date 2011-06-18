METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = stream protocol

include_HEADERS = rpg_net_client_sockethandler.h rpg_net_common.h \
	rpg_net_common_tools.h rpg_net_connection_manager.h rpg_net_connection_manager.i \
	rpg_net_controller_base.h rpg_net_defines.h rpg_net_iconnection.h rpg_net_iconnectionmanager.h \
	rpg_net_icounter.h rpg_net_listener.h rpg_net_message.h rpg_net_module_headerparser.h \
	rpg_net_module_protocolhandler.h rpg_net_module_runtimestatistic.h rpg_net_module_runtimestatistic.i \
	rpg_net_module_sockethandler.h rpg_net_packet_headers.h rpg_net_protocol_layer.h rpg_net_remote_comm.h \
	rpg_net_resetcounterhandler.h rpg_net_sessionmessage.h rpg_net_signalhandler.h \
	rpg_net_sockethandler.h rpg_net_sockethandler_base.h rpg_net_sockethandler_base.i \
	rpg_net_statistichandler.h rpg_net_statistichandler.i rpg_net_stream.h rpg_net_stream_config.h \
	rpg_net_stream_messageallocator.h rpg_net_stream_socket_base.h rpg_net_stream_socket_base.i

lib_LTLIBRARIES = libRPG_Net.la

libRPG_Net_la_SOURCES = rpg_net_client_sockethandler.cpp \
	rpg_net_client_sockethandler.h rpg_net_common.h rpg_net_common_tools.cpp rpg_net_common_tools.h \
	rpg_net_connection_manager.cpp rpg_net_connection_manager.h rpg_net_connection_manager.i \
	rpg_net_controller_base.cpp rpg_net_controller_base.h rpg_net_defines.h rpg_net_iconnection.h \
	rpg_net_iconnectionmanager.h rpg_net_icounter.h rpg_net_listener.cpp rpg_net_listener.h \
	rpg_net_message.cpp rpg_net_message.h rpg_net_module_headerparser.cpp \
	rpg_net_module_headerparser.h rpg_net_module_protocolhandler.cpp rpg_net_module_protocolhandler.h \
	rpg_net_module_runtimestatistic.cpp rpg_net_module_runtimestatistic.h rpg_net_module_runtimestatistic.i \
	rpg_net_module_sockethandler.cpp rpg_net_module_sockethandler.h rpg_net_packet_headers.h \
	rpg_net_protocol_layer.cpp rpg_net_protocol_layer.h rpg_net_remote_comm.h \
	rpg_net_resetcounterhandler.cpp rpg_net_resetcounterhandler.h rpg_net_sessionmessage.cpp \
	rpg_net_sessionmessage.h rpg_net_signalhandler.cpp rpg_net_signalhandler.h \
	rpg_net_sockethandler.cpp rpg_net_sockethandler.h rpg_net_sockethandler_base.cpp \
	rpg_net_sockethandler_base.h rpg_net_sockethandler_base.i rpg_net_statistichandler.cpp \
	rpg_net_statistichandler.h rpg_net_statistichandler.i rpg_net_stream.cpp rpg_net_stream.h \
	rpg_net_stream_config.cpp rpg_net_stream_config.h rpg_net_stream_messageallocator.cpp \
	rpg_net_stream_messageallocator.h rpg_net_stream_socket_base.cpp rpg_net_stream_socket_base.h \
	rpg_net_stream_socket_base.i
