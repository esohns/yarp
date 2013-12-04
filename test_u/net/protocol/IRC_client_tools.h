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

#include "rpg_net_protocol_common.h"

#include <gtk/gtk.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class RPG_Stream_IAllocator;
class RPG_Stream_Module;

class IRC_Client_Tools
{
 public:
  static const bool connect(RPG_Stream_IAllocator*,                  // message allocator
                            const RPG_Net_Protocol_IRCLoginOptions&, // login options
                            const bool&,                             // debug scanner ?
                            const bool&,                             // debug parser ?
                            const unsigned long&,                    // statistics reporting interval (0: OFF)
                            const std::string&,                      // hostname
                            const unsigned short&,                   // port
                            RPG_Stream_Module*);                     // final module

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_Tools());
  ACE_UNIMPLEMENTED_FUNC(~IRC_Client_Tools());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_Tools(const IRC_Client_Tools&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_Tools& operator=(const IRC_Client_Tools&));
};

#endif
