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

#include "net_server_eventhandler.h"

#include "gtk/gtk.h"

#include "common_ui_common.h"

#include "stream_common.h"

#include "rpg_common_macros.h"

#include "net_callbacks.h"

Net_Server_EventHandler::Net_Server_EventHandler (Net_Server_GTK_CBData* CBData_in)
 : CBData_ (CBData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::Net_Server_EventHandler"));

  ACE_ASSERT (CBData_);
}

void
Net_Server_EventHandler::start (Stream_SessionId_t sessionId_in,
                                const struct RPG_Net_Protocol_SessionData& sessionData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);

  ACE_ASSERT (CBData_);
  ACE_ASSERT (CBData_->UIState);

  guint event_source_id = g_idle_add (idle_start_session_server_cb,
                                      CBData_);
  if (!event_source_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_start_session_cb): \"%m\", returning\n")));
    return;
  } // end IF

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.insert (COMMON_UI_EVENT_CONNECT);
  } // end lock scope
}

void
Net_Server_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const enum Stream_SessionMessageType& notification_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (notification_in);
}

void
Net_Server_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::end"));

  ACE_UNUSED_ARG (sessionId_in);

  ACE_ASSERT (CBData_);
  ACE_ASSERT (CBData_->UIState);

  guint event_source_id = g_idle_add (idle_end_session_server_cb,
                                      CBData_);
  if (!event_source_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_end_session_cb): \"%m\", returning\n")));
    return;
  } // end IF

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.insert (COMMON_UI_EVENT_DISCONNECT);
  } // end lock scope
}

void
Net_Server_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const RPG_Net_Protocol_Message& message_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (message_in);
}

void
Net_Server_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const RPG_Net_Protocol_SessionMessage& sessionMessage_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Server_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionMessage_in);
}
