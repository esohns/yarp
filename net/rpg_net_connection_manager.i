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

#include "rpg_net_defines.h"
#include "rpg_net_iconnection.h"

#include <ace/OS.h>

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::RPG_Net_Connection_Manager()
 : myCondition(myLock),
//                ACE_TEXT_ALWAYS_CHAR(""),
//                NULL),
   myMaxNumConnections(RPG_NET_MAX_NUM_OPEN_CONNECTIONS),
   myUserData(),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::RPG_Net_Connection_Manager"));

//   // init user data
//   ACE_OS::memset(&myUserData,
//                  0,
//                  sizeof(ConfigType));
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::~RPG_Net_Connection_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::~RPG_Net_Connection_Manager"));

  // clean up
  {
    // synch access to myConnections
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    if (!myConnections.is_empty())
    {
      // *NOTE*: we should NEVER get here; this is just a precaution !
      abortConnections();
    } // end IF
  } // end lock scope
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::init(const unsigned int& maxNumConnections_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::init"));

  myMaxNumConnections = maxNumConnections_in;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set maximum # connections: %u\n"),
//              myMaxNumConnections));
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::set(const ConfigType& userData_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::set"));

  myUserData = userData_in;

  myIsInitialized = true;
}

template <typename ConfigType,
          typename StatisticsContainerType>
bool
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::registerConnection(CONNECTION_TYPE* connection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::registerConnection"));

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

    myConnections.insert_tail(connection_in);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered connection %@ (total: %u)...\n"),
               connection_in,
               myConnections.size()));
  } // end lock scope

  // initialize registered connection...
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

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::deregisterConnection(const CONNECTION_TYPE* connection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::deregisterConnection"));

  bool found = false;

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  CONNECTION_TYPE* connection = NULL;
//   for (CONNECTIONLIST_ITERATOR_TYPE iter = myConnections.begin();
//        iter != myConnections.end();
//        iter++)
  for (ACE_DLList_Iterator<CONNECTION_TYPE> iterator(myConnections);
       iterator.next(connection);
       iterator.advance())
  {
    if (connection == connection_in)
    {
      found = true;

      iterator.remove();

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("deregistered connection (%@) (total: %u)\n"),
                 connection_in,
                 myConnections.size()));

      break;
    } // end IF
  } // end FOR

  if (!found)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to remove connection (%@): not found, aborting\n"),
               connection_in));

    // what else can we do ?
    return;
  } // end IF
  else
  {  // if there are no more connections, signal any waiters...
    if (myConnections.is_empty())
      myCondition.broadcast();
  } // end ELSE
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::abortConnections()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::abortConnections"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check: anything to do ?
  if (myConnections.is_empty())
  {
    //ACE_DEBUG((LM_DEBUG,
    //           ACE_TEXT("nothing to do, returning\n")));

    return;
  } // end IF

  unsigned long num_clients = myConnections.size();
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u connections(s)...\n"),
             num_clients));

  CONNECTION_TYPE* connection = NULL;
//   for (ACE_DLList_Iterator<CONNECTION_TYPE> iterator(myConnections);
//        iterator.next(connection);
//        iterator.advance())
//   CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
  do
  {
    connection = NULL;
    if (myConnections.get(connection) != -1)
    {
      try
      {
        // close connection
        // *NOTE*: implicitly, this invokes deregisterConnection
        connection->abort();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
      }
    } // end IF
  } while (!myConnections.is_empty());

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u connection(s)...DONE\n"),
             num_clients));
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::waitConnections() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::waitConnections"));

  {
    // need lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    while (!myConnections.is_empty())
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

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::numConnections() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::numConnections"));

  unsigned long num_connections = 0;
  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    num_connections = myConnections.size();
  } // end lock scope

  return num_connections;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::abortOldestConnection()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::abortOldestConnection"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check: anything to do ?
  if (myConnections.is_empty())
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("nothing to do, returning\n")));

    return;
  } // end IF

  // close "oldest" connection --> list head
  CONNECTION_TYPE* connection = NULL;
  if (myConnections.get(connection) != -1)
  {
    try
    {
      // implicitly invokes deregisterConnection
      connection->abort();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
bool
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::collect(StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::collect"));

  // init result
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(StatisticsContainerType));

  StatisticsContainerType temp;
  // aggregate statistical data
  // *WARNING*: this assumes we're holding our lock !
  CONNECTION_TYPE* connection = NULL;
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(const_cast<ACE_DLList<CONNECTION_TYPE>&> (myConnections));
       iterator.next(connection);
       iterator.advance())
//   for (CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
//        iter != myConnections.end();
//        iter++)
  {
    ACE_OS::memset(&temp,
                   0,
                   sizeof(StatisticsContainerType));

    // collect information
    try
    {
      connection->collect(temp);
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

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::report"));

  // init result
  StatisticsContainerType result;
  ACE_OS::memset(&result,
                 0,
                 sizeof(StatisticsContainerType));

  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    // sanity check: anything to do ?
    if (myConnections.is_empty())
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

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::dump_state"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  CONNECTION_TYPE* connection = NULL;
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(const_cast<ACE_DLList<CONNECTION_TYPE>&> (myConnections));
       iterator.next(connection);
       iterator.advance())
//   for (CONNECTIONLIST_CONSTITERATOR_TYPE iter = myConnections.begin();
//        iter != myConnections.end();
//        iter++)
  {
    // dump connection information
    try
    {
      connection->dump_state();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::dump_state(), continuing")));
    }
  } // end FOR
}
