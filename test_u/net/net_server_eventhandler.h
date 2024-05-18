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

#ifndef NET_SERVER_EVENTHANDLER_H
#define NET_SERVER_EVENTHANDLER_H

#include "stream_isessionnotify.h"

#include "net_server_common.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"

// forward declaration(s)
typedef Stream_ISessionDataNotify_T<struct RPG_Net_Protocol_SessionData,
                                    enum Stream_SessionMessageType,
                                    RPG_Net_Protocol_Message,
                                    RPG_Net_Protocol_SessionMessage> RPG_Net_Protocol_ISessionNotify_t;

class Net_Server_EventHandler
 : public RPG_Net_Protocol_ISessionNotify_t
{
 public:
  Net_Server_EventHandler (Net_Server_GTK_CBData*); // GTK state
  inline virtual ~Net_Server_EventHandler () {}

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,                          // session id
                      const struct RPG_Net_Protocol_SessionData&); // session data
  virtual void notify (Stream_SessionId_t,                     // session id
                       const enum Stream_SessionMessageType&); // event (state/status change, ...)
  virtual void end (Stream_SessionId_t); // session id
  virtual void notify (Stream_SessionId_t,               // session id
                       const RPG_Net_Protocol_Message&); // (protocol) data
  virtual void notify (Stream_SessionId_t,                      // session id
                       const RPG_Net_Protocol_SessionMessage&); // session message

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_Server_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (Net_Server_EventHandler (const Net_Server_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (Net_Server_EventHandler& operator=(const Net_Server_EventHandler&))

  Net_Server_GTK_CBData* CBData_;
};

#endif
