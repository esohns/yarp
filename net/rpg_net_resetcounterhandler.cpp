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
#include "stdafx.h"

#include "rpg_net_resetcounterhandler.h"

#include "rpg_net_icounter.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Net_ResetCounterHandler::RPG_Net_ResetCounterHandler(RPG_Net_ICounter* counter_in)
  : inherited(NULL,                            // no reactor
              ACE_Event_Handler::LO_PRIORITY), // priority
    myCounter(counter_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ResetCounterHandler::RPG_Net_ResetCounterHandler"));

}

RPG_Net_ResetCounterHandler::~RPG_Net_ResetCounterHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ResetCounterHandler::~RPG_Net_ResetCounterHandler"));

}

int
RPG_Net_ResetCounterHandler::handle_timeout(const ACE_Time_Value& tv_in,
                                            const void* arg_in)
{
//   RPG_TRACE(ACE_TEXT("RPG_Net_ResetCounterHandler::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

  try
  {
    myCounter->reset();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught an exception in RPG_Net_ICounter::reset() --> check implementation !, continuing\n")));

    // *TODO*: what else can we do ?
  }

  return 0;
}
