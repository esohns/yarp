/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include "stdafx.h"

#include "rpg_net_transportlayer_ip_cast.h"

#include <sstream>
#include <string>

#include "rpg_common_macros.h"

#include "rpg_net_defines.h"

RPG_Net_TransportLayer_IP_Broadcast::RPG_Net_TransportLayer_IP_Broadcast(RPG_Net_ClientServerRole_t clientServerRole_in)
 : inherited(),
//   myAddress(),
   myClientServerRole(clientServerRole_in),
   myTransportLayer(TRANSPORTLAYER_IP_BROADCAST),
   myPort(RPG_NET_DEFAULT_PORT),
   myUseLoopback(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Broadcast::RPG_Net_TransportLayer_IP_Broadcast"));

  std::string address = ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEFAULT_IP_BROADCAST_ADDRESS);
  address += ':';
  std::ostringstream converter;
  converter << RPG_NET_DEFAULT_PORT;
  address += converter.str();
  if (myAddress.set(ACE_TEXT(address.c_str()), AF_INET) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
  if (open(myAddress,
           PF_INET,
           0,
           0,
           NULL) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Bcast::open(): \"%m\", continuing\n")));
}

RPG_Net_TransportLayer_IP_Broadcast::~RPG_Net_TransportLayer_IP_Broadcast()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Broadcast::~RPG_Net_TransportLayer_IP_Broadcast"));

  if (close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Bcast::close(): \"%m\", continuing\n")));
}

void
RPG_Net_TransportLayer_IP_Broadcast::init(unsigned short port_in,
                                          bool useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Broadcast::init"));

  myPort = port_in;
  myUseLoopback = useLoopback_in;

  if (myAddress.get_port_number() != myPort)
  {
    if (close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Dgram_Bcast::close(): \"%m\", continuing\n")));
    myAddress.set_port_number(myPort, 1);
  } // end IF
  if (open(myAddress,
           PF_INET,
           0,
           0,
           NULL) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Bcast::open(): \"%m\", continuing\n")));
}

/////////////////////////////////////////

RPG_Net_TransportLayer_IP_Multicast::RPG_Net_TransportLayer_IP_Multicast(RPG_Net_ClientServerRole_t clientServerRole_in)
 : inherited(ACE_SOCK_Dgram_Mcast::DEFOPTS),
   myJoined(false),
//   myAddress(),
   myClientServerRole(clientServerRole_in),
   myTransportLayer(TRANSPORTLAYER_IP_MULTICAST),
   myPort(RPG_NET_DEFAULT_PORT),
   myUseLoopback(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Multicast::RPG_Net_TransportLayer_IP_Multicast"));

  std::string address = ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEFAULT_IP_MULTICAST_ADDRESS);
  address += ':';
  std::ostringstream converter;
  converter << RPG_NET_DEFAULT_PORT;
  address += converter.str();
  if (myAddress.set(ACE_TEXT(address.c_str()), AF_INET) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
  if (open(myAddress,
           NULL,
           1) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::open(): \"%m\", continuing\n")));
}

RPG_Net_TransportLayer_IP_Multicast::~RPG_Net_TransportLayer_IP_Multicast()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Multicast::~RPG_Net_TransportLayer_IP_Multicast"));

  if (myJoined)
    if (leave(myAddress,
              NULL) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::leave(): \"%m\", continuing\n")));
  if (close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::close(): \"%m\", continuing\n")));
}

void
RPG_Net_TransportLayer_IP_Multicast::init(unsigned short port_in,
                                          bool useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_IP_Multicast::init"));

  myPort = port_in;
  myUseLoopback = useLoopback_in;

  if (myAddress.get_port_number() != myPort)
  {
    if (myJoined)
    {
      if (leave(myAddress,
                NULL) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::leave(): \"%m\", continuing\n")));
      myJoined = false;
    } // end IF
    if (close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::close(): \"%m\", continuing\n")));
    myAddress.set_port_number(myPort, 1);
  } // end IF
  if (open(myAddress,
           NULL,
           1) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::open(): \"%m\", continuing\n")));
  if (join(myAddress,
           1,
           NULL) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram_Mcast::join(): \"%m\", continuing\n")));
  else
    myJoined = true;
}
