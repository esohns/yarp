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

#ifndef RPG_COMMON_TIMERHANDLER_H
#define RPG_COMMON_TIMERHANDLER_H

#include "rpg_common_exports.h"
#include "rpg_common_itimer.h"

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

class RPG_Common_Export RPG_Common_TimerHandler
 : public ACE_Event_Handler
{
 public:
  RPG_Common_TimerHandler(RPG_Common_ITimer*,                // dispatch interface
                          const bool& isOneShot_in = false); // invoke only once ?
  virtual ~RPG_Common_TimerHandler();

  // implement specific behaviour
  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TimerHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TimerHandler(const RPG_Common_TimerHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TimerHandler& operator=(const RPG_Common_TimerHandler&));

  RPG_Common_ITimer* myDispatch;
  bool               myIsOneShot;
};

#endif
