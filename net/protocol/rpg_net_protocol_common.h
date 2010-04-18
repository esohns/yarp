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

#include "rpg_net_protocol_IRC_codes.h"

#include <string>
#include <vector>

struct RPG_Net_Protocol_IRCMessage
{
  union
  {
    std::string serverName;
    std::string nick;
  };
  std::string user;
  std::string host;
  struct
  {
    union
    {
      std::string string;
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
  std::vector<std::string> params;
};

#endif
