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

#include "rpg_net_protocol_tools.h"

#include "rpg_net_protocol_common.h"

#include <sstream>

const std::string
RPG_Net_Protocol_Tools::IRCMessage2String(const RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCMessage2String"));

  std::ostringstream converter;
  // see RFC1459
  if (message_in.prefix.origin)
  {
    converter << ACE_TEXT("PREFIX [origin: \"") << *message_in.prefix.origin << ACE_TEXT("\"]");
    if (message_in.prefix.user)
      converter << ACE_TEXT(", [user: \"") << *message_in.prefix.user << ACE_TEXT("\"]");
    if (message_in.prefix.host)
      converter << ACE_TEXT(", [host: \"") << *message_in.prefix.host << ACE_TEXT("\"]");
    converter << ACE_TEXT(" \\PREFIX") << std::endl;
  } // end IF
  converter << ACE_TEXT("COMMAND ");
  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      converter << ACE_TEXT("[\"") << *message_in.command.string << ACE_TEXT("\"]");

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      converter << ACE_TEXT("[") << message_in.command.numeric << ACE_TEXT("]");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command discriminator (was: %d), aborting\n"),
                 message_in.command.discriminator));

      return converter.str();
    }
  } // end SWITCH
  converter << ACE_TEXT(" \\COMMAND") << std::endl;
  if (message_in.params)
  {
    if (!message_in.params->empty())
    {
      converter << ACE_TEXT("PARAMS") << std::endl;
      int i = 1;
      for (std::vector<std::string>::const_iterator iterator = message_in.params->begin();
           iterator != message_in.params->end();
           iterator++, i++)
      {
        converter << ACE_TEXT("#") << i << ACE_TEXT(" :\"") << *iterator << ACE_TEXT("\"") << std::endl;
      } // end FOR
      converter << ACE_TEXT("\\PARAMS") << std::endl;
    } // end IF
  } // end IF

  return converter.str();
}
