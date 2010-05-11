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

#ifndef RPG_NET_PROTOCOL_IRCMESSAGE_H
#define RPG_NET_PROTOCOL_IRCMESSAGE_H

#include "rpg_net_protocol_IRC_codes.h"

#include <rpg_common_referencecounter.h>

#include <ace/Global_Macros.h>

#include <string>
#include <list>

class RPG_Net_Protocol_IRCMessage
 : public RPG_Common_ReferenceCounter
{
  public:
    enum CommandType
    {
      // *NOTE*: in an effort to avoid clashes and still handle IRC commands
      //         and replies/errors uniformly, start this beyond the (known)
      //         range of (numeric) commands
      //         --> check RFC1459
      //         --> see also: rpg_net_protocol_IRC_codes.h
      PASS = 900,
      NICK,
      USER,
      QUIT,
      JOIN,
      MODE,
      PRIVMSG,
      NOTICE,
      PING,
      PONG,
      ERROR,
      //
      RPG_NET_PROTOCOL_COMMANDTYPE_MAX,
      RPG_NET_PROTOCOL_COMMANDTYPE_INVALID
    };

    // *WARNING*: this class assumes responsibility for all dynamic objects
    // "attached" to it in the course of its life (see dtor)
    RPG_Net_Protocol_IRCMessage();

    void dump_state() const;

    struct Prefix
    {
      std::string origin; // <servername> || <nick>
      std::string user;
      std::string host;
    } prefix;

    struct Command
    {
      union
      {
        std::string* string;
        RPG_Net_Protocol_IRC_Codes::RFC1459Numeric numeric;
      };
      enum discriminator_t
      {
        STRING = 0,
        NUMERIC,
        INVALID
      };
      discriminator_t discriminator;
    } command;

    std::list<std::string> params;

  private:
   typedef RPG_Common_ReferenceCounter inherited;

  // safety measures
//   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage());
   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage(const RPG_Net_Protocol_IRCMessage&));
   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage& operator=(const RPG_Net_Protocol_IRCMessage&));
   virtual ~RPG_Net_Protocol_IRCMessage();
};

typedef RPG_Net_Protocol_IRCMessage::CommandType RPG_Net_Protocol_CommandType_t;

#endif
