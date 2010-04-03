/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include "rpg_net_connection_manager.h"

#include "rpg_net_defines.h"
#include "rpg_net_iconnection.h"

RPG_Net_Connection_Manager::RPG_Net_Connection_Manager()
 : myMaxNumConnections(RPG_NET_DEF_MAX_NUM_OPEN_CONNECTIONS)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::RPG_Net_Connection_Manager"));

}

RPG_Net_Connection_Manager::~RPG_Net_Connection_Manager()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::~RPG_Net_Connection_Manager"));

  // clean up
  {
    // synch access to myConnections
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    if (!myConnections.empty())
    {
      // *NOTE*: we should NEVER get here; this is just a precaution !
      abortConnections();
    } // end IF
  } // end lock scope
}

void
RPG_Net_Connection_Manager::init(const unsigned long& maxNumConnections_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::init"));

  myMaxNumConnections = maxNumConnections_in;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("set maximum # connections: %u\n"),
             myMaxNumConnections));
}

const bool
RPG_Net_Connection_Manager::registerConnection(RPG_Net_IConnection* connection_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::registerConnection"));

  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    if (myConnections.size() >= myMaxNumConnections)
    {
      // max reached
      ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("rejecting connection (maximum count of %u has been reached), aborting\n"),
                myMaxNumConnections));

      return false;
    } // end IF

    myConnections.push_back(connection_in);

    ACE_DEBUG((LM_DEBUG,
              ACE_TEXT("registered new client connection (current total: %u)\n"),
              myConnections.size()));
  } // end lock scope

  // debug information
  try
  {
    connection_in->dump_state();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_IConnection::dump_state(), continuing\n")));
  }

  return true;
}

void
RPG_Net_Connection_Manager::deregisterConnection(const unsigned long& connectionID_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::deregisterConnection"));

  bool          found   = false;
  unsigned long temp_id = 0;

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  for (CONNECTIONLIST_ITERATOR_TYPE iter = myConnections.begin();
       iter != myConnections.end();
       iter++)
  {
    try
    {
      temp_id = (*iter)->getID();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::getID(), continuing\n")));

      continue;
    }

    if (temp_id == connectionID_in)
    {
      found = true;
      try
      {
        myConnections.erase(iter);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in std::list::erase(), aborting\n")));

        // what else can we do ?
        return;
      }

      // don't need to continue...
      break;
    } // end IF
  } // end FOR

  if (!found)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("connection (ID: %u) not registered, returning\n"),
               connectionID_in));
  } // end IF
  else
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("deregistered connection (ID: %u) (total remaining: %u)\n"),
               connectionID_in,
               myConnections.size()));
  } // end ELSE
}

void
RPG_Net_Connection_Manager::abortConnections()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::abortConnections"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check: anything to do ?
  if (myConnections.empty())
  {
    //ACE_DEBUG((LM_DEBUG,
    //           ACE_TEXT("nothing to do, returning\n")));

    return;
  } // end IF

  unsigned long num_clients = myConnections.size();
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u client(s)...\n"),
             num_clients));

  CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
  while (!myConnections.empty())
  {
    iter = myConnections.begin();

    // close connection
    try
    {
      // *NOTE*: this should implicitly call deregisterConnection !
      (*iter)->abort();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
    }
  } // end WHILE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u client(s)...DONE\n"),
             num_clients));
}

const unsigned long
RPG_Net_Connection_Manager::numConnections() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::numConnections"));

  unsigned long num_connections = 0;
  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    num_connections = myConnections.size();
  } // end lock scope

  return num_connections;
}

void
RPG_Net_Connection_Manager::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::dump_state"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  for (CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
       iter != myConnections.end();
       iter++)
  {
    // dump connection information
    try
    {
      (*iter)->dump_state();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::dump_state(), continuing")));
    }
  } // end FOR
}
