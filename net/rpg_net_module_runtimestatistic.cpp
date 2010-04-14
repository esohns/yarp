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

#include "rpg_net_module_runtimestatistic.h"

#include "rpg_net_common_tools.h"

#include <rpg_common_timer_manager.h>

#include <stream_message_base.h>
#include <stream_iallocator.h>

#include <ace/Time_Value.h>

#include <iostream>

RPG_Net_Module_RuntimeStatistic::RPG_Net_Module_RuntimeStatistic()
 : myIsInitialized(false),
   myResetTimeoutHandler(this),
   myResetTimeoutHandlerID(0),
   myLocalReportingHandler(this,
                           STATISTICHANDLER_TYPE::ACTION_REPORT),
   myLocalReportingHandlerID(0),
   mySessionID(0),
   myNumTotalMessages(0),
   myNumSessionMessages(0),
   myMessageCounter(0),
   myLastMessagesPerSecondCount(0),
   myNumTotalBytes(0.0),
   myByteCounter(0),
   myLastBytesPerSecondCount(0),
// myMessageTypeStatistics.clear(),
   myAllocator(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::RPG_Net_Module_RuntimeStatistic"));

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

RPG_Net_Module_RuntimeStatistic::~RPG_Net_Module_RuntimeStatistic()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::~RPG_Net_Module_RuntimeStatistic"));

  // clean up
  fini_timers();
}

const bool
RPG_Net_Module_RuntimeStatistic::init(const unsigned long& reportingInterval_in,
                                      const Stream_IAllocator* allocator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::init"));

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

      myNumTotalMessages = 0;
      myNumSessionMessages = 0;
      myMessageCounter = 0;
      myLastMessagesPerSecondCount = 0;

      myNumTotalBytes = 0.0;
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

void
RPG_Net_Module_RuntimeStatistic::handleDataMessage(Stream_MessageBase*& message_inout,
                                                   bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    myNumTotalMessages++;
    myMessageCounter++;

    myNumTotalBytes += message_inout->total_length();
    myByteCounter += message_inout->total_length();
  } // end lock scope

  // add message to statistic...
  // --> retrieve type of message and other details...
  // *WARNING*: we silently ASSUME that upstream took care of "crunching" the data
  // for this to actually work...
  RPG_Net_MessageHeader* message_header = ACE_reinterpret_cast(RPG_Net_MessageHeader*,
                                                               message_inout->rd_ptr());
  // increment corresponding counter...
  myMessageTypeStatistics[message_header->messageType]++;
}

void
RPG_Net_Module_RuntimeStatistic::handleSessionMessage(RPG_Net_SessionMessage*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    myNumTotalMessages++;
    myNumSessionMessages++;
    myMessageCounter++;
  } // end lock scope

  switch (message_inout->getType())
  {
    case Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // remember session ID for reporting...
      mySessionID = message_inout->getConfig()->getUserData().sessionID;

      break;
    }
    case Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // session finished ? --> print overall statistics
      // *TODO*: ...and don't forget to re-init internal counters ?
      final_report();

      break;
    }
    case Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
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

void
RPG_Net_Module_RuntimeStatistic::reset()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::reset"));

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

const bool
RPG_Net_Module_RuntimeStatistic::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::collect"));

  // *NOTE*: asynchronous call: someones' using our API
  // --> fill the argument with meaningful values...

  // init return value(s)
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    data_out.numDataMessages = (myNumTotalMessages - myNumSessionMessages);
    data_out.numBytes = myNumTotalBytes;
  } // end lock scope

  return true;
}

void
RPG_Net_Module_RuntimeStatistic::report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::report"));

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
               ACE_TEXT("*** [%u] RUNTIME STATISTICS [%u] ***\n--> Stream Statistics <--\nmessages seen (last second): %u\nmessages seen (total): %u (data: %.2f %%)\ndata seen (last second): %u bytes\ndata seen (total): %.0f bytes\ncurrent cache usage [%u messages / %u total allocated heap]\n*** RUNTIME STATISTICS ***\\END\n"),
               mySessionID, mySessionID,
               myLastMessagesPerSecondCount,
               myNumTotalMessages,
               ((myNumTotalMessages - myNumSessionMessages) / 100.0),
               myLastBytesPerSecondCount,
               myNumTotalBytes,
               numMessagesOnline,
               totalHeapBytesAllocated));
//                cache_used,
//                cache_size,
//                cache_used_relative));
  } // end lock scope
}

void
RPG_Net_Module_RuntimeStatistic::final_report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::final_report"));

  {
    // synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if (myNumTotalMessages - myNumSessionMessages)
    {
      // write some output
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("*** [%u] SESSION STATISTICS [%u] ***\ntotal # data message(s) (as seen): %u\n --> Protocol Info <--\n"),
                 mySessionID, mySessionID,
                 (myNumTotalMessages - myNumSessionMessages)));

      std::string protocol_string;
      for (MESSAGETYPE2COUNT_CONSTITERATOR_TYPE iter = myMessageTypeStatistics.begin();
           iter != myMessageTypeStatistics.end();
           iter++)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": %u --> %.2f %%\n"),
                   RPG_Net_Common_Tools::messageType2String(iter->first).c_str(),
                   iter->second,
                   ACE_static_cast(double,
                                   ((iter->second * 100.0) / (myNumTotalMessages - myNumSessionMessages)))));
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

void
RPG_Net_Module_RuntimeStatistic::fini_timers(const bool& cancelAllTimers_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::fini_timers"));

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
