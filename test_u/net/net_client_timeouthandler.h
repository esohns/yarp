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

#ifndef NET_CLIENT_TIMEOUTHANDLER_H
#define NET_CLIENT_TIMEOUTHANDLER_H

#include <rpg_net_client_sockethandler.h>

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

#include <string>
#include <list>

class Net_Client_TimeoutHandler
 : public ACE_Event_Handler
{
 public:
  Net_Client_TimeoutHandler(const std::string&,                         // target hostname
                            const unsigned short&,                      // target port number
                            RPG_Net_Client_Connector*,                  // connector
                            std::list<RPG_Net_Client_SocketHandler*>*); // connection repository
  virtual ~Net_Client_TimeoutHandler();

  // implement specific behaviour
  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(Net_Client_TimeoutHandler());
  ACE_UNIMPLEMENTED_FUNC(Net_Client_TimeoutHandler(const Net_Client_TimeoutHandler&));
  ACE_UNIMPLEMENTED_FUNC(Net_Client_TimeoutHandler& operator=(const Net_Client_TimeoutHandler&));

  std::string                               myServerHostname;
  unsigned short                            myServerPortNumber;
  RPG_Net_Client_Connector*                 myConnector;
  std::list<RPG_Net_Client_SocketHandler*>* myConnectionHandlers;
};

#endif
