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

#include <stream_message_base.h>
#include <stream_iallocator.h>

#include <ace/Time_Value.h>

#include <iostream>

RPG_Net_Module_RuntimeStatistic::RPG_Net_Module_RuntimeStatistic()
 : myIsInitialized(false),
   myTimerQueue(NULL,
                NULL),
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

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...\n")));

  // ok: activate timer queue
  if (myTimerQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to activate() timer dispatch queue: \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...DONE\n")));

  // schedule the second-granularity timer
  ACE_Time_Value second_interval(1, 0); // one second interval
  int id = -1;
  id = myTimerQueue.schedule(&myResetTimeoutHandler,
                             NULL,
                             ACE_OS::gettimeofday () + second_interval,
                             second_interval);
  if (id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to schedule() timer: \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return;
  } // end IF
  myResetTimeoutHandlerID = id;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("scheduled second-interval timer (ID: %d)...\n"),
//              myResetTimeoutHandlerID));
}

RPG_Net_Module_RuntimeStatistic::~RPG_Net_Module_RuntimeStatistic()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::~RPG_Net_Module_RuntimeStatistic"));

  // clean up
  fini_timers();

  myTimerQueue.deactivate();
  // make sure the dispatcher thread is really dead...
  myTimerQueue.wait();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivated timers and dispatcher...\n")));
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
    int id = -1;
    ACE_Time_Value reporting_interval(reportingInterval_in, 0);
    id = myTimerQueue.schedule(&myLocalReportingHandler,
                               NULL,
                               ACE_OS::gettimeofday () + reporting_interval,
                               reporting_interval);
    if (id == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule() timer: \"%s\", aborting\n"),
                 ACE_OS::strerror(ACE_OS::last_error())));

      return false;
    } // end IF
    myLocalReportingHandlerID = id;

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("scheduled (local) reporting timer (ID: %d) for intervals of %u second(s)...\n"),
//                myLocalReportingHandlerID,
//                myLocalReportingInterval));
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

      // start profile timer...
//       myProfile.start();

      break;
    }
    case Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // stop profile timer...
      // *WARNING*: this cannot be completely accurate unless ALL
      // modules are synchronous. Otherwise, downstream modules still
      // need to finish handling their queued work...
      // We only put this here because it is a nice place for this kind
      // of functionality
      // --> take this information with a grain of salt !
//       myProfile.stop();


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
RPG_Net_Module_RuntimeStatistic::collect(RPG_Net_RuntimeStatistic& data_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::collect"));

  // *NOTE*: this happens asynchronously whenever someone uses our
  // RPG_Common_IStatistic API
  // --> we need to fill the argument with meaningful values...

  // init return value(s)
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, aborting\n")));

    return false;
  } // end IF

  // *TODO*: do something meaningful here...
  ACE_ASSERT(false);

  // OK: all is well...
  return true;
}

void
RPG_Net_Module_RuntimeStatistic::report()
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
    // *NOTE*: synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if (myNumTotalMessages)
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
  } // end lock scope

//   // only profile stuff left to do...
//   ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
//   elapsed_time.real_time = 0.0;
//   elapsed_time.user_time = 0.0;
//   elapsed_time.system_time = 0.0;
//   if (ACE_const_cast(ACE_Profile_Timer*,
//                      &myProfile)->elapsed_time(elapsed_time) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_Profile_Timer::elapsed_time: \"%s\", returning\n"),
//                ACE_OS::strerror(ACE_OS::last_error())));
//
//     return;
//   } // end IF
//   ACE_Profile_Timer::Rusage elapsed_rusage;
//   ACE_OS::memset(&elapsed_rusage,
//                  0,
//                  sizeof(elapsed_rusage));
//   ACE_const_cast(ACE_Profile_Timer*,
//                  &myProfile)->elapsed_rusage(elapsed_rusage);
//
//   ACE_Time_Value user_time(elapsed_rusage.ru_utime);
//   ACE_Time_Value system_time(elapsed_rusage.ru_stime);
//   std::string user_time_string;
//   std::string system_time_string;
//   RPG_Common_Tools::period2String(user_time,
//                                   user_time_string);
//   RPG_Common_Tools::period2String(system_time,
//                                   system_time_string);
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" --> Session Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
//              elapsed_time.real_time,
//              elapsed_time.user_time,
//              elapsed_time.system_time,
//              user_time_string.c_str(),
//              system_time_string.c_str(),
//              elapsed_rusage.ru_maxrss,
//              elapsed_rusage.ru_ixrss,
//              elapsed_rusage.ru_idrss,
//              elapsed_rusage.ru_isrss,
//              elapsed_rusage.ru_minflt,
//              elapsed_rusage.ru_majflt,
//              elapsed_rusage.ru_nswap,
//              elapsed_rusage.ru_inblock,
//              elapsed_rusage.ru_oublock,
//              elapsed_rusage.ru_msgsnd,
//              elapsed_rusage.ru_msgrcv,
//              elapsed_rusage.ru_nsignals,
//              elapsed_rusage.ru_nvcsw,
//              elapsed_rusage.ru_nivcsw));

  //double messages_per_sec = double (message_count) / et.real_time;
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("\t\tmessages = %d\n\t\ttotal bytes = %d\n\t\tmbits/sec = %f\n\t\tusec-per-message = %f\n\t\tmessages-per-second = %0.00f\n"),
  //            message_count,
  //            total_bytes,
  //            (((double) total_bytes * 8) / et.real_time) / (double) (1024 * 1024),
  //            (et.real_time / (double) message_count) * 1000000,
  //            messages_per_sec < 0 ? 0 : messages_per_sec));
}

void
RPG_Net_Module_RuntimeStatistic::fini_timers(const bool& cancelAllTimers_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::fini_timers"));

  if (cancelAllTimers_in)
  {
    if (myResetTimeoutHandlerID)
    {
      if (myTimerQueue.cancel(myResetTimeoutHandlerID) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to cancel() timer: \"%s\", continuing\n"),
                  ACE_OS::strerror(ACE_OS::last_error())));
      } // end IF

      myResetTimeoutHandlerID = 0;
    } // end IF
  } // end IF

  if (myLocalReportingHandlerID)
  {
    if (myTimerQueue.cancel(myLocalReportingHandlerID) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel() timer: \"%s\", continuing\n"),
                 ACE_OS::strerror(ACE_OS::last_error())));
    } // end IF

    myLocalReportingHandlerID = 0;
  } // end IF
}
