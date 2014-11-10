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

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
//#include "rpg_net_iconnectionmanager.h"

#include <string>

template <typename ConfigurationType,
          typename StatisticsContainerType>
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::RPG_Net_ConnectionBase(MANAGER_T* manager_in,
                                                                        RPG_Net_TransportLayer_Base* transportLayer_in)
 : inherited(1,    // initial count
             true) // delete on zero ?
 , myManager(manager_in)
 , myTransportLayer(transportLayer_in)
// , myUserData()
 , myIsRegistered(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::RPG_Net_ConnectionBase"));

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(myUserData));
  if (myManager)
  {
    try
    { // (try to) get user data from the connection manager
      myManager->getConfiguration(myUserData);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::getConfiguration(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::~RPG_Net_ConnectionBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::~RPG_Net_ConnectionBase"));

  //if (myManager && myIsRegistered)
  //{ // (try to) de-register with connection manager
  //  try
  //  {
  //    myManager->deregisterConnection(this);
  //  }
  //  catch (...)
  //  {
  //    ACE_DEBUG((LM_ERROR,
  //               ACE_TEXT("caught exception in RPG_Net_IConnectionManager::deregisterConnection(), continuing\n")));
  //  }
  //} // end IF
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
void
RPG_Net_ConnectionBase<ConfigurationType,
                          StatisticsContainerType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::dump_state"));

  // sanity check(s)
  ACE_ASSERT(myTransportLayer);

  ACE_HANDLE handle = ACE_INVALID_HANDLE;
  ACE_INET_Addr local_inet_address, peer_inet_address;
  try
  {
    myTransportLayer->info(handle,
                           local_inet_address,
                           peer_inet_address);
  } // end try
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_ITransportLayer::info(), returning\n")));
    return;
  } // end catch

  ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  std::string local_address;
  if (local_inet_address.addr_to_string(buffer,
                                        sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    local_address = buffer;
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  std::string peer_address;
  if (peer_inet_address.addr_to_string(buffer,
                                       sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  else
    peer_address = buffer;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [Id: %u [%u]]: \"%s\" <--> \"%s\"\n"),
             id(), handle,
             ACE_TEXT(local_address.c_str()),
             ACE_TEXT(peer_address.c_str())));
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
bool
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::open(const ACE_INET_Addr& peerAddress_in,
                                                      unsigned short portNumber_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::open"));

  // sanity check(s)
//  if (!myManager)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("invalid connection manager, aborting\n")));
//    return -1;
//  } // end IF

//  if (myManager)
//  {
//    // (try to) register with the connection manager...
//    // *WARNING*: as we register BEFORE the connection has fully opened, there
//    // is a small window for races...
//    try
//    {
//      myIsRegistered = myManager->registerConnection(this);
//    }
//    catch (...)
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
//    }
//  } // end IF

  // *TODO*
  ACE_ASSERT (false);

  return true;
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
void
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::close()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::close"));

  // sanity check(s)
//  if (!myManager)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("invalid connection manager, aborting\n")));
//    return -1;
//  } // end IF

//  if (myManager)
//  {
//    // (try to) register with the connection manager...
//    // *WARNING*: as we register BEFORE the connection has fully opened, there
//    // is a small window for races...
//    try
//    {
//      myIsRegistered = myManager->registerConnection(this);
//    }
//    catch (...)
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
//    }
//  } // end IF

  // *TODO*
  ACE_ASSERT (false);
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
void
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::ping()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::ping"));

  // *TODO*
  ACE_ASSERT (false);
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
void
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::info(ACE_HANDLE& handle_out,
                                                      ACE_INET_Addr& localAddress_out,
                                                      ACE_INET_Addr& peerAddress_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::info"));

  // sanity check(s)
  ACE_ASSERT(myTransportLayer);

  try
  {
    myTransportLayer->info(handle_out,
                           localAddress_out,
                           peerAddress_out);
  } // end try
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_ITransportLayer::info(), continuing\n")));
  } // end catch
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::id() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::id"));

  // sanity check(s)
  ACE_ASSERT(myTransportLayer);

  try
  {
    return myTransportLayer->id();
  } // end try
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_ITransportLayer::id(), aborting\n")));
  } // end catch

  return 0;
}
