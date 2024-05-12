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

#ifndef TEST_U_NET_CLIENT_COMMON_H
#define TEST_U_NET_CLIENT_COMMON_H

#include <list>

#include "ace/Thread_Mutex.h"

#include "common_ui_gtk_common.h"

#include "stream_common.h"
#include "stream_isessionnotify.h"

#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"
#include "rpg_net_protocol_stream_common.h"

// forward declaration(s)
struct RPG_Client_Configuration;
typedef Stream_ISessionDataNotify_T<struct RPG_Net_Protocol_SessionData,
                                    enum Stream_SessionMessageType,
                                    RPG_Net_Protocol_Message,
                                    RPG_Net_Protocol_SessionMessage> RPG_Net_Protocol_ISessionNotify_t;
typedef std::list<RPG_Net_Protocol_ISessionNotify_t*> RPG_Net_Protocol_Subscribers_t;
typedef RPG_Net_Protocol_Subscribers_t::iterator RPG_Net_Protocol_SubscribersIterator_t;

struct Net_Client_GTK_CBData
 : Common_UI_GTK_CBData
{
  Net_Client_GTK_CBData ()
   : Common_UI_GTK_CBData ()
   , allowUserRuntimeStatistic (true)
   , configuration (NULL)
   , subscribers ()
   //, subscribersLock ()
  {}

  bool                             allowUserRuntimeStatistic;
  struct RPG_Client_Configuration* configuration;
  RPG_Net_Protocol_Subscribers_t   subscribers;
  //ACE_Recursive_Thread_Mutex     subscribersLock;
};

#endif
