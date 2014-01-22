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

#include "net_eventhandler.h"

#include "net_common.h"

#include "rpg_common_macros.h"

Net_EventHandler::Net_EventHandler(Net_GTK_CBData_t* CBData_in)
 : myCBData(CBData_in)
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::Net_EventHandler"));

  // sanity check(s)
  ACE_ASSERT(myCBData);
}

Net_EventHandler::~Net_EventHandler()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::~Net_EventHandler"));

}

void
Net_EventHandler::start()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::start"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myCBData->lock);

  myCBData->event_stack.push_back(NET_GTKEVENT_CONNECT);
}

void
Net_EventHandler::notify(const RPG_Net_Message& message_in)
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG(message_in);
}

void
Net_EventHandler::end()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::end"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myCBData->lock);

  myCBData->event_stack.push_back(NET_GTKEVENT_DISCONNECT);
}
