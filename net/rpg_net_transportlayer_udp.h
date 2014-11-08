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

#ifndef RPG_NET_TRANSPORTLAYER_UDP_H
#define RPG_NET_TRANSPORTLAYER_UDP_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"

class RPG_Net_Export RPG_Net_TransportLayer_UDP
 : public ACE_SOCK_Dgram
{
 public:
  RPG_Net_TransportLayer_UDP(RPG_Net_ClientServerRole_t);
  virtual ~RPG_Net_TransportLayer_UDP();

  void init(unsigned short, // port number
            bool = false);  // use loopback device ?

 private:
  typedef ACE_SOCK_Dgram inherited;

  RPG_Net_TransportLayer_UDP();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_UDP(const RPG_Net_TransportLayer_UDP&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_UDP& operator=(const RPG_Net_TransportLayer_UDP&));

  ACE_INET_Addr              myAddress;

  RPG_Net_ClientServerRole_t myClientServerRole;
  RPG_Net_TransportLayer_t   myTransportLayer;
  unsigned short             myPort;
  bool                       myUseLoopback;
};

#endif
