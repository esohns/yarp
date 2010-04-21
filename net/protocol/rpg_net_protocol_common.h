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

#ifndef RPG_NET_PROTOCOL_COMMON_H
#define RPG_NET_PROTOCOL_COMMON_H

#include "rpg_net_protocol_tools.h"
#include "rpg_net_protocol_IRC_codes.h"

#include <rpg_common_referencecounter.h>

#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>

#include <string>
#include <list>

class RPG_Net_Protocol_IRCMessage
 : public RPG_Common_ReferenceCounter
{
 public:
  inline RPG_Net_Protocol_IRCMessage()
   : inherited(1, true)
  {
    ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::RPG_Net_Protocol_IRCMessage"));

    prefix.origin = NULL;
    prefix.user = NULL;
    prefix.host = NULL;
    command.string = NULL;
    command.discriminator = Command::INVALID;
    params = NULL;
  };

  inline virtual void dump_state() const
  {
    ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::dump_state"));

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%s"),
               RPG_Net_Protocol_Tools::IRCMessage2String(*this).c_str()));
    inherited::dump_state();
  };

  struct Prefix
  {
    std::string* origin; // <servername> || <nick>
    std::string* user;
    std::string* host;
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

  std::list<std::string>* params;

 private:
  typedef RPG_Common_ReferenceCounter inherited;
  // safety measures
//   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage(const RPG_Net_Protocol_IRCMessage&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCMessage& operator=(const RPG_Net_Protocol_IRCMessage&));
  inline virtual ~RPG_Net_Protocol_IRCMessage()
  {
    ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::~RPG_Net_Protocol_IRCMessage"));

    if (prefix.origin)
      delete prefix.origin;
    if (prefix.user)
      delete prefix.user;
    if (prefix.host)
      delete prefix.host;
    switch (command.discriminator)
    {
      case RPG_Net_Protocol_IRCMessage::Command::STRING:
      {
        if (command.string)
          delete command.string;

        break;
      }
      default:
        break;
    } // end SWITCH
    if (params)
    {
      params->clear();
      delete params;
    } // end IF
  };
};

#endif
