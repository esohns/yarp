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

#ifndef IRC_CLIENT_SIGNALHANDLER_H
#define IRC_CLIENT_SIGNALHANDLER_H

#include "IRC_common.h"

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/INET_Addr.h>

#include <string>

class IRC_Client_SignalHandler
 : public ACE_Event_Handler
{
 public:
  IRC_Client_SignalHandler(const std::string&,     // target hostname
                           const unsigned short&,  // target port number
                           IRC_Client_Connector*); // connector
  virtual ~IRC_Client_SignalHandler();

  // implement specific behaviour
  virtual int handle_signal(int,          // signal
                            siginfo_t*,   // not needed on UNIX
                            ucontext_t*); // not used

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_SignalHandler());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_SignalHandler(const IRC_Client_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_SignalHandler& operator=(const IRC_Client_SignalHandler&));

  ACE_INET_Addr         myPeerAddress;
  IRC_Client_Connector* myConnector;
};

#endif
