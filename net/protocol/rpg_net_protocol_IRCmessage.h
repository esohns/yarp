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

#include <list>
#include <string>
#include <vector>

// *NOTE*: the list of parameters is essentially a simple list of items
// (separated by ' '). However, an item can itself be a list (separated by ',')...
// --> accomodate this "feature" by including a list of "ranges" (start/end positions
// of list-items)
typedef std::list<std::string> string_list_t;
typedef string_list_t::iterator string_list_iterator_t;
typedef string_list_t::const_iterator string_list_const_iterator_t;
typedef std::pair<unsigned long, unsigned long> list_item_range_t;
typedef std::vector<list_item_range_t> list_items_ranges_t;
typedef list_items_ranges_t::const_iterator list_items_ranges_iterator_t;

typedef string_list_t RPG_Net_Protocol_Parameters_t;
typedef string_list_const_iterator_t RPG_Net_Protocol_ParametersIterator_t;

class RPG_Net_Protocol_IRCMessage
 : public RPG_Common_ReferenceCounter
{
  public:
    enum CommandType
    {
      // *NOTE*: in an effort to avoid clashes and still handle IRC commands
      //         and replies/errors uniformly, start this beyond the
      //         range of (numeric) commands (== three-digit number)
      //         --> check RFC1459
      //         --> see also: rpg_net_protocol_IRC_codes.h
      PASS = 1000,
      NICK,
      USER,
      SERVER,
      OPER,
      QUIT,
      SQUIT,
      JOIN,
      PART,
      MODE,
      TOPIC,
      NAMES,
      LIST,
      INVITE,
      KICK,
      SVERSION, // *TODO*: "VERSION" is taken by config.h...
      STATS,
      LINKS,
      TIME,
      CONNECT,
      TRACE,
      ADMIN,
      INFO,
      PRIVMSG,
      NOTICE,
      WHO,
      WHOIS,
      WHOWAS,
      KILL,
      PING,
      PONG,
      ERROR,
      AWAY,
      REHASH,
      RESTART,
      SUMMON,
      USERS,
      WALLOPS,
      USERHOST,
      ISON,
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

    RPG_Net_Protocol_Parameters_t params;
    list_items_ranges_t           list_param_ranges;

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
