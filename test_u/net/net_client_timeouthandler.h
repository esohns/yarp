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

#include <string>

#include "ace/Event_Handler.h"
#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"

#include "net_client_common.h"

class Net_Client_TimeoutHandler
 : public ACE_Event_Handler
{
 public:
  enum ActionMode_t
  {
    ACTION_NORMAL = 0,
    ACTION_ALTERNATING,
    ACTION_STRESS,
    //// ---------------------
    //ACTION_GTK, // dispatch UI events
    // ---------------------
    ACTION_MAX,
    ACTION_INVALID = -1
  };

  enum AlternatingMode_t
  {
    ALTERNATING_CONNECT = 0,
    ALTERNATING_ABORT,
    // ---------------------
    ALTERNATING_MAX,
    ALTERNATING_INVALID = -1
  };

  Net_Client_TimeoutHandler (ActionMode_t,            // mode
                             unsigned int,            // max #connections
                             const ACE_INET_Addr&,    // remote SAP
                             Net_Client_IConnector*); // connector
  virtual ~Net_Client_TimeoutHandler ();

  // implement specific behaviour
  virtual int handle_timeout (const ACE_Time_Value&, // current time
                              const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_TimeoutHandler ());
  ACE_UNIMPLEMENTED_FUNC (Net_Client_TimeoutHandler (const Net_Client_TimeoutHandler&));
  ACE_UNIMPLEMENTED_FUNC (Net_Client_TimeoutHandler& operator=(const Net_Client_TimeoutHandler&));

  ActionMode_t           myMode;
  AlternatingMode_t      myAlternatingMode;
  unsigned int           myMaxNumConnections;
  ACE_INET_Addr          myPeerAddress;
  Net_Client_IConnector* myConnector;
};

#endif
