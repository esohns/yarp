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

#ifndef RPG_Net_Protocol_EventHandler_H
#define RPG_Net_Protocol_EventHandler_H

#include "ace/Global_Macros.h"

#include "stream_common.h"
#include "stream_isessionnotify.h"

#include "rpg_net_protocol_session_message.h"
#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"

class RPG_Net_Protocol_EventHandler
 : public RPG_Net_Protocol_ISessionNotify_t
{
 public:
  RPG_Net_Protocol_EventHandler ();
  inline virtual ~RPG_Net_Protocol_EventHandler () {}

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,
                      const struct RPG_Net_SessionData&);
  inline virtual void notify (Stream_SessionId_t, const enum Stream_SessionMessageType&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  virtual void end (Stream_SessionId_t);
  virtual void notify (Stream_SessionId_t,
                       const RPG_Net_Protocol_Message&);
  virtual void notify (Stream_SessionId_t,
                       const RPG_Net_Protocol_SessionMessage&);

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_EventHandler (const RPG_Net_Protocol_EventHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_EventHandler& operator= (const RPG_Net_Protocol_EventHandler&));
};

#endif // RPG_Net_Protocol_EventHandler_H
