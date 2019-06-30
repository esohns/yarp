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

#ifndef RPG_NET_PROTOCOL_CONFIGURATION_H
#define RPG_NET_PROTOCOL_CONFIGURATION_H

#include "ace/Time_Value.h"

#include "common_configuration.h"

#include "stream_common.h"
#include "stream_configuration.h"

#include "net_common.h"
#include "net_connection_configuration.h"

#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct RPG_Net_Protocol_SessionData,
                                    enum Stream_SessionMessageType,
                                    RPG_Net_Protocol_Message,
                                    RPG_Net_Protocol_SessionMessage> RPG_Net_Protocol_ISessionNotify_t;
typedef std::list<RPG_Net_Protocol_ISessionNotify_t*> RPG_Net_Protocol_Subscribers_t;
typedef RPG_Net_Protocol_Subscribers_t::iterator RPG_Net_Protocol_SubscribersIterator_t;

struct RPG_Net_Protocol_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  RPG_Net_Protocol_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , subscriber (NULL)
   , subscribers (NULL)
  {}

  RPG_Net_Protocol_ISessionNotify_t* subscriber;
  RPG_Net_Protocol_Subscribers_t* subscribers;
};

struct RPG_Net_Protocol_StreamConfiguration
 : Stream_Configuration
{
  RPG_Net_Protocol_StreamConfiguration ()
   : Stream_Configuration ()
   , crunchMessageBuffers (true)
#if defined (_DEBUG)
   , debugScanner (false)
   , debugParser (false)
#endif // _DEBUG
  {}

  bool crunchMessageBuffers;
#if defined (_DEBUG)
  bool debugScanner;
  bool debugParser;
#endif // _DEBUG
};
typedef Stream_Configuration_T<//empty_string_,
                               struct Common_FlexParserAllocatorConfiguration,
                               struct RPG_Net_Protocol_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct RPG_Net_Protocol_ModuleHandlerConfiguration> RPG_Net_Protocol_StreamConfiguration_t;

class RPG_Net_Protocol_ConnectionConfiguration
 : public Net_ConnectionConfiguration_T<struct Common_FlexParserAllocatorConfiguration,
                                        RPG_Net_Protocol_StreamConfiguration_t,
                                        NET_TRANSPORTLAYER_TCP>
{
 public:
   RPG_Net_Protocol_ConnectionConfiguration ()
   : Net_ConnectionConfiguration_T ()
  {}
};

struct RPG_Net_Protocol_Configuration
{
  RPG_Net_Protocol_Configuration ()
   : connectionConfiguration ()
   , streamConfiguration ()
   , clientPingInterval (0)
   //, loginOptions ()
  {}

  // **************************** socket data **********************************
  RPG_Net_Protocol_ConnectionConfiguration connectionConfiguration;

  // **************************** stream data **********************************
  RPG_Net_Protocol_StreamConfiguration_t   streamConfiguration;

  // *************************** protocol data *********************************
  unsigned int                             clientPingInterval; // server only
  //RPG_Net_Protocol_IRCLoginOptions         loginOptions;
};

#endif
