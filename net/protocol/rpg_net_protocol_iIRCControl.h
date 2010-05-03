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

#ifndef RPG_NET_PROTOCOL_IIRCCONTROL_H
#define RPG_NET_PROTOCOL_IIRCCONTROL_H

#include <string>

// forward declaration(s)
struct RPG_Net_Protocol_IRCLoginOptions;
class RPG_Net_Protocol_INotify;

class RPG_Net_Protocol_IIRCControl
{
 public:
  // *NOTE*: to shut up the compiler (gcc4) complaining about missing virtual dtors, set
  // -Wno-non-virtual-dtor in the project settings...

  // exposed interface
  virtual void joinIRC(const RPG_Net_Protocol_IRCLoginOptions&, // login details
                       const RPG_Net_Protocol_INotify*) = 0;    // data callback
  virtual void registerNotification(const RPG_Net_Protocol_INotify*) = 0; // data callback
  virtual void sendMessage(const std::string&) = 0; // message
  virtual void leaveIRC(const std::string&) = 0; // reason
};

#endif
