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

#include "rpg_net_defines.h"
#include "rpg_net_icounter.h"
#include "rpg_net_statistichandler.h"
#include "rpg_net_resetcounterhandler.h"

#include <rpg_common_istatistic.h>

#include <rpg_stream_task_base_synch.h>
#include <rpg_stream_streammodule_base.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <set>
#include <map>

// forward declaration(s)
class RPG_Stream_IAllocator;

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
class RPG_Net_Module_RuntimeStatistic
 : public RPG_Stream_TaskBaseSynch<SessionMessageType,
                                   ProtocolMessageType>,
   public RPG_Net_ICounter,
   public RPG_Common_IStatistic<StatisticsContainerType>
{
 public:
  RPG_Net_Module_RuntimeStatistic();
  virtual ~RPG_Net_Module_RuntimeStatistic();

  // initialization
  bool init(const unsigned int& = RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL, // (local) reporting interval [seconds: 0 --> OFF]
            const RPG_Stream_IAllocator* = NULL);                            // report cache usage ?

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(ProtocolMessageType*&, // data message handle
                                 bool&);            // return value: pass message downstream ?

  // implement this so we can print overall statistics after session completes...
  virtual void handleSessionMessage(SessionMessageType*&, // session message handle
                                    bool&);               // return value: pass message downstream ?

  // implement RPG_Net_ICounter
  virtual void reset();

  // implement RPG_Common_IStatistic
  virtual bool collect(StatisticsContainerType&) const; // return value: info
  // *NOTE*: this also implements locally triggered reporting !
  virtual void report() const;

 private:
  typedef RPG_Stream_TaskBaseSynch<SessionMessageType,
                                   ProtocolMessageType> inherited;

  // message type counters
  typedef std::set<ProtocolCommandType> MESSAGETYPECONTAINER_TYPE;
  typedef typename MESSAGETYPECONTAINER_TYPE::const_iterator MESSAGETYPECONTAINER_CONSTITERATOR_TYPE;
  typedef std::map<ProtocolCommandType,
                   unsigned long> MESSAGETYPE2COUNT_TYPE;
  typedef std::pair<ProtocolCommandType,
                    unsigned long> MESSAGETYPE2COUNTPAIR_TYPE;
  typedef typename MESSAGETYPE2COUNT_TYPE::const_iterator MESSAGETYPE2COUNT_CONSTITERATOR_TYPE;

  // convenience types
  typedef StatisticsContainerType STATISTICINTERFACE_TYPE;
  typedef RPG_Net_StatisticHandler<STATISTICINTERFACE_TYPE> STATISTICHANDLER_TYPE;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_RuntimeStatistic(const RPG_Net_Module_RuntimeStatistic&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_RuntimeStatistic& operator=(const RPG_Net_Module_RuntimeStatistic&));

  // helper method(s)
  void final_report() const;
  void fini_timers(const bool& = true); // cancel both timers ? (false --> cancel only myLocalReportingHandlerID)

  bool                               myIsInitialized;

  // timer stuff
//   RPG_Net_TimerQueue_t               myTimerQueue;
  RPG_Net_ResetCounterHandler        myResetTimeoutHandler;
  int                                myResetTimeoutHandlerID;
  STATISTICHANDLER_TYPE              myLocalReportingHandler;
  int                                myLocalReportingHandlerID;

  // *GENERIC STATS*
  mutable ACE_Thread_Mutex           myLock;
  unsigned int                       mySessionID;

  // *NOTE*: data messages == (myNumTotalMessages - myNumSessionMessages)
  unsigned int                       myNumInboundMessages;
  unsigned int                       myNumOutboundMessages;
  unsigned int                       myNumSessionMessages;
  // used to compute message throughput...
  unsigned int                       myMessageCounter;
  // *NOTE: support asynchronous collecting/reporting of data...
  unsigned int                       myLastMessagesPerSecondCount;

  double                             myNumInboundBytes;
  // used to compute data throughput...
  unsigned int                       myByteCounter;
  // *NOTE: support asynchronous collecting/reporting of data...
  unsigned int                       myLastBytesPerSecondCount;

  // *MESSAGE TYPE STATS*
  MESSAGETYPE2COUNT_TYPE             myMessageTypeStatistics;

  // *CACHE STATS*
  const RPG_Stream_IAllocator*       myAllocator;
};

// // declare module
// DATASTREAM_MODULE_WRITER_ONLY_T(RPG_Net_Module_RuntimeStatistic<SessionMessageType>, // type
//                                 RPG_Net_Module_RuntimeStatistic);                    // name

// include template implementation
#include "rpg_net_module_runtimestatistic.i"

#endif
