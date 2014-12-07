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

#include "ace/OS.h"
#include "ace/Log_Msg.h"

template <typename ConfigurationType,
          typename StatisticsContainerType>
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::RPG_Net_ConnectionBase(MANAGER_T* manager_in)
 : inherited(1,    // initial count
             true) // delete on zero ?
 , myManager(manager_in)
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

  fini ();
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
bool
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::init(const ACE_INET_Addr& peerAddress_in,
                                                      unsigned short portNumber_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::init"));

  ACE_UNUSED_ARG (peerAddress_in);
  ACE_UNUSED_ARG (portNumber_in);

  // sanity check(s)
  if (!myManager)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT ("invalid connection manager, aborting\n")));
    return false;
  } // end if

  // (try to) register with the connection manager...
  // *warning*: as we register before the connection has fully opened, there
  // is a small window for races...
  try
  {
    myIsRegistered = myManager->registerConnection(this);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_IConnectionManager::egisterConnection(), continuing\n")));
  }

  return true;
}

template <typename ConfigurationType,
          typename StatisticsContainerType>
void
RPG_Net_ConnectionBase<ConfigurationType,
                       StatisticsContainerType>::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_ConnectionBase::fini"));

  // sanity check(s)
  if (!myManager)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid connection manager, returning\n")));
    return;
  } // end IF

  if (myIsRegistered)
  {
    // (try to) de-register with the connection manager...
    // *WARNING*: as we register BEFORE the connection has fully opened, there
    // is a small window for races...
    try
    {
      myManager->deregisterConnection(this);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::deregisterConnection(), continuing\n")));
    }

    myIsRegistered = false;
  } // end IF
}
