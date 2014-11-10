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

#ifndef RPG_NET_TRANSPORTLAYER_TCP_H
#define RPG_NET_TRANSPORTLAYER_TCP_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"
#include "rpg_net_transportlayer_base.h"

class RPG_Net_Export RPG_Net_TransportLayer_TCP
 : public RPG_Net_TransportLayer_Base
{
 public:
  RPG_Net_TransportLayer_TCP(RPG_Net_ClientServerRole_t);
  virtual ~RPG_Net_TransportLayer_TCP();

  void init(unsigned short, // port number
            bool = false);  // use loopback device ?

 private:
  typedef RPG_Net_TransportLayer_Base inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_TCP());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_TCP(const RPG_Net_TransportLayer_TCP&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TransportLayer_TCP& operator=(const RPG_Net_TransportLayer_TCP&));
};

#endif
