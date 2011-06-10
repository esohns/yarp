/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <rpg_net_sockethandler.h>
// #include <rpg_net_client_sockethandler.h>
// #include <rpg_net_stream_socket_base.h>
// #include <rpg_net_stream.h>

#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>

// convenient types
// typedef RPG_Net_StreamSocketBase<RPG_Net_Stream> RPG_Net_Client_SocketHandler;
typedef RPG_Net_SocketHandler RPG_Net_Client_SocketHandler;
typedef ACE_Connector<RPG_Net_Client_SocketHandler,
                      ACE_SOCK_CONNECTOR> RPG_Net_Client_Connector;

#endif
