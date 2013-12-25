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

#ifndef RPG_NET_MODULE_SOCKETHANDLER_H
#define RPG_NET_MODULE_SOCKETHANDLER_H

#include "rpg_net_common.h"
#include "rpg_net_stream_config.h"

#include "rpg_common_istatistic.h"

#include "rpg_stream_headmoduletask_base.h"
#include "rpg_stream_streammodule.h"

#include <ace/Global_Macros.h>
#include <ace/Time_Value.h>

// forward declaration(s)
class RPG_Stream_IAllocator;
class RPG_Net_SessionMessage;
class RPG_Net_Message;

class RPG_Net_Module_SocketHandler
 : public RPG_Stream_HeadModuleTaskBase<RPG_Net_ConfigPOD,
                                        RPG_Net_StreamConfig,
                                        RPG_Net_SessionMessage,
                                        RPG_Net_Message>,
   // callback to trigger statistics collection...
   public RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>
{
 public:
  RPG_Net_Module_SocketHandler();
  virtual ~RPG_Net_Module_SocketHandler();

  // configuration / initialization
  bool init(RPG_Stream_IAllocator*,   // message allocator
		        const bool& = false,      // active object ?
            const unsigned int& = 0); // statistics collecting interval (second(s))
                                      // 0 --> DON'T collect statistics

  // user interface
  // info
  bool isInitialized() const;
  unsigned int getSessionID() const;

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Message*&, // data message handle
                                 bool&);            // return value: pass message downstream ?

  // catch the session ID...
  virtual void handleSessionMessage(RPG_Net_SessionMessage*&, // session message handle
                                    bool&);                   // return value: pass message downstream ?

  // implement RPG_Common_IStatistic
  // *NOTE*: implements regular (timer-based) statistics collection
  virtual bool collect(RPG_Net_RuntimeStatistic&) const; // return value: (currently unused !)
  virtual void report() const;

 private:
  typedef RPG_Stream_HeadModuleTaskBase<RPG_Net_ConfigPOD,
                                        RPG_Net_StreamConfig,
                                        RPG_Net_SessionMessage,
                                        RPG_Net_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_SocketHandler(const RPG_Net_Module_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_SocketHandler& operator=(const RPG_Net_Module_SocketHandler&));

  // helper methods
  bool bisectMessages(RPG_Net_Message*&); // return value: complete message (chain)
//   RPG_Net_Message* allocateMessage(const unsigned int&); // requested size
  bool putStatisticsMessage(const RPG_Net_RuntimeStatistic&, // statistics info
                            const ACE_Time_Value&) const;    // statistics generation time

  bool                               myIsInitialized;

  // timer stuff
	unsigned int                       myStatCollectionInterval; // seconds
  RPG_Net_StatisticHandler_Reactor_t myStatCollectHandler;
  long                               myStatCollectHandlerID;

  // protocol stuff
  unsigned int                       myCurrentMessageLength;
  RPG_Net_Message*                   myCurrentMessage;
  RPG_Net_Message*                   myCurrentBuffer;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Module_SocketHandler);

#endif
