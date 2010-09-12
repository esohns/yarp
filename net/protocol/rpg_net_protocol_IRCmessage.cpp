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

#include "rpg_net_protocol_IRCmessage.h"

#include "rpg_net_protocol_tools.h"

RPG_Net_Protocol_IRCMessage::RPG_Net_Protocol_IRCMessage()
 : inherited(1,
             true)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::RPG_Net_Protocol_IRCMessage"));

  command.string = NULL;
  command.discriminator = Command::INVALID;
}

RPG_Net_Protocol_IRCMessage::~RPG_Net_Protocol_IRCMessage()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::~RPG_Net_Protocol_IRCMessage"));

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
}

void
RPG_Net_Protocol_IRCMessage::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCMessage::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%s"),
             RPG_Net_Protocol_Tools::dump(*this).c_str()));
}
