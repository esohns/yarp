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

#include <ace/OS.h>

RPG_Net_Connection_Manager::RPG_Net_Connection_Manager()
 : myCondition(myLock),
   myMaxNumConnections(RPG_NET_DEF_MAX_NUM_OPEN_CONNECTIONS),
   //myUserData(),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::RPG_Net_Connection_Manager"));

  // init user data
  ACE_OS::memset(&myUserData,
                 0,
                 sizeof(RPG_Net_ConfigPOD));
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
RPG_Net_Connection_Manager::init(const unsigned long& maxNumConnections_in,
                                 const RPG_Net_ConfigPOD& userData_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::init"));

  myMaxNumConnections = maxNumConnections_in;

  // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set maximum # connections: %u\n"),
//              myMaxNumConnections));

  myUserData = userData_in;

  myIsInitialized = true;
}

const bool
RPG_Net_Connection_Manager::registerConnection(RPG_Net_IConnection* connection_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::registerConnection"));

  // sanity check
  ACE_ASSERT(myIsInitialized);

  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    if (myConnections.size() >= myMaxNumConnections)
    {
      // max reached
//       ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("rejecting connection (maximum count of %u has been reached), aborting\n"),
//                 myMaxNumConnections));

      return false;
    } // end IF

    myConnections.push_back(connection_in);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered connection %@ (total: %u)...\n"),
               connection_in,
               myConnections.size()));
  } // end lock scope

  // registered connections are initialized...
  try
  {
    connection_in->init(myUserData);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_IConnection::init(), continuing\n")));
  }

  // *WARNING*: can't get valid connection info at this stage...
//   // debug information
//   try
//   {
//     connection_in->dump_state();
//   }
//   catch (...)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("caught exception in RPG_Net_IConnection::dump_state(), continuing\n")));
//   }

  return true;
}

void
RPG_Net_Connection_Manager::deregisterConnection(const RPG_Net_IConnection* connection_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::deregisterConnection"));

  bool found = false;

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  for (CONNECTIONLIST_ITERATOR_TYPE iter = myConnections.begin();
       iter != myConnections.end();
       iter++)
  {
    if (*iter == connection_in)
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
               ACE_TEXT("connection (%@) not registered, returning\n"),
               connection_in));
  } // end IF
  else
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("deregistered connection (%@) (total: %u)\n"),
               connection_in,
               myConnections.size()));
  } // end ELSE

  // if there are no more connections, signal any waiters...
  if (myConnections.empty())
    myCondition.broadcast();
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
             ACE_TEXT("aborting %u connections(s)...\n"),
             num_clients));

  CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
  while (!myConnections.empty())
  {
    iter = myConnections.begin();

    // close connection
    try
    {
      // *NOTE*: implicitly, this invokes deregisterConnection
      (*iter)->abort();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
    }
  } // end WHILE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u connection(s)...DONE\n"),
             num_clients));
}

void
RPG_Net_Connection_Manager::waitConnections() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::waitConnections"));

  {
    // need lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    while (!myConnections.empty())
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting for (count: %d) connection(s) to close...\n"),
                 myConnections.size()));

      myCondition.wait();
    } // end WHILE
  } // end lock scope

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("leaving...\n")));
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
RPG_Net_Connection_Manager::abortOldestConnection()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::abortOldestConnection"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check: anything to do ?
  if (myConnections.empty())
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("nothing to do, returning\n")));

    return;
  } // end IF

  // close "oldest" connection --> list head
  try
  {
    // implicitly invokes deregisterConnection
    myConnections.front()->abort();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
  }
}

const bool
RPG_Net_Connection_Manager::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::collect"));

  // init result
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  RPG_Net_RuntimeStatistic temp;
  // aggregate statistical data
  // *WARNING*: this assumes we're holding our lock !
  for (CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
       iter != myConnections.end();
       iter++)
  {
    ACE_OS::memset(&temp,
                   0,
                   sizeof(RPG_Net_RuntimeStatistic));

    // collect information
    try
    {
      (*iter)->collect(temp);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("caught exception in RPG_Common_IStatistic::collect(), continuing\n")));
    }

    data_out += temp;
  } // end FOR

  return true;
}

void
RPG_Net_Connection_Manager::report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::report"));

  // init result
  RPG_Net_RuntimeStatistic result;
  ACE_OS::memset(&result,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    // sanity check: anything to do ?
    if (myConnections.empty())
    {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("nothing to do, returning\n")));

      return;
    } // end IF

    // collect (aggregated) data from our active connections
    if (!collect(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_IStatistic::collect(), aborting\n")));

      // nothing to report
      return;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("*** RUNTIME STATISTICS ***\n--> [%u] Connection(s) <--\n # data messages: %u (avg.: %u)\ndata: %.0f (avg.: %.2f) bytes\n*** RUNTIME STATISTICS ***\\END\n"),
               myConnections.size(),
               result.numDataMessages,
               (myConnections.size() ? (result.numDataMessages / myConnections.size()) : 0),
               result.numBytes,
               (myConnections.size() ? (result.numBytes / myConnections.size()) : 0.0)));
  } // end lock scope
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
