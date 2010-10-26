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

#include "rpg_net_common_tools.h"

#include <rpg_common_timer_manager.h>

#include <rpg_stream_message_base.h>
#include <rpg_stream_iallocator.h>

#include <ace/Time_Value.h>

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::RPG_Net_Module_RuntimeStatistic()
 : myIsInitialized(false),
   myResetTimeoutHandler(this),
   myResetTimeoutHandlerID(0),
   myLocalReportingHandler(this,
                           STATISTICHANDLER_TYPE::ACTION_REPORT),
   myLocalReportingHandlerID(0),
   mySessionID(0),
   myNumInboundMessages(0),
   myNumOutboundMessages(0),
   myNumSessionMessages(0),
   myMessageCounter(0),
   myLastMessagesPerSecondCount(0),
   myNumInboundBytes(0.0),
   myByteCounter(0),
   myLastBytesPerSecondCount(0),
// myMessageTypeStatistics.clear(),
   myAllocator(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::RPG_Net_Module_RuntimeStatistic"));

  // schedule the second-granularity timer
  ACE_Time_Value second_interval(1, 0); // one second interval
  if (!RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->scheduleTimer(myResetTimeoutHandler,    // handler
                                                                    second_interval,          // interval
                                                                    false,                    // one-shot ?
                                                                    myResetTimeoutHandlerID)) // return value: timer ID
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Timer_Manager::scheduleTimer(%u), aborting\n"),
               1));

    // what else can we do ?
    return;
  } // end IF
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("scheduled second-interval timer (ID: %d)...\n"),
//              myResetTimeoutHandlerID));
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::~RPG_Net_Module_RuntimeStatistic()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::~RPG_Net_Module_RuntimeStatistic"));

  // clean up
  fini_timers();
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
const bool
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::init(const unsigned long& reportingInterval_in,
                                                               const RPG_Stream_IAllocator* allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // stop reporting timer
    fini_timers(false);

    mySessionID = 0;
    // reset various counters...
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      myNumInboundMessages = 0;
      myNumOutboundMessages = 0;
      myNumSessionMessages = 0;
      myMessageCounter = 0;
      myLastMessagesPerSecondCount = 0;

      myNumInboundBytes = 0.0;
      myByteCounter = 0;
      myLastBytesPerSecondCount = 0;

      myMessageTypeStatistics.clear();
    } // end lock scope

    myIsInitialized = false;
  } // end IF

  // want runtime statistics reporting at regular intervals ?...
  if (reportingInterval_in)
  {
    // schedule the reporting interval timer
    ACE_Time_Value reporting_interval(reportingInterval_in, 0);
    if (!RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->scheduleTimer(myLocalReportingHandler,    // handler
                                                                      reporting_interval,         // interval
                                                                      false,                      // one-shot ?
                                                                      myLocalReportingHandlerID)) // return value: timer ID
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_Timer_Manager::scheduleTimer(%u), aborting\n"),
                 reportingInterval_in));

      // what else can we do ?
      return false;
    } // end IF
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("scheduled (local) reporting timer (ID: %d) for intervals of %u second(s)...\n"),
//                myLocalReportingHandlerID,
//                reportingInterval_in));
  } // end IF
  else
  {
    // *NOTE*: even if we don't report them ourselves, we might still be triggered from
    // outside...
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("(local) statistics reporting has been disabled...\n")));
  } // end IF

  myAllocator = allocator_in;
//   // sanity check(s)
//   if (!myAllocator)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("invalid argument (was NULL), aborting\n")));
//
//     return false;
//   } // end IF

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::handleDataMessage(ProtocolMessageType*& message_inout,
                                                                            bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    if (inherited::is_writer())
    {
      myNumInboundMessages++;
      myNumInboundBytes += message_inout->total_length();
      myByteCounter += message_inout->total_length();
    } // end IF
    else
      myNumOutboundMessages++;
    myMessageCounter++;
  } // end lock scope

  // add message to statistic...
  // --> increment corresponding counter
  myMessageTypeStatistics[static_cast<ProtocolCommandType> (message_inout->getCommand())]++;
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::handleSessionMessage(SessionMessageType*& message_inout,
                                                                               bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    // *NOTE*: currently, session messages travel only downstream...
    myNumInboundMessages++;
    myNumSessionMessages++;
    myMessageCounter++;
  } // end lock scope

  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // remember session ID for reporting...
      mySessionID = message_inout->getConfig()->getUserData().sessionID;

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // session finished ? --> print overall statistics
      // *TODO*: ...and don't forget to re-init internal counters ?
      final_report();

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
    {
//       // *NOTE*: protect access to statistics data
//       // from asynchronous API calls (as well as local reporting)...
//       {
//         ACE_Guard<ACE_Thread_Mutex> aGuard(myPcapStatsLock);
//
//         myCurrentPcapStats = message_inout->getConfig()->getPcapStats();
//
//         // remember previous timestamp (so we can satisfy our asynchronous API)...
//         myLastPcapStatsTimestamp = myCurrentPcapStatsTimestamp;
//
//         myCurrentPcapStatsTimestamp = message_inout->getConfig()->getStatGenerationTime();
//       } // end lock scope

      break;
    }
    default:
    {
      // don't do anything...
      break;
    }
  } // end SWITCH
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::reset()
{
//   RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::reset"));

  // this should happen every second (roughly)...
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // remember this result (so we can satisfy our asynchronous API)...
    myLastMessagesPerSecondCount = myMessageCounter;
    myLastBytesPerSecondCount = myByteCounter;

    // reset counters
    myMessageCounter = 0;
    myByteCounter = 0;
  } // end lock scope
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
const bool
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::collect(StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::collect"));

  // *NOTE*: asynchronous call: someones' using our API
  // --> fill the argument with meaningful values...

  // init return value(s)
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(StatisticsContainerType));

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    data_out.numDataMessages = ((myNumInboundMessages + myNumOutboundMessages) - myNumSessionMessages);
    data_out.numBytes = myNumInboundBytes;
  } // end lock scope

  return true;
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::report"));

  // compute cache usage...
//   unsigned long cache_used = 0;
//   unsigned long cache_size = 0;
//   double        cache_used_relative = 0.0;
  unsigned long numMessagesOnline = 0;
  unsigned long totalHeapBytesAllocated = 0;
  if (myAllocator)
  {
    numMessagesOnline = myAllocator->cache_depth();
    totalHeapBytesAllocated = myAllocator->cache_size();
//    ACE_ASSERT(cache_size);
//     cache_used_relative = cache_used / ((cache_size ?
//                                          cache_size : 1) * 100.0);
  } // end IF

  // ...write some output
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("*** [%u] RUNTIME STATISTICS [%u] ***\n--> Stream Statistics <--\nmessages seen (last second): %u\nmessages seen (total [in/out]): %u/%u (data: %.2f %%)\n(inbound) data seen (last second): %u bytes\n(inbound)  data seen (total): %.0f bytes\ncurrent cache usage [%u messages / %u total allocated heap]\n*** RUNTIME STATISTICS ***\\END\n"),
               mySessionID, mySessionID,
               myLastMessagesPerSecondCount,
               myNumInboundMessages, myNumOutboundMessages,
               (((myNumInboundMessages + myNumOutboundMessages) - myNumSessionMessages) / 100.0),
               myLastBytesPerSecondCount,
               myNumInboundBytes,
               numMessagesOnline,
               totalHeapBytesAllocated));
//                cache_used,
//                cache_size,
//                cache_used_relative));
  } // end lock scope
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::final_report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::final_report"));

  {
    // synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if ((myNumInboundMessages + myNumOutboundMessages) - myNumSessionMessages)
    {
      // write some output
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("*** [%u] SESSION STATISTICS [%u] ***\ntotal # data message(s) (as seen [in/out]): %u/%u\n --> Protocol Info <--\n"),
                 mySessionID, mySessionID,
                 (myNumInboundMessages - myNumSessionMessages),
                 myNumOutboundMessages));

      std::string protocol_string;
      for (MESSAGETYPE2COUNT_CONSTITERATOR_TYPE iter = myMessageTypeStatistics.begin();
           iter != myMessageTypeStatistics.end();
           iter++)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": %u --> %.2f %%\n"),
                   ProtocolMessageType::commandType2String(iter->first).c_str(),
                   iter->second,
                   static_cast<double> (((iter->second * 100.0) / (myNumInboundMessages - myNumSessionMessages)))));
      } // end FOR
    } // end IF

//     double messages_per_sec = double (message_count) / et.real_time;
//     ACE_DEBUG ((LM_DEBUG,
//                 ACE_TEXT ("\t\tmessages = %d\n\t\ttotal bytes = %d\n\t\tmbits/sec = %f\n\t\tusec-per-message = %f\n\t\tmessages-per-second = %0.00f\n"),
//                 message_count,
//                 total_bytes,
//                 (((double) total_bytes * 8) / et.real_time) / (double) (1024 * 1024),
//                 (et.real_time / (double) message_count) * 1000000,
//                 messages_per_sec < 0 ? 0 : messages_per_sec));
  } // end lock scope
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic<SessionMessageType,
                                ProtocolMessageType,
                                ProtocolCommandType,
                                StatisticsContainerType>::fini_timers(const bool& cancelAllTimers_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::fini_timers"));

  if (cancelAllTimers_in)
  {
    if (myResetTimeoutHandlerID)
    {
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myResetTimeoutHandlerID);
      myResetTimeoutHandlerID = 0;
    } // end IF
  } // end IF

  if (myLocalReportingHandlerID)
  {
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myLocalReportingHandlerID);
    myLocalReportingHandlerID = 0;
  } // end IF
}
