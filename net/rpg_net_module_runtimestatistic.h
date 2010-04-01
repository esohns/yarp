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

#ifndef RPG_NET_MODULE_RUNTIMESTATISTIC_H
#define RPG_NET_MODULE_RUNTIMESTATISTIC_H

#include "rpg_net_common.h"
#include "rpg_net_icounter.h"
#include "rpg_net_statistichandler.h"
#include "rpg_net_resetcounterhandler.h"
#include "rpg_net_protocol_layer.h"

#include <rpg_common_istatistic.h>

#include <stream_task_base_synch.h>
#include <stream_streammodule_base.h>
#include <stream_session_message_base.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>
#include <ace/Timer_Queue_T.h>
#include <ace/Timer_Heap_T.h>
#include <ace/Timer_Queue_Adapters.h>
#include <ace/Profile_Timer.h>

#include <map>

// forward declaration(s)
class Stream_MessageBase;
class Stream_IAllocator;

class RPG_Net_Module_RuntimeStatistic
 : public Stream_TaskBaseSynch<RPG_Net_StreamConfigPOD>,
   public RPG_Net_ICounter,
   public RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>
{
 public:
  RPG_Net_Module_RuntimeStatistic();
  virtual ~RPG_Net_Module_RuntimeStatistic();

  // initialization
  const bool init(const bool&,           // print hash ("#") mark for every 1000 messages seen to stdout
                  const bool&,           // print libpcap-related stats
                  // *TODO*: just pass an augmented ACE_Allocator interface !
                  // *NOTE*: if this is non-NULL, cache usage data will be reported !
                  Stream_IAllocator*,
                  const unsigned long&); // (local) reporting interval [seconds: 0 --> OFF]

  // implement (part of) Stream_ITask
  virtual void handleDataMessage(Stream_MessageBase*&, // data message handle
                                 bool&);            // return value: pass message downstream ?

  // implement this so we can print overall statistics after session completes...
  virtual void handleSessionMessage(Stream_SessionMessageBase<RPG_Net_StreamConfigPOD>*&, // session message handle
                                    bool&);                                               // return value: pass message downstream ?

  // implement RPG_Net_ICounter
  virtual void reset();

  // implement RPG_Common_IStatistic
  virtual const bool collect(RPG_Net_RuntimeStatistic&); // return value: info
  // *NOTE*: this also implements locally triggered reporting !
  virtual void report();

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_StreamConfigPOD> inherited;

  // these typedefs ensure that we use the minimal amount of locking necessary
  typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_Null_Mutex> UPCALL_TYPE;
  typedef ACE_Timer_Heap_T<ACE_Event_Handler*,
                           UPCALL_TYPE,
                           ACE_Null_Mutex> TIMERHEAP_TYPE;
  typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*,
                                    UPCALL_TYPE,
                                    ACE_Null_Mutex> TIMERHEAPITERATOR_TYPE;
  typedef ACE_Thread_Timer_Queue_Adapter<TIMERHEAP_TYPE> TIMERQUEUE_TYPE;

  // message type counters
  typedef std::map<RPG_Net_Protocol_t,
                   unsigned long> MESSAGETYPECOUNTCONTAINER_TYPE;
  typedef MESSAGETYPECOUNTCONTAINER_TYPE::const_iterator MESSAGETYPECOUNTCONTAINER_CONSTITERATOR_TYPE;
  typedef MESSAGETYPECOUNTCONTAINER_TYPE::iterator MESSAGETYPECOUNTCONTAINER_ITERATOR_TYPE;

  // convenience types
  typedef RPG_Net_RuntimeStatistic STATISTICINTERFACE_TYPE;
  typedef RPG_Net_StatisticHandler<STATISTICINTERFACE_TYPE> STATISTICHANDLER_TYPE;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_RuntimeStatistic(const RPG_Net_Module_RuntimeStatistic&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_RuntimeStatistic& operator=(const RPG_Net_Module_RuntimeStatistic&));

  // helper method(s)
  void final_report() const;
  void fini_timers(const bool& = true); // cancel both timers ? (false --> cancel only myLocalReportingHandlerID)

  bool                               myIsInitialized;

  // timer stuff
  TIMERQUEUE_TYPE                    myTimerQueue;
  RPG_Net_ResetCounterHandler        myResetTimeoutHandler;
  int                                myResetTimeoutHandlerID;
  STATISTICHANDLER_TYPE              myLocalReportingHandler;
  int                                myLocalReportingHandlerID;

  // *GENERIC STATS*
  mutable ACE_Thread_Mutex           myLock;

  unsigned long                      myNumTotalMessages;
  // used to compute message throughput...
  unsigned long                      myMessageCounter;
  // *IMPORTANT NOTE: support asynchronous collecting/reporting of data...
  unsigned long                      myLastMessagesPerSecondCount;

  double                             myNumTotalBytes;
  // used to compute data throughput...
  unsigned long                      myByteCounter;
  // *IMPORTANT NOTE: support asynchronous collecting/reporting of data...
  unsigned long                      myLastBytesPerSecondCount;

  // *MESSAGE TYPE STATS*
  MESSAGETYPECOUNTCONTAINER_TYPE     myMessageTypeStatistics;

  // *CACHE STATS*
  Stream_IAllocator*                myAllocator;

  // *PROCESS PROFILE*
  ACE_Profile_Timer                  myProfile;

  bool                               myPrintHashMark;
  bool                               myPrintPcapStats;
  unsigned long                      myLocalReportingInterval;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Module_RuntimeStatistic);
#endif
