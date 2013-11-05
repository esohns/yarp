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

#ifndef RPG_NET_ASYNCHSOCKETHANDLER_T_H
#define RPG_NET_ASYNCHSOCKETHANDLER_T_H

#include "rpg_net_iconnection.h"
#include "rpg_net_iconnectionmanager.h"

#include <ace/Global_Macros.h>
#include <ace/Asynch_IO.h>
#include <ace/INET_Addr.h>

// forward declarations
class ACE_Message_Block;

template <typename ConfigType,
          typename StatisticsContainerType>
class RPG_Net_AsynchSocketHandler_T
 : public ACE_Service_Handler,
   public RPG_Net_IConnection<ConfigType,
                              StatisticsContainerType>
{
 public:
  virtual ~RPG_Net_AsynchSocketHandler_T();

  //check if registration with the connection manager was OK...
  virtual void open(ACE_HANDLE,          // (socket) handle
                    ACE_Message_Block&); // initial data (if any)
  virtual void addresses(const ACE_INET_Addr&,  // remote address
                         const ACE_INET_Addr&); // local address
  virtual int handle_close(ACE_HANDLE,        // (socket) handle
						               ACE_Reactor_Mask); // reactor mask

  // implement RPG_Net_IConnection
  virtual void init(const ConfigType&);
//   virtual const bool isRegistered() const;
  virtual void abort();
  virtual unsigned int getID() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 protected:
  // convenient types
  typedef RPG_Net_IConnectionManager<ConfigType,
                                     StatisticsContainerType> MANAGER_t;
  // meant to be sub-classed
  RPG_Net_AsynchSocketHandler_T(MANAGER_t*); // manager handle
	// *TODO*: clean this up ASAP !!!!
  RPG_Net_AsynchSocketHandler_T();

  // helper method(s)
  void initiate_read_stream();

  virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result&); // result

  ConfigType 							myUserData;
  bool       							myIsInitialized;
  ACE_Asynch_Write_Stream myOutputStream;

 private:
  typedef ACE_Service_Handler inherited;

  // safety measures
	// *TODO*: clean this up ASAP !!!!
  //ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler_T());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler_T(const RPG_Net_AsynchSocketHandler_T&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler_T& operator=(const RPG_Net_AsynchSocketHandler_T&));

  // helper method(s)
  ACE_Message_Block* allocateMessage(const unsigned int&); // requested size

  ACE_Asynch_Read_Stream  myInputStream;
	ACE_INET_Addr           myPeer;

  // *NOTE*: we save this so we can de-register even when our "handle"
  // (getID()) has gone stale...
  unsigned int						myID;
  bool       							myIsRegistered;
  MANAGER_t* 							myManager;
};

// include template definition
#include "rpg_net_asynchsockethandler_t.i"

#endif

