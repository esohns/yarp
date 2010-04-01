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

#include "rpg_net_message.h"

#include <rpg_common_tools.h>

#include <stream_message_base.h>
#include <stream_iallocator.h>

// #include <ace/OS.h>
#include <ace/Time_Value.h>
// #include <ace/Date_Time.h>
// #include <ace/Message_Block.h>

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
   myNumTotalMessages(0),
   myMessageCounter(0),
   myLastMessagesPerSecondCount(0),
   myNumTotalBytes(0.0),
   myByteCounter(0),
   myLastBytesPerSecondCount(0),
// myMessageTypeStatistics.clear(),
   myAllocator(NULL),
   myPrintHashMark(false), // silent by default !
   myPrintPcapStats(false), // silent by default !
   myLocalReportingInterval(0)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::RPG_Net_Module_RuntimeStatistic"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...\n")));

  // ok: activate timer queue
  if (myTimerQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to activate() timer dispatch queue: \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

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
               ACE_OS::strerror(errno)));

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
RPG_Net_Module_RuntimeStatistic::init(const bool& printHashMark_in,
                                      const bool& printPcapStats_in,
                                      Stream_IAllocator* allocator_in,
                                      const unsigned long& reportingInterval_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("re-initializing...\n")));

    // stop reporting timer
    fini_timers(false);

    // reset various counters...
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      myNumTotalMessages = 0;
      myMessageCounter = 0;
      myLastMessagesPerSecondCount = 0;

      myNumTotalBytes = 0.0;
      myByteCounter = 0;
      myLastBytesPerSecondCount = 0;

      myMessageTypeStatistics.clear();
    } // end lock scope

    myIsInitialized = false;
  } // end IF

  myPrintHashMark = printHashMark_in;
  myPrintPcapStats = printPcapStats_in;

  // want runtime statistics reporting at regular intervals ?...
  myLocalReportingInterval = reportingInterval_in;
  if (myLocalReportingInterval)
  {
    // schedule the reporting interval timer
    int id = -1;
    ACE_Time_Value reporting_interval(myLocalReportingInterval, 0);
    id = myTimerQueue.schedule(&myLocalReportingHandler,
                               NULL,
                               ACE_OS::gettimeofday () + reporting_interval,
                               reporting_interval);
    if (id == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule() timer: \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF
    myLocalReportingHandlerID = id;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("scheduled (local) reporting timer (ID: %d) for intervals of %u second(s)...\n"),
               myLocalReportingHandlerID,
               myLocalReportingInterval));
  } // end IF
  else
  {
    // *NOTE*: even if we don't report them ourselves, we might still be triggered from
    // outside...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("(local) statistics reporting has been disabled...\n")));
  } // end IF

  myAllocator = allocator_in;
//   // sanity check(s)
//   if (!myAllocator)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("invalid argument (was NULL) --> check implementation !, aborting\n")));
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

  bool print_hash = false;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    myNumTotalMessages++;
    if ((myNumTotalMessages % 10000) == 0)
    {
      print_hash = true;
    } // end IF

    myMessageCounter++;

    myNumTotalBytes += message_inout->length();
    myByteCounter += message_inout->length();
  } // end lock scope

  // add message to statistic...
  RPG_Net_Message* message = NULL;
  message = ACE_dynamic_cast(RPG_Net_Message*,
                             message_inout);
  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Message) failed, aborting\n")));

    return;
  } // end IF
  // *NOTE*: this only makes sense if message headers have actually been parsed upstream...
  if (message->getToplevelProtocol() != RPG_Net_Protocol_Layer::INVALID_PROTOCOL)
  {
    MESSAGETYPECOUNTCONTAINER_ITERATOR_TYPE iter = myMessageTypeStatistics.end();
    iter = myMessageTypeStatistics.find(message->getToplevelProtocol());
    if (iter == myMessageTypeStatistics.end())
    {
      myMessageTypeStatistics[message->getToplevelProtocol()] = 1;
    } // end IF
    else
    {
      // increment corresponding message type counter
      iter->second++;
    } // end ELSE
  } // end IF

  if (myPrintHashMark && print_hash)
  {
    // write some output
    // *TODO*: where ?
    std::cout << '#';
  } // end IF
}

void
RPG_Net_Module_RuntimeStatistic::handleSessionMessage(Stream_SessionMessageBase<RPG_Net_StreamConfigPOD>*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, returning\n")));

    return;
  } // end IF

  switch (message_inout->getType())
  {
    case Stream_SessionMessage::SESSION_BEGIN:
    {
      // start profile timer...
      myProfile.start();

      break;
    }
    case Stream_SessionMessage::SESSION_END:
    {
      // stop profile timer...
      // *WARNING*: this cannot be completely accurate unless ALL
      // modules are synchronous. Otherwise, downstream modules still
      // need to finish handling their queued work...
      // We only put this here because it is a nice place for this kind
      // of functionality
      // --> take this with a pinch of salt !
      myProfile.stop();

      // session finished ? --> print overall statistics
      // *TODO*: ...and don't forget to re-init internal counters ?
      final_report();

      break;
    }
    case Stream_SessionMessage::SESSION_STATISTICS:
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

  // *NOTE*: this happens asynchronously whenever someone uses our RPG_Common_IStatistic API
  // --> we need to fill the argument with meaningful values...

  // init return value(s)
  data_out.messagesPerSec = 0;

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
  unsigned long cache_used = 0;
  unsigned long cache_size = 0;
  double        cache_used_relative = 0.0;
  if (myAllocator)
  {
    cache_size = myAllocator->cache_size();
    cache_used = cache_size - myAllocator->cache_depth();
//    ACE_ASSERT(cache_size);
    cache_used_relative = cache_used / ((cache_size ?
                                         cache_size : 1) * 100.0);
  } // end IF

  // ...write some output
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("*** RUNTIME STATISTICS ***\n--> Stream Statistics <--\ndata messages seen (last second): %u\ndata messages seen (total): %u\ndata seen (last second): %u bytes\ndata seen (total): %.0f bytes\ncurrent cache usage [%u/%u]: %.2f %%\n*** RUNTIME STATISTICS ***\\END\n"),
               myLastMessagesPerSecondCount,
               myNumTotalMessages,
               myLastBytesPerSecondCount,
               myNumTotalBytes,
               cache_used,
               cache_size,
               cache_used_relative));
  } // end lock scope
}

void
RPG_Net_Module_RuntimeStatistic::final_report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic::final_report"));

  {
    // *IMPORTANT NOTE*: synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // write some output
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("*** Session Statistics ***\ntotal # message(s) (as seen): %u\n --> Protocol Info <--\n"),
               myNumTotalMessages));

    // sanity check (this is not strictly necessary...)
    if (myNumTotalMessages)
    {
      std::string protocol_string;
      for (MESSAGETYPECOUNTCONTAINER_CONSTITERATOR_TYPE iter = myMessageTypeStatistics.begin();
          iter != myMessageTypeStatistics.end();
          iter++)
      {
        // create protocol string
        RPG_Net_Protocol_Layer::ProtocolLayer2String(iter->first,
                                                     protocol_string);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": %u --> %.2f %%\n"),
                   protocol_string.c_str(),
                   iter->second,
                   ACE_static_cast(double,
                                   ((iter->second * 100.0) / myNumTotalMessages))));
      } // end FOR
    } // end IF
  } // end lock scope

  // only profile stuff left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (ACE_const_cast(ACE_Profile_Timer*,
                     &myProfile)->elapsed_time(elapsed_time) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Profile_Timer::elapsed_time: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset(&elapsed_rusage,
                 0,
                 sizeof(elapsed_rusage));
  ACE_const_cast(ACE_Profile_Timer*,
                 &myProfile)->elapsed_rusage(elapsed_rusage);

  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String(user_time,
                                  user_time_string);
  RPG_Common_Tools::period2String(system_time,
                                  system_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Session Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str(),
             elapsed_rusage.ru_maxrss,
             elapsed_rusage.ru_ixrss,
             elapsed_rusage.ru_idrss,
             elapsed_rusage.ru_isrss,
             elapsed_rusage.ru_minflt,
             elapsed_rusage.ru_majflt,
             elapsed_rusage.ru_nswap,
             elapsed_rusage.ru_inblock,
             elapsed_rusage.ru_oublock,
             elapsed_rusage.ru_msgsnd,
             elapsed_rusage.ru_msgrcv,
             elapsed_rusage.ru_nsignals,
             elapsed_rusage.ru_nvcsw,
             elapsed_rusage.ru_nivcsw));

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
                  ACE_OS::strerror(errno)));
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
                 ACE_OS::strerror(errno)));
    } // end IF

    myLocalReportingHandlerID = 0;
  } // end IF
}
