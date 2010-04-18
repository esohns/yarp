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

#include "rpg_net_protocol_module_IRCsplitter.h"

#include "rpg_net_protocol_defines.h"

#include <rpg_net_message.h>

#include <stream_iallocator.h>

#include <string.h>

RPG_Net_Protocol_Module_IRCSplitter::RPG_Net_Protocol_Module_IRCSplitter()
 : inherited(false), // DON'T auto-start !
   myIsInitialized(false),
   mySessionID(0),
   myStatCollectHandler(this,
                        STATISTICHANDLER_TYPE::ACTION_COLLECT),
   myStatCollectHandlerID(0),
   myTraceScanning(false),
//    myScanner(NULL,  // no default input stream
//              NULL), // no default output stream
   myScanner(NULL),
   myCurrentNumMessages(0),
   myCurrentState(NULL),
   myCurrentMessage(NULL),
   myCurrentBuffer(NULL),
   myCurrentMessageLength(0),
   myCurrentBufferIsResized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::RPG_Net_Protocol_Module_IRCSplitter"));

}

RPG_Net_Protocol_Module_IRCSplitter::~RPG_Net_Protocol_Module_IRCSplitter()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::~RPG_Net_Protocol_Module_IRCSplitter"));

  // clean up timer if necessary
  if (myStatCollectHandlerID)
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);

  // fini scanner context
  if (myScanner)
    yy_destroy(myScanner);

  // clean up any unprocessed (chained) buffer(s)
  if (myCurrentMessage)
    myCurrentMessage->release();
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::init(Stream_IAllocator* allocator_in,
                                          const unsigned long& statisticsCollectionInterval_in,
                                          const bool& traceScanning_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // clean up
    myIsInitialized = false;
    mySessionID = 0;
    if (myStatCollectHandlerID)
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);
    myStatCollectHandlerID = 0;
    myTraceScanning = false;
    // *TODO*: use yyrestart() ?
    // fini scanner context
    if (myScanner)
      yy_destroy(myScanner);
    myScanner = NULL;
    myCurrentNumMessages = 0;
    if (myCurrentState)
      yy_delete_buffer(myCurrentState);
    myCurrentState = NULL;
    if (myCurrentMessage)
      myCurrentMessage->release();
    myCurrentMessage = NULL;
    myCurrentBuffer = NULL;
    myCurrentMessageLength = 0;
    myCurrentBufferIsResized = false;
  } // end IF

  // set base class initializer(s)
  inherited::myAllocator = allocator_in;

  if (statisticsCollectionInterval_in)
  {
    // schedule regular statistics collection...
    ACE_Time_Value collecting_interval(statisticsCollectionInterval_in, 0);
    if (!RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->scheduleTimer(myStatCollectHandler,    // handler
                                                                      collecting_interval,     // interval
                                                                      false,                   // one-shot ?
                                                                      myStatCollectHandlerID)) // return value: id
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_Timer_Manager::scheduleTimer(%u), aborting\n"),
                 statisticsCollectionInterval_in));

      // what else can we do ?
      return false;
    } // end IF
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("scheduled statistics collecting timer (ID: %d) for intervals of %u second(s)...\n"),
//                myStatCollectHandlerID,
//                statisticsCollectionInterval_in));
  } // end IF

  // *NOTE*: need to clean up timer beyond this point !

  myTraceScanning = traceScanning_in;

  // init scanner context
  if (yy_init_extra(&myCurrentNumMessages, // extra data
                    &myScanner))           // scanner context handle
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to yy_init(): \"%m\", aborting\n")));

    // clean up
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);
    myStatCollectHandlerID = 0;

    // what else can we do ?
    return false;
  } // end IF

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCSplitter::handleDataMessage(Stream_MessageBase*& message_inout,
                                                       bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::handleDataMessage"));

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
  else
  {
    myCurrentBuffer = ACE_dynamic_cast(RPG_Net_Message*, message_inout);
    // sanity check(s)
    ACE_ASSERT(myCurrentBuffer);
  } // end ELSE

  // scan the incoming stream for frame bounds "\r\n"

  // do we know where to start ?
  if (myCurrentMessage == NULL)
    myCurrentMessage = myCurrentBuffer; // start scanning at offset 0...

  // *NOTE*: the scanner checks sequences of >= 2 bytes (.*\r\n)
  // --> make sure we have a minimum amount of data...
  // --> more sanity check(s)
  if (myCurrentMessage->total_length() < RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE)
    return; // don't have enough data --> cannot proceed
  if (myCurrentBuffer->length() < RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE)
  {
    // *sigh*: OK, so this CAN actually happen...
    // case1: if we have anything OTHER than '\n', there's nothing to do
    //        --> wait for more data
    // case2: if we have an '\n' we have to check the trailing character
    //        of the PRECEDING buffer:
    //        - if it's an '\r' --> voila, we've found a frame boundary
    //        - else            --> wait for more data
    if (((*myCurrentBuffer->rd_ptr()) == '\n') &&
        (myCurrentMessage != myCurrentBuffer))
    {
      ACE_Message_Block* preceding_buffer = myCurrentMessage;
      for (;
            preceding_buffer->cont() != myCurrentBuffer;
            preceding_buffer = preceding_buffer->cont());
      if (*(preceding_buffer->rd_ptr() + (preceding_buffer->length() - 1)) == '\r')
      {
        // OK, we have all of it !
        // --> push it downstream...
        if (put_next(myCurrentMessage, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%m\", continuing\n")));

          // clean up
          myCurrentMessage->release();
        } // end IF

        // bye bye...
        myCurrentMessage = NULL;
        myCurrentBuffer = NULL;

        return;
      } // end IF
    } // end IF

    return; // don't have enough data --> cannot proceed
  } // end IF

  // OK, init our scanner...

  // *NOTE*: in order to accomodate flex, the buffer needs two trailing
  // '\0' characters...
  // --> make sure it has this capacity
  if (myCurrentBuffer->space() < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)
  {
    // *sigh*: (try to) resize it then...
    if (myCurrentBuffer->size(myCurrentBuffer->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), aborting\n"),
                 (myCurrentBuffer->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));

      // what else can we do ?
      return false;
    } // end IF
    myCurrentBufferIsResized = true;

    // *WARNING*: beyond this point, make sure we resize the buffer back
    // to its original length...
  } // end IF
//   for (int i = 0;
//        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
//        i++)
//     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
  *(myCurrentBuffer->wr_ptr()) = YY_END_OF_BUFFER_CHAR;
  *(myCurrentBuffer->wr_ptr() + 1) = YY_END_OF_BUFFER_CHAR;

  if (!scan_begin(myCurrentBuffer->rd_ptr(),
                  myCurrentBuffer->length() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to scan_begin(%@, %u), aborting\n"),
               myCurrentBuffer->rd_ptr(),
               (myCurrentBuffer->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));

    // clean up
    if (myCurrentBufferIsResized)
    {
      if (myCurrentBuffer->size(myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                   (myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
      myCurrentBufferIsResized = false;
    } // end IF

    // what else can we do ?
    return false;
  } // end IF

  // scan it !
  myCurrentNumMessages = 0;
//   while (myCurrentMessageLength = myScanner.yylex())
  do
  {
    myCurrentMessageLength = yylex(myScanner);
    switch (myCurrentMessageLength)
    {
//       case -1:
//       {
//       //     ACE_DEBUG((LM_ERROR,
//     //                ACE_TEXT("failed to IRCBisectFlexLexer::yylex(): \"%m\", aborting\n")));
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to yylex(): \"%m\", aborting\n")));
      //
//         // what else can we do ?
//         return false;
//       }
      case 0:
      {
        // no (more) frame boundaries found
        // --> finished scanning

        // *WARNING*: we may ALSO get here IF the message was "empty" ("\r\n")
        // --> according to the RFC, this is valid (!)
        // --> pass the message on
        // --> fall through...
        if (::strncmp(myCurrentBuffer->rd_ptr(),
                      RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY,
                      RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE))
          break;
      }
      default:
      {
        // OK: found a frame boundary !

        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("message (ID: %u): found frame #%u (length: %u), boundary at offset %u...\n"),
                   myCurrentBuffer->getID(),
                   myCurrentNumMessages,
                   (myCurrentMessageLength + RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE),
                   (myCurrentMessageLength + (myCurrentBuffer->rd_ptr() - myCurrentBuffer->base()))));

        // OK, we have all of it !
        // use copy ctor and just reference the same data block...
        RPG_Net_Message* new_head = ACE_dynamic_cast(RPG_Net_Message*,
                                                     myCurrentBuffer->duplicate());

        // adjust wr_ptr to make (total-)length() work...
        myCurrentBuffer->wr_ptr(myCurrentBuffer->rd_ptr() + (myCurrentMessageLength + RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE));
        // adjust rd_ptr to point to the beginning of the next message
        new_head->rd_ptr(myCurrentMessageLength + RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE);

        // --> push it downstream...
        if (put_next(myCurrentMessage, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%m\", continuing\n")));

          // clean up
          myCurrentMessage->release();
        } // end IF

        // set new message head/current buffer
        myCurrentMessage = new_head;
        myCurrentBuffer = myCurrentMessage;

        break;
      }
    } // end SWITCH
  } // end WHILE

  // no (more) frames within this buffer...
  myCurrentMessageLength = 0;

  // clean up
  scan_end();
  // *NOTE*: that even if we've sent some frames downstream in the meantime,
  // we're still referencing the same buffer we resized earlier, as it's always
  // the new "head" message...
  if (myCurrentBufferIsResized)
  {
    if (myCurrentBuffer->size(myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                 (myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
    myCurrentBufferIsResized = false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("message (ID: %u): contains %u frames...\n"),
             myCurrentBuffer->getID(),
             myCurrentNumMessages));
}

void
RPG_Net_Protocol_Module_IRCSplitter::handleSessionMessage(RPG_Net_SessionMessage*& message_inout,
                                                          bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

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
    default:
    {
      // don't do anything...
      break;
    }
  } // end SWITCH
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::collect"));

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
RPG_Net_Protocol_Module_IRCSplitter::report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::report"));

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);

  // *TODO*: support (local) reporting here as well ?
  // --> leave this to any downstream modules...
  ACE_ASSERT(false);
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::putStatisticsMessage(const RPG_Net_RuntimeStatistic& info_in,
                                                          const ACE_Time_Value& collectionTime_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::putStatisticsMessage"));

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
               ACE_TEXT("failed to allocate RPG_Net_StreamConfig: \"%m\", aborting\n")));

    return false;
  } // end IF

  // step3: send the data downstream...
  // *NOTE*: this is a "fire-and-forget" API, so we don't need to
  // worry about config any longer !
  return inherited::putSessionMessage(mySessionID,
                                      Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS,
                                      config,
                                      inherited::myAllocator);
}
