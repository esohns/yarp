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

#include "ace/Synch.h"

#include "stream_common.h"

//#include "net_message.h"

#include "rpg_common_macros.h"

#include "net_common.h"

Net_EventHandler::Net_EventHandler (Net_GTK_CBData_t* CBData_in)
 : CBData_ (CBData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::Net_EventHandler"));

  ACE_ASSERT (CBData_);
}

void
Net_EventHandler::start (Stream_SessionId_t sessionId_in,
                         const struct RPG_Net_Protocol_SessionData& sessionData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);

  ACE_ASSERT (CBData_);

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (CBData_->stackLock);

  CBData_->eventStack.push_back (NET_GTKEVENT_CONNECT);
}

void
Net_EventHandler::notify (Stream_SessionId_t sessionId_in,
                          const enum Stream_SessionMessageType& notification_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (notification_in);
}

void
Net_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::end"));

  ACE_UNUSED_ARG (sessionId_in);

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (CBData_->stackLock);

  CBData_->eventStack.push_back (NET_GTKEVENT_DISCONNECT);
}

void
Net_EventHandler::notify (Stream_SessionId_t sessionId_in,
                          const RPG_Net_Protocol_Message& message_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (message_in);
}

void
Net_EventHandler::notify (Stream_SessionId_t sessionId_in,
                          const RPG_Net_Protocol_SessionMessage& sessionMessage_in)
{
  RPG_TRACE (ACE_TEXT ("Net_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionMessage_in);
}
