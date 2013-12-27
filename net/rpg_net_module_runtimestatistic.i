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

#include <ace/Time_Value.h>

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_message_base.h"
#include "rpg_stream_iallocator.h"

#include "rpg_net_common_tools.h"

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::RPG_Net_Module_RuntimeStatistic_t()
 : inherited(),
   myIsInitialized(false),
   myResetTimeoutHandler(this),
   myResetTimeoutHandlerID(-1),
   myLocalReportingHandler(this,
                           STATISTICHANDLER_TYPE::ACTION_REPORT),
   myLocalReportingHandlerID(-1),
   mySessionID(0),
   myNumInboundMessages(0),
   myNumOutboundMessages(0),
   myNumSessionMessages(0),
   myMessageCounter(0),
   myLastMessagesPerSecondCount(0),
   myNumInboundBytes(0.0F),
   myByteCounter(0),
   myLastBytesPerSecondCount(0),
// myMessageTypeStatistics.clear(),
   myAllocator(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::RPG_Net_Module_RuntimeStatistic_t"));

  // schedule the second-granularity timer
  ACE_Time_Value second_interval(1, 0); // one second interval
  myResetTimeoutHandlerID = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&myResetTimeoutHandler,                    // handler
                                                                                    NULL,                                      // act
                                                                                    ACE_OS::gettimeofday () + second_interval, // wakeup time
                                                                                    second_interval);                          // interval
  if (myResetTimeoutHandlerID == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));

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
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::~RPG_Net_Module_RuntimeStatistic_t()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::~RPG_Net_Module_RuntimeStatistic_t"));

  // clean up
  fini_timers(true);
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
bool
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::init(const unsigned int& reportingInterval_in,
                                                                 const RPG_Stream_IAllocator* allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::init"));

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

      myNumInboundBytes = 0.0F;
      myByteCounter = 0;
      myLastBytesPerSecondCount = 0;

      myMessageTypeStatistics.clear();
    } // end lock scope

    myIsInitialized = false;
  } // end IF

  // statistics reporting
  if (reportingInterval_in)
  {
    // schedule the reporting interval timer
    ACE_Time_Value reporting_interval(reportingInterval_in, 0);
    ACE_ASSERT(myLocalReportingHandlerID == -1);
    myLocalReportingHandlerID = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&myLocalReportingHandler,                     // handler
                                                                                        NULL,                                         // act
                                                                                        ACE_OS::gettimeofday () + reporting_interval, // wakeup time
                                                                                        reporting_interval);                          // interval
    if (myLocalReportingHandlerID == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));

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
    // *NOTE*: even if this doesn't report, it might still be triggered from outside...
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
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::handleDataMessage(ProtocolMessageType*& message_inout,
                                                                              bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update counters...
    myNumInboundMessages++;
    myNumInboundBytes += message_inout->total_length();
    myByteCounter += message_inout->total_length();

    myMessageCounter++;

	// add message to statistic...
    // --> increment corresponding counter
    myMessageTypeStatistics[static_cast<ProtocolCommandType>(message_inout->getCommand())]++;
  } // end lock scope
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::handleSessionMessage(SessionMessageType*& message_inout,
                                                                                 bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::handleSessionMessage"));

  // don't care (implies yes per default)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // update our counters...
    // *NOTE*: currently, session messages travel only downstream...
    //myNumInboundMessages++;
    myNumSessionMessages++;
    //myMessageCounter++;
  } // end lock scope

  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // retain session ID for reporting...
      mySessionID = message_inout->getConfig()->getUserData().sessionID;

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // session finished ? --> print overall statistics
      // *TODO*: re-init internal counters ?
      final_report();

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
    {
//       // *NOTE*: protect access to statistics data
//       // from asynchronous API calls (as well as local reporting)...
//       {
//         ACE_Guard<ACE_Thread_Mutex> aGuard(myStatsLock);
//
//         myCurrentStats = message_inout->getConfig()->getStats();
//
//         // remember previous timestamp (so we can satisfy our asynchronous API)...
//         myLastStatsTimestamp = myCurrentStatsTimestamp;
//
//         myCurrentStatsTimestamp = message_inout->getConfig()->getStatGenerationTime();
//       } // end lock scope

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
void
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::reset()
{
//   RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::reset"));

  // this should happen every second (roughly)...
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // remember this result (satisfies an asynchronous API)...
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
bool
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::collect(StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::collect"));

  // *NOTE*: asynchronous call: someones' using our API
  // --> fill the argument with meaningful values...

  // init return value(s)
  ACE_OS::memset(&data_out, 0, sizeof(StatisticsContainerType));

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
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::report"));

  // compute cache usage...
//   unsigned int cache_used = 0;
//   unsigned int cache_size = 0;
//   double        cache_used_relative = 0.0;
  unsigned int numMessagesOnline = 0;
  unsigned int totalHeapBytesAllocated = 0;
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
               ACE_TEXT("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics <--\nmessages seen (last second): %u\nmessages seen (total [in/out]): %u/%u (data: %.2f %%)\n(inbound) data seen (last second): %u bytes\n(inbound)  data seen (total): %.0f bytes\ncurrent cache usage [%u messages / %u total allocated heap]\n*** RUNTIME STATISTICS ***\\END\n"),
               mySessionID,
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
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::final_report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::final_report"));

  {
    // synchronize access to statistics data
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if ((myNumInboundMessages + myNumOutboundMessages))
    {
      // write some output
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("*** [session: %u] SESSION STATISTICS ***\ntotal # data message(s) (as seen [in/out]): %u/%u\n --> Protocol Info <--\n"),
                 mySessionID,
                 myNumInboundMessages,
                 myNumOutboundMessages));

      std::string protocol_string;
      for (MESSAGETYPE2COUNT_CONSTITERATOR_TYPE iterator = myMessageTypeStatistics.begin();
           iterator != myMessageTypeStatistics.end();
           iterator++)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": %u --> %.2f %%\n"),
                   ProtocolMessageType::commandType2String(iterator->first).c_str(),
                   iterator->second,
                   static_cast<double>(((iterator->second * 100.0) / myNumInboundMessages))));
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
RPG_Net_Module_RuntimeStatistic_t<SessionMessageType,
                                  ProtocolMessageType,
                                  ProtocolCommandType,
                                  StatisticsContainerType>::fini_timers(const bool& cancelAllTimers_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatistic_t::fini_timers"));

  if (cancelAllTimers_in)
  {
    if (myResetTimeoutHandlerID != -1)
    {
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myResetTimeoutHandlerID) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   myResetTimeoutHandlerID));
      myResetTimeoutHandlerID = -1;
    } // end IF
  } // end IF

  if (myLocalReportingHandlerID != -1)
  {
    if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myLocalReportingHandlerID) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                 myLocalReportingHandlerID));
    myLocalReportingHandlerID = -1;
  } // end IF
}

// ----------------------------------------------------------------------------

template <typename SessionMessageType,
          typename ProtocolMessageType,
		  typename ProtocolCommandType,
          typename StatisticsContainerType>
RPG_Net_Module_RuntimeStatisticReader_t<SessionMessageType,
                                        ProtocolMessageType,
										ProtocolCommandType,
                                        StatisticsContainerType>::RPG_Net_Module_RuntimeStatisticReader_t()
 : inherited()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatisticReader_t::RPG_Net_Module_RuntimeStatisticReader_t"));

  inherited::flags_ |= ACE_Task_Flags::ACE_READER;
}

template <typename SessionMessageType,
          typename ProtocolMessageType,
		  typename ProtocolCommandType,
          typename StatisticsContainerType>
RPG_Net_Module_RuntimeStatisticReader_t<SessionMessageType,
                                        ProtocolMessageType,
										ProtocolCommandType,
                                        StatisticsContainerType>::~RPG_Net_Module_RuntimeStatisticReader_t()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatisticReader_t::~RPG_Net_Module_RuntimeStatisticReader_t"));

}

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType>
int
RPG_Net_Module_RuntimeStatisticReader_t<SessionMessageType,
                                        ProtocolMessageType,
                                        ProtocolCommandType,
                                        StatisticsContainerType>::put(ACE_Message_Block* mb_in,
                                                                      ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_RuntimeStatisticReader_t::put"));

  // pass the message to the sibling
  ACE_Task_Base* sibling_task = sibling();
  if (!sibling_task)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no sibling task: \"%m\", aborting\n")));

	return -1;
  } // end IF
  TASK_TYPE* stream_task = dynamic_cast<TASK_TYPE*>(sibling_task);
  if (!stream_task)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to dynamic_cast<RPG_Net_Module_RuntimeStatistic_t>: \"%m\", aborting\n")));

	return -1;
  } // end IF
  ProtocolMessageType* message = dynamic_cast<MESSAGE_TYPE*>(mb_in);
  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to dynamic_cast<RPG_Net_Message>: \"%m\", aborting\n")));

	return -1;
  } // end IF

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(stream_task->myLock);

    // update counters...
	  stream_task->myNumOutboundMessages++;
		stream_task->myByteCounter += mb_in->total_length();

//	// add message to statistic...
//	// --> increment corresponding counter
//	stream_task->myMessageTypeStatistics[static_cast<COMMAND_TYPE>(message->getCommand())]++;
  } // end lock scope

  return inherited::put(mb_in, tv_in);
}
