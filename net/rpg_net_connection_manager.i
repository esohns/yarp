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

#include <ace/OS.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_net_defines.h"
#include "rpg_net_iconnection.h"

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::RPG_Net_Connection_Manager()
 : myCondition(myLock),
//                ACE_TEXT_ALWAYS_CHAR(""),
//                NULL),
   myMaxNumConnections(RPG_NET_MAX_NUM_OPEN_CONNECTIONS),
   myUserData(),
   myIsInitialized(false),
   myIsActive(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::RPG_Net_Connection_Manager"));

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(myUserData));
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
      // *NOTE*: we should NEVER get here !
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("%u connections still open --> check implementation !, continuing\n"),
                 myConnections.size()));

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
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::start"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  myIsActive = true;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::stop"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  myIsActive = false;
}

template <typename ConfigType,
          typename StatisticsContainerType>
bool
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::stop"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  return myIsActive;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::getConfig(ConfigType& config_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::getConfig"));

  // sanity check
  ACE_ASSERT(myIsInitialized);

  config_out = myUserData;
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

    if (!myIsActive)
      return false; // currently rejecting new connections...

    if (myConnections.size() >= myMaxNumConnections)
    {
      // max reached
      //ACE_DEBUG((LM_DEBUG,
      //           ACE_TEXT("rejecting connection (maximum count [%u] has been reached), aborting\n"),
      //           myMaxNumConnections));

      return false;
    } // end IF

    myConnections.insert_tail(connection_in);

    // debug info
    ACE_HANDLE handle = ACE_INVALID_HANDLE;
    ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
    ACE_OS::memset(buffer, 0, sizeof(buffer));
    std::string localAddress;
    ACE_INET_Addr local_SAP, remote_SAP;
    try
    {
      connection_in->info(handle, local_SAP, remote_SAP);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::info(), aborting")));

      return false;
    }
    if (local_SAP.addr_to_string(buffer, sizeof(buffer)) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    localAddress = buffer;
    ACE_OS::memset(buffer, 0, sizeof(buffer));
    if (remote_SAP.addr_to_string(buffer, sizeof(buffer)) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

    // *PORTABILITY*: this isn't entirely portable...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered connection [%@/%u]: (\"%s\") <--> (\"%s\") (total: %u)...\n"),
               handle, reinterpret_cast<unsigned int>(handle),
               localAddress.c_str(),
               buffer,
               myConnections.size()));
#else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered connection [%@/%u]: (\"%s\") <--> (\"%s\") (total: %u)...\n"),
               connection_in, handle,
               localAddress.c_str(),
               buffer,
               myConnections.size()));
#endif
  } // end lock scope

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

  // debug info
  ACE_HANDLE handle = ACE_INVALID_HANDLE;
  ACE_INET_Addr address1, address2;
  try
  {
    connection_in->info(handle,
                        address1,
                        address2);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_IConnection::info(), continuing\n")));
  }

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  CONNECTION_TYPE* connection = NULL;
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(myConnections);
       iterator.next(connection);
       iterator.advance())
    if (connection == connection_in)
    {
      found = true;

      iterator.remove();

      // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("deregistered connection %@/%u (total: %u)\n"),
                 connection_in, reinterpret_cast<unsigned int>(handle),
								 myConnections.size()));
#else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("deregistered connection %d (total: %u)\n"),
                 handle,
								 myConnections.size()));
#endif

      break;
    } // end IF

  if (!found)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to remove connection (%@): not found, aborting\n"),
               connection_in));

    // what else can we do ?
    return;
  } // end IF
  else
  { // if there are no more connections, signal any waiters...
    if (myConnections.is_empty() == 1)
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
  if (myConnections.is_empty() == 1)
    return;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u connections(s)...\n"),
             myConnections.size()));

  CONNECTION_TYPE* connection = NULL;
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(myConnections);
       iterator.next(connection);
       iterator.advance())
  {
    ACE_ASSERT(connection);
    try
    {
      // *IMPORTANT NOTE*: implicitly invokes deregisterConnection from a reactor thread, if any
      connection->abort();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), continuing")));
    }
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("aborting %u connection(s)...DONE\n"),
             myConnections.size()));
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

    while (myConnections.is_empty() == 0)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting for (count: %d) connection(s) to leave...\n"),
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

  size_t num_connections = 0;
  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    num_connections = myConnections.size();
  } // end lock scope

  return static_cast<unsigned int>(num_connections);
}

template <typename ConfigType,
          typename StatisticsContainerType>
const typename RPG_Net_Connection_Manager<ConfigType,
                                          StatisticsContainerType>::CONNECTION_TYPE*
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::operator[](unsigned int index_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::operator[]"));

  // init result
  CONNECTION_TYPE* result = NULL;

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check
  if (index_in > myConnections.size())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("out of bounds (index was: %u), aborting"),
               index_in));

    return result;
  } // end IF

  CONNECTIONLIST_ITERATOR_TYPE iterator(const_cast<CONNECTIONLIST_TYPE&>(myConnections));
  for (unsigned int i = 0;
       iterator.next(result) && (i < index_in);
       iterator.advance(), i++) {} // end FOR

  return result;
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
  if (myConnections.is_empty() == 1)
    return;

  // close "oldest" connection --> list head
  CONNECTION_TYPE* connection = NULL;
  if (myConnections.get(connection) != -1)
  {
    try
    {
      // *IMPORTANT NOTE*: implicitly invokes deregisterConnection from a reactor thread, if any
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
void
RPG_Net_Connection_Manager<ConfigType,
                           StatisticsContainerType>::abortNewestConnection()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Connection_Manager::abortNewestConnection"));

  // synch access to myConnections
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  // sanity check: anything to do ?
  if (myConnections.is_empty() == 1)
    return;

  // close "newest" connection --> list tail
  CONNECTION_TYPE* connection = NULL;
  CONNECTIONLIST_REVERSEITERATOR_TYPE iterator(myConnections);
  if (iterator.next(connection) == 1)
  {
    try
    {
      // *IMPORTANT NOTE*: implicitly invokes deregisterConnection from a reactor thread, if any
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
  ACE_OS::memset(&data_out, 0, sizeof(data_out));

  StatisticsContainerType temp;
  // aggregate statistical data
  // *WARNING*: this assumes we're holding our lock !
  CONNECTION_TYPE* connection = NULL;
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(const_cast<CONNECTIONLIST_TYPE&>(myConnections));
       iterator.next(connection);
       iterator.advance())
  {
    ACE_OS::memset(&temp, 0, sizeof(temp));
    try
    { // collect information
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
  ACE_OS::memset(&result, 0, sizeof(result));

  // synch access to myConnections
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    // sanity check: anything to do ?
    if (myConnections.is_empty() == 1)
      return;

    // collect (aggregated) data from our active connections
    if (!collect(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_IStatistic::collect(), aborting\n")));

      // nothing to report
      return;
    } // end IF

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
  for (CONNECTIONLIST_ITERATOR_TYPE iterator(const_cast<CONNECTIONLIST_TYPE&>(myConnections));
       iterator.next(connection);
       iterator.advance())
  {
    try
    { // dump connection information
      connection->dump_state();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnection::dump_state(), continuing")));
    }
  } // end FOR
}
