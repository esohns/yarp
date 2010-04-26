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

#ifndef RPG_NET_COMMON_H
#define RPG_NET_COMMON_H

#include "rpg_net_remote_comm.h"

#include <stream_streammodule.h>

#include <ace/Time_Value.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

// // timer queue
// #include <ace/Synch.h>
// #include <ace/Timer_Queue_T.h>
// #include <ace/Timer_Heap_T.h>
// #include <ace/Timer_Queue_Adapters.h>
//
// // these typedefs ensure that we use the minimal amount of locking necessary
// typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_Null_Mutex> UPCALL_TYPE;
// typedef ACE_Timer_Heap_T<ACE_Event_Handler*, UPCALL_TYPE, ACE_Null_Mutex> TIMERHEAP_TYPE;
// typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*, UPCALL_TYPE, ACE_Null_Mutex> TIMERHEAPITERATOR_TYPE;
// // typedef ACE_Thread_Timer_Queue_Adapter<TIMERHEAP_TYPE> TIMERQUEUE_TYPE;
// typedef ACE_Thread_Timer_Queue_Adapter<TIMERHEAP_TYPE> RPG_Net_TimerQueue_t;

// forward declaration(s)
class Stream_IAllocator;
template <typename ConfigType,
          typename StatisticsContainerType> class RPG_Net_Connection_Manager;
class RPG_Net_SessionMessage;
class RPG_Net_Message;
template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType> class RPG_Net_Module_RuntimeStatistic;

struct RPG_Net_RuntimeStatistic
{
  unsigned long numDataMessages; // (protocol) messages
  double        numBytes;        // amount of processed data

  // convenience
  inline RPG_Net_RuntimeStatistic operator+=(const RPG_Net_RuntimeStatistic& rhs)
  {
    numDataMessages += rhs.numDataMessages;
    numBytes += rhs.numBytes;

    return *this;
  };
};

struct RPG_Net_ConfigPOD
{
  // ************ connection config data ************
  unsigned long            clientPingInterval; // used by the server...
  int                      socketBufferSize;
  Stream_IAllocator*       messageAllocator;
  unsigned long            defaultBufferSize;
  // ************ stream config data ************
//   unsigned long connectionID;
  unsigned long            sessionID; // (== socket handle !)
  unsigned long            statisticsReportingInterval;
  // ************ runtime data ************
  RPG_Net_RuntimeStatistic currentStatistics;
  ACE_Time_Value           lastCollectionTimestamp;
};

typedef ACE_Singleton<RPG_Net_Connection_Manager<RPG_Net_ConfigPOD,
                                                 RPG_Net_RuntimeStatistic>,
                      ACE_Recursive_Thread_Mutex> RPG_NET_CONNECTIONMANAGER_SINGLETON;

// declare module(s)
typedef RPG_Net_Module_RuntimeStatistic<RPG_Net_SessionMessage,
                                        RPG_Net_Message,
                                        RPG_Net_MessageType,
                                        RPG_Net_RuntimeStatistic> RPG_NET_MODULE_RUNTIMESTATISTICS_T;
DATASTREAM_MODULE_T(RPG_NET_MODULE_RUNTIMESTATISTICS_T, // type
                    RPG_Net_Module_RuntimeStatistic);   // name

#endif
