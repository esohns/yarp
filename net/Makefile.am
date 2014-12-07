#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_NET_BUILD_DLL=1 \
  -DMETA_PACKAGE=\"@META_PACKAGE@\" \
  -I$(srcdir)/../common \
  -I$(srcdir)/../chance/dice \
  -I$(srcdir)/stream

SUBDIRS = stream . protocol client server scripts

# define dependencies
.: stream
protocol: .
client: .
server: .

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_net_asynch_tcpsockethandler.h \
  rpg_net_asynch_udpsockethandler.h \
  rpg_net_common_modules.h \
  rpg_net_common.h \
  rpg_net_common_tools.h \
  rpg_net_connection_base.h \
  rpg_net_connection_base.inl \
  rpg_net_connection_common.h \
  rpg_net_connection_manager_common.h \
  rpg_net_connection_manager.h \
  rpg_net_connection_manager.inl \
  rpg_net_controller_base.h \
  rpg_net_defines.h \
  rpg_net_exports.h \
  rpg_net_iconnection.h \
  rpg_net_iconnectionmanager.h \
  rpg_net_icounter.h \
  rpg_net_inotify.h \
  rpg_net_itransportlayer.h \
  rpg_net_message_base.h \
  rpg_net_message_base.inl \
  rpg_net_message.h \
  rpg_net_module_eventhandler.h \
  rpg_net_module_headerparser.h \
  rpg_net_module_protocolhandler.h \
  rpg_net_module_runtimestatistic.h \
  rpg_net_module_runtimestatistic.inl \
  rpg_net_module_sockethandler.h \
  rpg_net_packet_headers.h \
  rpg_net_protocol_layer.h \
  rpg_net_remote_comm.h \
  rpg_net_resetcounterhandler.h \
  rpg_net_sessionmessage.h \
  rpg_net_socket_common.h \
  rpg_net_socketconnection_base.h \
  rpg_net_socketconnection_base.inl \
  rpg_net_sockethandler_base.h \
  rpg_net_statistichandler.h \
  rpg_net_statistichandler.inl \
  rpg_net_stream_asynch_tcpsocket_base.h \
  rpg_net_stream_asynch_tcpsocket_base.inl \
  rpg_net_stream_asynch_udpsocket_base.h \
  rpg_net_stream_asynch_udpsocket_base.inl \
  rpg_net_stream_common.h \
  rpg_net_stream_config.h \
  rpg_net_stream_messageallocator.h \
  rpg_net_stream_tcpsocket_base.h \
  rpg_net_stream_tcpsocket_base.inl \
  rpg_net_stream_udpsocket_base.h \
  rpg_net_stream_udpsocket_base.inl \
  rpg_net_stream.h \
  rpg_net_tcpconnection.h \
  rpg_net_tcpsockethandler.h \
  rpg_net_transportlayer_base.h \
  rpg_net_transportlayer_ip_cast.h \
  rpg_net_transportlayer_tcp.h \
  rpg_net_transportlayer_udp.h \
  rpg_net_udpconnection.h \
  rpg_net_udpsockethandler.h

lib_LTLIBRARIES = libRPG_Net.la

libRPG_Net_la_SOURCES = \
  rpg_net_asynch_tcpsockethandler.cpp \
  rpg_net_asynch_tcpsockethandler.h \
  rpg_net_asynch_udpsockethandler.cpp \
  rpg_net_asynch_udpsockethandler.h \
  rpg_net_common_modules.h \
  rpg_net_common_tools.cpp \
  rpg_net_common_tools.h \
  rpg_net_common.h \
  rpg_net_connection_base.cpp \
  rpg_net_connection_base.h \
  rpg_net_connection_base.inl \
  rpg_net_connection_common.h \
  rpg_net_connection_manager_common.h \
  rpg_net_connection_manager.cpp \
  rpg_net_connection_manager.h \
  rpg_net_connection_manager.inl \
  rpg_net_controller_base.cpp \
  rpg_net_controller_base.h \
  rpg_net_defines.h \
  rpg_net_exports.h \
  rpg_net_iconnection.h \
  rpg_net_iconnectionmanager.h \
  rpg_net_icounter.h \
  rpg_net_inotify.h \
  rpg_net_itransportlayer.h \
  rpg_net_message_base.cpp \
  rpg_net_message_base.h \
  rpg_net_message_base.inl \
  rpg_net_message.cpp \
  rpg_net_message.h \
  rpg_net_module_eventhandler.cpp \
  rpg_net_module_eventhandler.h \
  rpg_net_module_headerparser.cpp \
  rpg_net_module_headerparser.h \
  rpg_net_module_protocolhandler.cpp \
  rpg_net_module_protocolhandler.h \
  rpg_net_module_runtimestatistic.cpp \
  rpg_net_module_runtimestatistic.h \
  rpg_net_module_runtimestatistic.inl \
  rpg_net_module_sockethandler.cpp \
  rpg_net_module_sockethandler.h \
  rpg_net_packet_headers.h \
  rpg_net_protocol_layer.cpp \
  rpg_net_protocol_layer.h \
  rpg_net_remote_comm.h \
  rpg_net_resetcounterhandler.cpp \
  rpg_net_resetcounterhandler.h \
  rpg_net_sessionmessage.cpp \
  rpg_net_sessionmessage.h \
  rpg_net_socket_common.h \
  rpg_net_socketconnection_base.cpp \
  rpg_net_socketconnection_base.h \
  rpg_net_socketconnection_base.inl \
  rpg_net_sockethandler_base.cpp \
  rpg_net_sockethandler_base.h \
  rpg_net_statistichandler.cpp \
  rpg_net_statistichandler.h \
  rpg_net_statistichandler.inl \
  rpg_net_stream_asynch_tcpsocket_base.cpp \
  rpg_net_stream_asynch_tcpsocket_base.h \
  rpg_net_stream_asynch_tcpsocket_base.inl \
  rpg_net_stream_asynch_udpsocket_base.cpp \
  rpg_net_stream_asynch_udpsocket_base.h \
  rpg_net_stream_asynch_udpsocket_base.inl \
  rpg_net_stream_common.h \
  rpg_net_stream_config.cpp \
  rpg_net_stream_config.h \
  rpg_net_stream_messageallocator.cpp \
  rpg_net_stream_messageallocator.h \
  rpg_net_stream_socket_base.cpp \
  rpg_net_stream_tcpsocket_base.cpp \
  rpg_net_stream_tcpsocket_base.h \
  rpg_net_stream_tcpsocket_base.inl \
  rpg_net_stream_udpsocket_base.cpp \
  rpg_net_stream_udpsocket_base.h \
  rpg_net_stream_udpsocket_base.inl \
  rpg_net_stream.cpp \
  rpg_net_stream.h \
  rpg_net_tcpconnection.cpp \
  rpg_net_tcpconnection.h \
  rpg_net_tcpsockethandler.cpp \
  rpg_net_tcpsockethandler.h \
  rpg_net_transportlayer_base.cpp \
  rpg_net_transportlayer_base.h \
  rpg_net_transportlayer_ip_cast.cpp \
  rpg_net_transportlayer_ip_cast.h \
  rpg_net_transportlayer_tcp.cpp \
  rpg_net_transportlayer_tcp.h \
  rpg_net_transportlayer_udp.cpp \
  rpg_net_transportlayer_udp.h \
  rpg_net_udpconnection.cpp \
  rpg_net_udpconnection.h \
  rpg_net_udpsockethandler.cpp \
  rpg_net_udpsockethandler.h \
  stdafx.cpp \
  stdafx.h
