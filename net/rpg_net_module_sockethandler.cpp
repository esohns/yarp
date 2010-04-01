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

#include "rpg_net_module_sockethandler.h"

// #include "rps_flb_common.h"
// #include "rps_flb_common_tools.h"

#include "rpg_net_message.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_stream_config.h"
// #include "rps_flb_common_message_scheduler.h"

#include <stream_iallocator.h>

// #include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
// #include <ace/Message_Block.h>

RPG_Net_Module_SocketHandler::RPG_Net_Module_SocketHandler()
 : inherited(false), // DON'T auto-start !
   myIsInitialized(false),
   myStatCollectHandler(this,
                        STATISTICHANDLER_TYPE::ACTION_COLLECT),
   myStatCollectHandlerID(0),
   myAllocator(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::RPG_Net_Module_SocketHandler"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...\n")));

  // ok: activate timer queue
  if (myTimerQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to activate() timer dispatch queue: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...DONE\n")));
}

RPG_Net_Module_SocketHandler::~RPG_Net_Module_SocketHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::~RPG_Net_Module_SocketHandler"));

  // clean up
  if (myStatCollectHandlerID)
  {
    if (myTimerQueue.cancel(myStatCollectHandlerID) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel statistics collection timer (ID: %u): \"%s\", continuing\n"),
                 myStatCollectHandlerID,
                 ACE_OS::strerror(errno)));
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("deactivated statistics collection timer (ID: %u)...\n"),
               myStatCollectHandlerID));

    myStatCollectHandlerID = 0;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...\n")));

  myTimerQueue.deactivate();
  // make sure the dispatcher thread is really dead...
  myTimerQueue.wait();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...DONE\n")));
}

const bool
RPG_Net_Module_SocketHandler::init(Stream_IAllocator* messageAllocator_in,
                                   const std::string& networkInterface_in,
                                   const unsigned long& sizeReceiveBuffer_in,
                                   const unsigned long& statCollectInterval_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("already initialized --> nothing to do, aborting\n")));

    return false;
  } // end IF

  // sanity check(s)
  if (!messageAllocator_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was NULL), aborting\n")));

    return false;
  } // end IF

  myAllocator = messageAllocator_in;

  // *NOTE*: need to do this BEFORE opening any socket because
  // ::getnameinfo apparently cannot handle AF_PACKET sockets (don't ask !)...
  // retrieve local IP address
  std::string ip_address;
  if (!RPG_Net_Common_Tools::retrieveLocalIPAddress(networkInterface_in,
                                                    ip_address))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::retrieveLocalIPAddress(\"%s\"), aborting\n"),
               networkInterface_in.c_str()));

    return false;
  } // end IF

  // retrieve local hostname
  std::string hostname;
  if (!RPG_Net_Common_Tools::retrieveLocalHostname(hostname))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::retrieveLocalHostname(), aborting\n")));

    return false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("local interface (\"%s\") --> hostname [IP address]: \"%s\" [\"%s\"]\n"),
             networkInterface_in.c_str(),
             hostname.c_str(),
             ip_address.c_str()));

  // tweak socket options
  if (sizeReceiveBuffer_in)
  {
    if (!RPG_Net_Common_Tools::setSocketBuffer(get_handle(),
                                               SO_RCVBUF,
                                               sizeReceiveBuffer_in))
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to RPG_Net_Common_Tools::setSocketBuffer(SO_RCVBUF, %u), aborting\n"),
                sizeReceiveBuffer_in));

      return false;
    } // end IF
  } // end IF

  // schedule regular statistics collection...
  if (statCollectInterval_in)
  {
    myStatCollectHandlerID = 0;
    ACE_Time_Value collecting_interval(statCollectInterval_in,
                                       0);
    myStatCollectHandlerID = myTimerQueue.schedule(&myStatCollectHandler,
                                                   NULL,
                                                   ACE_OS::gettimeofday () + collecting_interval,
                                                   collecting_interval);
    if (myStatCollectHandlerID == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule() timer: \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      // reset so we don't get confused in the dtor !
      myStatCollectHandlerID = 0;

      return false;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("scheduled statistics collecting timer (ID: %d) for intervals of %u second(s)...\n"),
               myStatCollectHandlerID,
               statCollectInterval_in));
  } // end IF

  // *NOTE*: need to clean up timer beyond this point !
//     // clean up
//   if (myStatCollectHandlerID)
//   {
//     if (myTimerQueue.cancel(myStatCollectHandlerID) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to cancel timer (ID: %u): \"%s\", continuing\n"),
//                           myStatCollectHandlerID,
//                           ACE_OS::strerror(errno)));
//     } // end IF
//     myStatCollectHandlerID = 0;
//   } // end IF

  // OK: all's well...
  myIsInitialized = true;

  // debug info


  return myIsInitialized;
}

int
RPG_Net_Module_SocketHandler::svc(void)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::svc"));

  RPG_Net_StreamConfigPOD sessionConfigData;
  ACE_OS::memset(&sessionConfigData,
                 0,
                 sizeof(RPG_Net_StreamConfigPOD));
  ACE_Message_Block*      controlMessage   = NULL;
  ACE_Time_Value          now;
  int                     ret              = 0;
  bool                    leaveLoop        = false;

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, aborting\n")));

    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  // step1: send initial session message downstream...
  if (!inherited::putSessionMessage(Stream_SessionMessage::SESSION_BEGIN,
                                    sessionConfigData,
                                    ACE_OS::gettimeofday(), // start of session
                                    false))                 // N/A
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage failed, aborting\n")));

    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("entering processing loop...\n")));

  // step2: process incoming data
  while (!leaveLoop)
  {
    // check user stop condition --> someone stopped() the stream ?
    if (!msg_queue_->is_empty())
    {
      now = ACE_OS::gettimeofday();

      // Note: something SHOULD be in the queue...
      if (getq(controlMessage, &now) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Task::getq(): \"%s\", aborting\n"),
                   ACE_OS::strerror(errno)));

        // remember we failed...
        ret = -1;

        // leave processing loop
        break;
      } // end IF

      // handle this control message
      bool user_abort = false;
      bool passMessageDownstream = true;
      inherited::handleControlMessage(controlMessage,
                                      user_abort,
                                      passMessageDownstream);

      if (passMessageDownstream)
      {
        // we've been told to pass this (control) message downstream...
        if (put_next(controlMessage) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%s\", continuing\n"),
                     ACE_OS::strerror(errno)));

          // remember we failed (at something)...
          ret = -1;
        } // end IF
      } // end IF

      // handle user abort
      if (user_abort)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("(user) abort, stopping activities...\n")));

        // leave processing loop
        break;
      } // end IF
    } // end IF

    // OK: handle data
/*    ret = pcap_dispatch(myPCAPHandle,
                        -1, // <-- "A cnt of -1 processes all the packets received
                            // in one buffer when reading a live capture,..."
                            // http://www.tcpdump.org/pcap3_man.html
                        &RPG_Net_Module_SocketHandler::recordPacket,
                        ACE_reinterpret_cast(u_char*,
                                             this)); // pass handle to ourselves...
    switch (ret)
    {
      // *** GOOD CASE ***
      default:
      {

        break;
      }
      case 0:
      {
        // no packet processed
        // probable causes:
        // - no LAN traffic/read timeout (maybe we are in non-blocking mode ?)
        // - no packets passed the filter
        // - someone (temporarily) pulled the network cable
        // - NIC/network cable broken

        // *IMPORTANT NOTE*: the rationale here is that this state will
        // eventually be identified (and hopefully resolved) by the
        // monitoring TSC, otherwise we will just continually grow our logfile...
        //ACE_DEBUG((LM_WARNING,
        //           ACE_TEXT("read timeout occurred (check cabling/NIC status LEDs/...), continuing\n")));

        break;
      }
      // *** ERROR CASE ***
      case -1:
      {
        // some internal error
        // Probable cause:
        // - someone (accidentally ?) shutdown the network service
        // - no more memory ?
        // - ...
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to pcap_dispatch(): \"%s\" --> check implementation !, aborting\n"),
                   pcap_geterr(myPCAPHandle)));

        // *IMPORTANT NOTE*: we shut ourselves down so we don't come here again
        // and generate (duplicate ?) error messages until we're officially
        // stop()ed from outside...
        // *TODO*: what else can we do ?
        ret = -1;
        leave_loop = true;

        // *IMPORTANT NOTE*: the rationale here is that this state will
        // eventually be identified (and hopefully resolved) by the
        // monitoring TSC, otherwise (problem persists) it MAY happen
        // that we are just continually restarted by the watchdog...
        break;
      }
      case -2:
      {
        // *IMPORTANT NOTE*: we arrive here when pcap_breakloop was called AND there were no packets.
        // If this was invoked from a signal handler, we should have a stop message in our queue by now
        // --> just proceed normally
        ACE_DEBUG((LM_INFO,
                   ACE_TEXT("pcap_breakloop() invoked, continuing\n")));

        break;
      }
    } // end SWITCH*/
  } // end WHILE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("left processing loop...\n")));

  // step3: send final session message downstream...
  if (!inherited::putSessionMessage(Stream_SessionMessage::SESSION_END,
                                    sessionConfigData,
                                    ACE_Time_Value::zero, // N/A
                                    true))                // ALWAYS a user abort...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage failed, aborting\n")));

    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  // tell the controller we're finished...
  finished();

  return ret;
}

void
RPG_Net_Module_SocketHandler::stop()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::stop"));

  // invoke base class behaviour
  inherited::stop();
}

const bool
RPG_Net_Module_SocketHandler::collect(RPG_Net_RuntimeStatistic& data_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::collect"));

  // just a dummy...
  ACE_UNUSED_ARG(data_out);

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, aborting\n")));

    return false;
  } // end IF

  // step0: init info container POD
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  // step1: remember time of stats collection
  ACE_Time_Value collectionTimestamp = ACE_OS::gettimeofday();

  // step2: *TODO*: collect info

  // step3: send this information downstream
  if (!putStatisticsMessage(data_out,
                            collectionTimestamp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to putStatisticsMessage(), aborting\n")));

    return false;
  } // end IF

  // OK: all is well...
  return true;
}

void
RPG_Net_Module_SocketHandler::report()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::report"));

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, returning\n")));

    return;
  } // end IF

  // *TODO*: support (local) reporting here as well ?
  // --> leave this to any downstream modules...
  ACE_ASSERT(false);
}

const bool
RPG_Net_Module_SocketHandler::allocateMessage(const unsigned long& requiredSize_in,
                                              RPG_Net_Message*& message_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::allocateMessage"));

  // init return value(s)
  message_out = NULL;

  try
  {
    // *NOTE*: the argument we pass to the allocator is irrelevant,
    // the allocator allocates RPG_Net_Messages...
    message_out = ACE_static_cast(RPG_Net_Message*,
                                  myAllocator->malloc(requiredSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_Allocator::malloc(%u), aborting\n"),
               requiredSize_in));

    return false;
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Allocator::malloc(%u), aborting\n"),
               requiredSize_in));

    return false;
  } // end IF

  return true;
}

const bool
RPG_Net_Module_SocketHandler::putStatisticsMessage(const RPG_Net_RuntimeStatistic& info_in,
                                                   const ACE_Time_Value& collectionTime_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::putStatisticsMessage"));

  // step1: init info POD
  RPG_Net_StreamConfigPOD data;
  ACE_OS::memset(&data,
                 0,
                 sizeof(RPG_Net_StreamConfigPOD));
  data.currentStatistics = info_in;
  data.collectionTimestamp = collectionTime_in;

  // step2: allocate config container
  RPG_Net_StreamConfig* config = NULL;
  try
  {
    config = new RPG_Net_StreamConfig(data);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in new, aborting\n")));

    return false;
  }
  if (!config)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate memory, aborting\n")));

    return false;
  } // end IF

  // *NOTE*: this is a "fire-and-forget" API, so we don't need to
  // worry about config any longer !
//   return inherited::putSessionMessage(Stream_SessionMessage::SESSION_STATISTICS,
//                                       config);
  return inherited::putSessionMessage(Stream_SessionMessage::SESSION_STATISTICS,
                                      data,
                                      ACE_Time_Value::zero, // N/A
                                      false);
}
/*
void
RPG_Net_Module_SocketHandler::recordPacket(u_char* args_in,
                                           const pcap_pkthdr* header_in,
                                           const u_char* body_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::recordPacket"));

  static RPG_Net_Message* message;
  static RPG_Net_Module_SocketHandler* this_ptr = ACE_reinterpret_cast(RPG_Net_Module_SocketHandler*,
                                                                       args_in);

  // we need to do this:
  // 1. allocate a message buffer
  // 2. copy the data into the buffer
  // 3. perform message initialization
  // 4. send the message buffer downstream

  // step1
  message = NULL;
  if (!this_ptr->allocateMessage(header_in->caplen,
                                 message))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Module_SocketHandler::allocateMessage(%u), aborting\n"),
               header_in->caplen));

    return;
  } // end IF

  // sanity check: message has enough space to hold our data ?
  ACE_ASSERT(message->capacity() >= header_in->caplen);

  // *NOTE*: from this point on, we need to make sure we recycle message !

  // step2
  if (message->copy(ACE_reinterpret_cast(const char*,
                                         body_in),
                    header_in->caplen) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("message (ID: %u): failed to ACE_Message_Block::copy(): \"%s\", aborting\n"),
               message->getID(),
               ACE_OS::strerror(errno)));

    // clean up
    message->release();

    return;
  } // end IF

  // step3
  message->init(//ACE_Time_Value::zero, // this is irrelevant !
                *header_in);

  // step4
  if (this_ptr->put_next(message) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::put_next(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // clean up
    message->release();

    return;
  } // end IF

  // OK: all finished !
}*/
