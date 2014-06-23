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

#ifndef RPG_NET_ASYNCHSOCKETHANDLER_H
#define RPG_NET_ASYNCHSOCKETHANDLER_H

#include "rpg_net_exports.h"
#include "rpg_net_asynchstreamhandler_t.h"
#include "rpg_net_common.h"
#include "rpg_net_stream.h"

#include <ace/Global_Macros.h>
#include <ace/Message_Block.h>

class RPG_Net_Export RPG_Net_AsynchSocketHandler
 : public RPG_Net_AsynchStreamHandler_T<RPG_Net_ConfigPOD,
                                        RPG_Net_RuntimeStatistic,
                                        RPG_Net_Stream>
{
 public:
  RPG_Net_AsynchSocketHandler();
  virtual ~RPG_Net_AsynchSocketHandler();

  // implement (part of) RPG_Net_IConnection
  virtual void ping();

  // override some handler method(s)
  virtual void open(ACE_HANDLE,          // (socket) handle
                    ACE_Message_Block&); // initial data (if any)

 private:
  typedef RPG_Net_AsynchStreamHandler_T<RPG_Net_ConfigPOD,
                                        RPG_Net_RuntimeStatistic,
                                        RPG_Net_Stream> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchStreamHandler_T(const RPG_Net_AsynchStreamHandler_T&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchStreamHandler_T& operator=(const RPG_Net_AsynchStreamHandler_T&));
};

#endif
