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

#ifndef IRC_CLIENT_TOOLS_H
#define IRC_CLIENT_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"

#include "common.h"

#include "rpg_net_protocol_common.h"

// forward declaration(s)
class Stream_IAllocator;

class IRC_Client_Tools
{
 public:
  static bool connect (Stream_IAllocator*,                      // message allocator
                       const RPG_Net_Protocol_IRCLoginOptions&, // login options
                       bool,                                    // debug scanner ?
                       bool,                                    // debug parser ?
                       unsigned int,                            // statistics reporting interval (0: OFF)
                       const std::string&,                      // hostname
                       unsigned short,                          // port
                       Common_Module_t*);                       // final module

 private:
  ACE_UNIMPLEMENTED_FUNC (IRC_Client_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~IRC_Client_Tools ());
  ACE_UNIMPLEMENTED_FUNC (IRC_Client_Tools (const IRC_Client_Tools&));
  ACE_UNIMPLEMENTED_FUNC (IRC_Client_Tools& operator=(const IRC_Client_Tools&));
};

#endif
