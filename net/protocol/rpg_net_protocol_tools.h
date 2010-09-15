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

#ifndef RPG_NET_PROTOCOL_TOOLS_H
#define RPG_NET_PROTOCOL_TOOLS_H

#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_IRCmessage.h"
#include "rpg_net_protocol_IRC_codes.h"

#include <ace/Global_Macros.h>

#include <string>

class RPG_Net_Protocol_Tools
{
 public:
  // debug info
  static const std::string dump(const RPG_Net_Protocol_IRCMessage&);
  static const RPG_Net_Protocol_CommandType_t IRCCommandString2Type(const std::string&);
  static const std::string IRCCode2String(const RPG_Net_Protocol_IRCNumeric_t&);
  static const RPG_Net_Protocol_UserMode IRCUserModeChar2UserMode(const char&);
  static const RPG_Net_Protocol_ChannelMode IRCChannelModeChar2ChannelMode(const char&);
  static const std::string IRCChannelMode2String(const RPG_Net_Protocol_ChannelMode&);
  static const std::string IRCUserMode2String(const RPG_Net_Protocol_UserMode&);

  static const std::string IRCMessage2String(const RPG_Net_Protocol_IRCMessage&);

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Tools());
  ACE_UNIMPLEMENTED_FUNC(virtual ~RPG_Net_Protocol_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Tools(const RPG_Net_Protocol_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Tools& operator=(const RPG_Net_Protocol_Tools&));

  // helper methods
  static const std::string concatParams(const RPG_Net_Protocol_Parameters_t&, // parameters
                                        const unsigned long& = 0);            // starting index
};

#endif
