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

#ifndef RPG_NET_RESETCOUNTERHANDLER_H
#define RPG_NET_RESETCOUNTERHANDLER_H

#include "rpg_net_exports.h"

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

// forward declaration(s)
class RPG_Net_ICounter;

class RPG_Net_Export RPG_Net_ResetCounterHandler
 : public ACE_Event_Handler
{
 public:
  RPG_Net_ResetCounterHandler(RPG_Net_ICounter*); // counter interface
  virtual ~RPG_Net_ResetCounterHandler();

  // implement specific behaviour
  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ResetCounterHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ResetCounterHandler(const RPG_Net_ResetCounterHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ResetCounterHandler& operator=(const RPG_Net_ResetCounterHandler&));

  RPG_Net_ICounter* myCounter;
};

#endif
