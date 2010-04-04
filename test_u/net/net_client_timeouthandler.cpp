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

#include "net_client_timeouthandler.h"

#include <rpg_net_client_sockethandler.h>

#include <ace/Reactor.h>

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler(const std::string& serverHostname_in,
                                                     const unsigned short& serverPort_in,
                                                     RPG_Net_Client_Connector* connector_in,
                                                     std::list<RPG_Net_Client_SocketHandler*>* connections_in)
 : inherited(ACE_Reactor::instance(),         // corresp. reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myServerHostname(serverHostname_in),
   myServerPortNumber(serverPort_in),
   myConnector(connector_in),
   myConnectionHandlers(connections_in)
{
  ACE_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

  // set corresp. reactor
  //reactor(reactor_in);
}

Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler()
{
  ACE_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler"));

}

int
Net_Client_TimeoutHandler::handle_timeout(const ACE_Time_Value& tv_in,
                                          const void* arg_in)
{
  ACE_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

  // step1: connect to server...
  RPG_Net_Client_SocketHandler* handler = NULL;
  ACE_INET_Addr remote_address(myServerPortNumber, // remote SAP
                               myServerHostname.c_str());
  if (myConnector->connect(handler,                     // service handler
                           remote_address/*,              // remote SAP
                           ACE_Synch_Options::defaults, // synch options
                           ACE_INET_Addr::sap_any,      // local SAP
                           0,                           // try to re-use address (SO_REUSEADDR)
                           O_RDWR,                      // flags
                           0*/) == -1)                  // perms
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Connector::connect(%s:%u): \"%s\", continuing\n"),
               ACE_TEXT_CHAR_TO_TCHAR(remote_address.get_host_name()),
               remote_address.get_port_number(),
               ACE_OS::strerror(errno)));

    // rejected ? --> release a connection !
    if (!(myConnectionHandlers->empty()))
    {
      RPG_Net_Client_SocketHandler* handler = myConnectionHandlers->back();
      // sanity check
      ACE_ASSERT(handler);

      if (handler->close(0) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Svc_Handler::close(): \"%s\", continuing\n"),
                   ACE_OS::strerror(errno)));
      } // end IF

      myConnectionHandlers->pop_back();

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("...released connection instead (remaining: %u)\n"),
                 myConnectionHandlers->size()));

      return 0;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    ACE_ASSERT(handler);

    // step2: add to connections
    myConnectionHandlers->push_front(handler);
  } // end ELSE

  return 0;
}
