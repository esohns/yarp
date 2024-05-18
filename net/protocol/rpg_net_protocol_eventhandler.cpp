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

#include "rpg_net_protocol_eventhandler.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

RPG_Net_Protocol_EventHandler::RPG_Net_Protocol_EventHandler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_EventHandler::RPG_Net_Protocol_EventHandler"));

}

void
RPG_Net_Protocol_EventHandler::start (Stream_SessionId_t sessionId_in,
                                      const struct RPG_Net_SessionData& sessionData_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_EventHandler::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);
}

void
RPG_Net_Protocol_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_EventHandler::end"));

  ACE_UNUSED_ARG (sessionId_in);
}

void
RPG_Net_Protocol_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                       const RPG_Net_Protocol_Message& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (message_in);
}

void
RPG_Net_Protocol_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                       const RPG_Net_Protocol_SessionMessage& sessionMessage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionMessage_in);
}
