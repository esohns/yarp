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

#ifndef RPG_NET_SIGNALHANDLER_H
#define RPG_NET_SIGNALHANDLER_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"

#include <rpg_common_istatistic.h>

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

#include <string>

// forward declaration(s)
class RPG_Common_IControl;

class RPG_Net_Export RPG_Net_SignalHandler
 : public ACE_Event_Handler
{
 public:
  RPG_Net_SignalHandler(RPG_Common_IControl*,                                     // controller
                        RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>* = NULL); // reporter
  virtual ~RPG_Net_SignalHandler();

  // implement specific behaviour
  virtual int handle_signal(int,          // signal
                            siginfo_t*,   // info
                            ucontext_t*); // context

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SignalHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SignalHandler(const RPG_Net_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SignalHandler& operator=(const RPG_Net_SignalHandler&));

  RPG_Common_IControl*                             myControl;
  RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>* myReport;
};

#endif
