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

#include "rpg_net_defines.h"
#include "rpg_net_message.h"
#include "rpg_net_remote_comm.h"
#include "rpg_net_stream_config.h"

#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>

RPG_Net_Module_SocketHandler::RPG_Net_Module_SocketHandler()
 : inherited(false), // DON'T auto-start !
   myIsInitialized(false),
   myConnectionID(0),
   myStatCollectHandler(this,
                        STATISTICHANDLER_TYPE::ACTION_COLLECT),
   myStatCollectHandlerID(0),
   myCurrentMessageLength(0),
   myCurrentMessage(NULL),
   myCurrentBuffer(NULL)
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
  cancelTimer();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...\n")));

  myTimerQueue.deactivate();
  // make sure the dispatcher thread is really dead...
  myTimerQueue.wait();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...DONE\n")));

  // clean up any unprocessed (chained) buffer(s)
  if (myCurrentMessage)
    myCurrentMessage->release();
}

const bool
RPG_Net_Module_SocketHandler::init(Stream_IAllocator* allocator_in,
                                   const unsigned long& connectionID_in,
                                   const unsigned long& statisticsCollectionInterval_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // clean up
    myIsInitialized = false;
    cancelTimer();
  } // end IF

  // schedule regular statistics collection...
  if (statisticsCollectionInterval_in)
  {
    myStatCollectHandlerID = 0;
    ACE_Time_Value collecting_interval(statisticsCollectionInterval_in,
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

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("scheduled statistics collecting timer (ID: %d) for intervals of %u second(s)...\n"),
//                myStatCollectHandlerID,
//                statisticsCollectionInterval_in));
  } // end IF

  // *NOTE*: need to clean up timer beyond this point !

  myConnectionID = connectionID_in;
  inherited::myAllocator = allocator_in;

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_SocketHandler::handleDataMessage(Stream_MessageBase*& message_inout,
                                                bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::handleDataMessage"));

  // init return value(s), default behavior is to pass all messages along...
  // --> we don't want that !
  passMessageDownstream_out = false;

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  // perhaps we already have part of this message ?
  if (myCurrentBuffer)
  {
    // enqueue the incoming buffer onto our chain
    myCurrentBuffer->cont(message_inout);
  } // end IF
  myCurrentBuffer = ACE_dynamic_cast(RPG_Net_Message*, message_inout);

  // sanity check(s)
  ACE_ASSERT(myCurrentBuffer);

  RPG_Net_Message* complete_message = NULL;
  while (bisectMessages(complete_message))
  {
    // full message available ?
    if (complete_message)
    {
      // --> push it downstream...
      if (put_next(complete_message) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Task::put_next(): \"%s\", continuing\n"),
                   ACE_OS::strerror(errno)));

        // clean up
        complete_message->release();
      } // end IF

      // reset state
      complete_message = NULL;
    } // end IF
  } // end WHILE
}

const bool
RPG_Net_Module_SocketHandler::collect(RPG_Net_RuntimeStatistic& data_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::collect"));

  // just a dummy...
  ACE_UNUSED_ARG(data_out);

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);

  // step0: init info container POD
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_RuntimeStatistic));

  // step1: *TODO*: collect info

  // step2: send this information downstream
  if (!putStatisticsMessage(data_out,
                            ACE_OS::gettimeofday()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to putSessionMessage(SESSION_STATISTICS), aborting\n")));

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
  ACE_ASSERT(myIsInitialized);

  // *TODO*: support (local) reporting here as well ?
  // --> leave this to any downstream modules...
  ACE_ASSERT(false);
}

const bool
RPG_Net_Module_SocketHandler::bisectMessages(RPG_Net_Message*& message_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::bisectMessages"));

  // init result
  message_out = NULL;

  if (myCurrentMessageLength == 0)
  {
    // --> evaluate the incoming message header

    // perhaps we already have part of the header ?
    if (myCurrentMessage == NULL)
    {
      // we really don't know anything
      // --> use the current buffer as our head...
      myCurrentMessage = myCurrentBuffer;
    } // end IF

    // OK, perhaps we can start interpreting the message header...

    // check if we received the full header yet...
    if (myCurrentMessage->total_length() < sizeof(RPG_Net_Remote_Comm::MessageHeader))
    {
      // we don't, so keep what we have (default behavior) ...

      // ... and wait for some more data
      return false;
    } // end IF

    // OK, we can start interpreting this message...

    // sanity check: do we have enough CONTIGUOUS data ?
    while (myCurrentMessage->length() < sizeof(RPG_Net_Remote_Comm::MessageHeader))
    {
      // *sigh*: copy some data from the chain to allow interpretation
      // of the message header
      // *WARNING*: for this to work, myCurrentMessage->size() must be
      // AT LEAST sizeof(RPG_Net_Remote_Comm::MessageHeader)...
      ACE_Message_Block* source = myCurrentMessage->cont();
      while (source->length() == 0)
        source = source->cont();
      size_t amount = (source->length() > sizeof(RPG_Net_Remote_Comm::MessageHeader) ? sizeof(RPG_Net_Remote_Comm::MessageHeader)
                                                                                     : source->length());
      if (myCurrentMessage->copy(source->rd_ptr(),
                                 amount))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::copy(): \"%s\", aborting\n"),
                   ACE_OS::strerror(errno)));

        // clean up
        myCurrentMessageLength = 0;
        myCurrentMessage->release();
        myCurrentMessage = NULL;
        myCurrentBuffer = NULL;

        // what else can we do ?
        return false;
      } // end IF

      // adjust the continuation accordingly...
      source->rd_ptr(amount);
    } // end WHILE

    RPG_Net_Remote_Comm::MessageHeader* message_header = ACE_reinterpret_cast(RPG_Net_Remote_Comm::MessageHeader*,
                                                                              myCurrentMessage->rd_ptr());
    // *TODO*: *PORTABILITY*: handle endianness && type issues !
    myCurrentMessageLength = message_header->messageLength + sizeof(unsigned long);
  } // end IF

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("[%u]: received %u bytes [current: %u, total: %u]...\n"),
//              myConnectionID,
//              myCurrentBuffer->length(),
//              myCurrentMessage->total_length(),
//              myCurrentMessageLength));

  // check if we received the whole message yet...
  if (myCurrentMessage->total_length() < myCurrentMessageLength)
  {
    // we don't, so keep what we have (default behavior) ...

    // ... and wait for some more data
    return false;
  } // end IF

  // OK, we have all of it !
  message_out = myCurrentMessage;

  // check if we have received (part of) the next message
  if (myCurrentMessage->total_length() > myCurrentMessageLength)
  {
    // remember overlapping bytes
//     size_t overlap = myCurrentMessage->total_length() - myCurrentMessageLength;

    // adjust write pointer of our current buffer so (total_-)length()
    // reflects the proper size...
    unsigned long offset = myCurrentMessageLength;
    // in order to find the correct offset in myCurrentBuffer, we MAY need to
    // count the total size of the preceding continuation... :-(
    ACE_Message_Block* current = myCurrentMessage;
    while (current != myCurrentBuffer)
    {
      offset -= current->length();
      current = current->cont();
    } // end WHILE

//     myCurrentBuffer->wr_ptr(myCurrentBuffer->rd_ptr() + offset);
//     // --> create a new message head...
//     RPG_Net_Message* new_head = allocateMessage(RPG_NET_DEF_NETWORK_BUFFER_SIZE);
//     if (new_head == NULL)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
//                  RPG_NET_DEF_NETWORK_BUFFER_SIZE));
//
//       // *TODO*: what else can we do ?
//       return true;
//     } // end IF
//     // ...and copy the overlapping data
//     if (new_head->copy(myCurrentBuffer->wr_ptr(),
//                        overlap))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Message_Block::copy(): \"%s\", aborting\n"),
//                  ACE_OS::strerror(errno)));
//
//       // clean up
//       new_head->release();
//
//       // *TODO*: what else can we do ?
//       return true;
//     } // end IF

    // [instead], use copy ctor and just reference the same data block...
    RPG_Net_Message* new_head = ACE_dynamic_cast(RPG_Net_Message*,
                                                 myCurrentBuffer->duplicate());

    // adjust wr_ptr to make length() work...
    myCurrentBuffer->wr_ptr(myCurrentBuffer->rd_ptr() + offset);
    // sanity check
    ACE_ASSERT(myCurrentMessage->total_length() == myCurrentMessageLength);

    // adjust rd_ptr to point to the beginning of the next message
    new_head->rd_ptr(offset);

    // set new message head/current buffer
    myCurrentMessage = new_head;
    myCurrentBuffer = myCurrentMessage;
  } // end IF
  else
  {
    // bye bye...
    myCurrentMessage = NULL;
    myCurrentBuffer = NULL;
  } // end ELSE

  // don't know anything about the next message...
  myCurrentMessageLength = 0;

  return true;
}

// RPG_Net_Message*
// RPG_Net_Module_SocketHandler::allocateMessage(const unsigned long& requestedSize_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::allocateMessage"));
//
//   // init return value(s)
//   RPG_Net_Message* message_out = NULL;
//
//   try
//   {
//     message_out = ACE_static_cast(RPG_Net_Message*,
//                                   inherited::myAllocator->malloc(requestedSize_in));
//   }
//   catch (...)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
//                requestedSize_in));
//   }
//   if (!message_out)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
//                requestedSize_in));
//   } // end IF
//
//   return message_out;
// }

const bool
RPG_Net_Module_SocketHandler::putStatisticsMessage(const RPG_Net_RuntimeStatistic& info_in,
                                                   const ACE_Time_Value& collectionTime_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::putStatisticsMessage"));

  // step1: init info POD
  RPG_Net_ConfigPOD data;
  ACE_OS::memset(&data,
                 0,
                 sizeof(RPG_Net_ConfigPOD));
  data.currentStatistics = info_in;
  data.lastCollectionTimestamp = collectionTime_in;

  // step2: allocate config container
  RPG_Net_StreamConfig* config = NULL;
  ACE_NEW_NORETURN(config,
                   RPG_Net_StreamConfig(data));
  if (!config)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate RPG_Net_StreamConfig: \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // step3: send the data downstream...
  // *NOTE*: this is a "fire-and-forget" API, so we don't need to
  // worry about config any longer !
  return inherited::putSessionMessage(Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS,
                                      config,
                                      inherited::myAllocator);
}
