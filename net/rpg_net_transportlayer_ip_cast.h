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

#ifndef RPG_NET_TRANSPORTLAYER_IP_CAST_H
#define RPG_NET_TRANSPORTLAYER_IP_CAST_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"
#include "rpg_net_transportlayer_base.h"

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include "ace/SOCK_Dgram_Mcast.h"

class RPG_Net_Export RPG_Net_TransportLayer_IP_Broadcast
 : public RPG_Net_TransportLayer_Base
 , public ACE_SOCK_Dgram_Bcast
{
 public:
  RPG_Net_TransportLayer_IP_Broadcast(RPG_Net_ClientServerRole_t);
  virtual ~RPG_Net_TransportLayer_IP_Broadcast();

  void init(unsigned short, // port number
            bool = false);  // use loopback device ?

 private:
  typedef RPG_Net_TransportLayer_Base inherited;
  typedef ACE_SOCK_Dgram_Bcast inherited2;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Broadcast());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Broadcast(const RPG_Net_TransportLayer_IP_Broadcast&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Broadcast& operator=(const RPG_Net_TransportLayer_IP_Broadcast&));

  ACE_INET_Addr myAddress;
};

/////////////////////////////////////////

class RPG_Net_Export RPG_Net_TransportLayer_IP_Multicast
 : public RPG_Net_TransportLayer_Base
 , public ACE_SOCK_Dgram_Mcast
{
 public:
  RPG_Net_TransportLayer_IP_Multicast(RPG_Net_ClientServerRole_t);
  virtual ~RPG_Net_TransportLayer_IP_Multicast();

  void init(unsigned short, // port number
            bool = false);  // use loopback device ?

 private:
  typedef RPG_Net_TransportLayer_Base inherited;
  typedef ACE_SOCK_Dgram_Mcast inherited2;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Multicast());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Multicast(const RPG_Net_TransportLayer_IP_Multicast&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_IP_Multicast& operator=(const RPG_Net_TransportLayer_IP_Multicast&));

  bool          myJoined;
  ACE_INET_Addr myAddress;
};

#endif
