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

#ifndef RPG_NET_ASYNCHUDPSOCKETHANDLER_H
#define RPG_NET_ASYNCHUDPSOCKETHANDLER_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"
#include "rpg_net_stream.h"

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"

class RPG_Net_Export RPG_Net_AsynchUDPSocketHandler
 : public ACE_Handler
{
 public:
  RPG_Net_AsynchUDPSocketHandler();
  virtual ~RPG_Net_AsynchUDPSocketHandler();

  //// implement (part of) RPG_Net_IConnection
  //virtual void ping();

  // override some handler method(s)
  virtual void open(ACE_HANDLE,          // (socket) handle
                    ACE_Message_Block&); // initial data (if any)

 private:
  typedef ACE_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_AsynchUDPSocketHandler (const RPG_Net_AsynchUDPSocketHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_AsynchUDPSocketHandler& operator=(const RPG_Net_AsynchUDPSocketHandler&));
};

#endif
