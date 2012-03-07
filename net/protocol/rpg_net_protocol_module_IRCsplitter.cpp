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
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_message.h"

#include <rpg_common_timer_manager.h>

#include <rpg_stream_iallocator.h>

RPG_Net_Protocol_Module_IRCSplitter::RPG_Net_Protocol_Module_IRCSplitter()
 : inherited(false), // DON'T auto-start !
   myCrunchMessages(false),
   mySessionID(0),
   myStatCollectHandler(this,
                        STATISTICHANDLER_TYPE::ACTION_COLLECT),
   myStatCollectHandlerID(0),
   myScannerContext(NULL),
   myCurrentBufferState(NULL),
   myCurrentNumFrames(0),
   myCurrentMessage(NULL),
   myCurrentBuffer(NULL),
   myCurrentMessageLength(0),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::RPG_Net_Protocol_Module_IRCSplitter"));

  if (RPG_Net_Protocol_IRCBisect_lex_init(&myScannerContext))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to yylex_init: \"%m\", continuing\n")));

  // trace ?
  RPG_Net_Protocol_IRCBisect_set_debug((RPG_NET_PROTOCOL_DEF_TRACE_SCANNING ? 1 : 0), myScannerContext);
}

RPG_Net_Protocol_Module_IRCSplitter::~RPG_Net_Protocol_Module_IRCSplitter()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::~RPG_Net_Protocol_Module_IRCSplitter"));

  // clean up timer if necessary
  if (myStatCollectHandlerID)
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);

  // fini scanner context
  if (myScannerContext)
    RPG_Net_Protocol_IRCBisect_lex_destroy(myScannerContext);

  // clean up any unprocessed (chained) buffer(s)
  if (myCurrentMessage)
    myCurrentMessage->release();
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::init(RPG_Stream_IAllocator* allocator_in,
                                          const bool& crunchMessages_in,
                                          const unsigned long& statisticsCollectionInterval_in,
                                          const bool& traceScanning_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // clean up
    myCrunchMessages = false;
    mySessionID = 0;
    if (myStatCollectHandlerID)
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);
    myStatCollectHandlerID = 0;
    myCurrentNumFrames = 0;
    if (myCurrentBufferState)
      RPG_Net_Protocol_IRCBisect__delete_buffer(myCurrentBufferState, myScannerContext);
    myCurrentBufferState = NULL;
    if (myCurrentMessage)
      myCurrentMessage->release();
    myCurrentMessage = NULL;
    myCurrentBuffer = NULL;
    myCurrentMessageLength = 0;
//     myCurrentBufferIsResized = false;
    myIsInitialized = false;
  } // end IF

  // set base class initializer(s)
  inherited::myAllocator = allocator_in;

  myCrunchMessages = crunchMessages_in;

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

  //// init scanner context
  //if (IRCBisectlex_init_extra(&myCurrentNumFrames, // extra data
  //                            &myScannerContext))  // scanner context handle
  //{
  //  ACE_DEBUG((LM_ERROR,
  //             ACE_TEXT("failed to IRCBisectlex_init_extra(): \"%m\", aborting\n")));

  //  // clean up
  //  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myStatCollectHandlerID);
  //  myStatCollectHandlerID = 0;

  //  // what else can we do ?
  //  return false;
  //} // end IF
  RPG_Net_Protocol_IRCBisect_set_debug((traceScanning_in ? 1 : 0), myScannerContext);

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCSplitter::handleDataMessage(RPG_Net_Protocol_Message*& message_inout,
                                                       bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::handleDataMessage"));

  // init return value(s), default behavior is to pass all messages along...
  // --> we don't want that !
  passMessageDownstream_out = false;

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  // perhaps we already have part of this message ?
  if (myCurrentBuffer)
    myCurrentBuffer->cont(message_inout); // chain the buffer
  myCurrentBuffer = message_inout;

  // scan the bytestream for frame bounds "\r\n"

  // do we know where to start ?
  if (myCurrentMessage == NULL)
    myCurrentMessage = myCurrentBuffer; // start scanning at offset 0...

  // *NOTE*: the scanner checks sequences of >= 2 bytes (.*\r\n)
  // --> make sure we have a minimum amount of data...
  // --> more sanity check(s)
  if (myCurrentMessage->total_length() < RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE)
    return; // don't have enough data, cannot proceed
  if (myCurrentBuffer->length() < RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE)
  {
    // *sigh*: OK, so this CAN actually happen...
    // case1: if we have anything OTHER than '\n', there's nothing to do
    //        --> wait for more data
    // case2: if we have an '\n' we have to check the trailing character
    //        of the PRECEDING buffer:
    //        - if it's an '\r' --> voilà, we've found a frame boundary
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
        if (myCrunchMessages)
        {
          myCurrentMessage->crunch();
        } // end IF

        // --> push it downstream...
        if (put_next(myCurrentMessage, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%m\", continuing\n")));

          // clean up
          myCurrentMessage->release();
        } // end IF

        // bye bye...
        myCurrentMessageLength = 0;
        myCurrentMessage = NULL;
        myCurrentBuffer = NULL;

        return;
      } // end IF
    } // end IF

    return; // don't have enough data, cannot proceed
  } // end IF

  // OK, init scanner...

  // *WARNING*: cannot use yy_scan_buffer(), as flex modifies the data... :-(
//   // *NOTE*: in order to accomodate flex, the buffer needs two trailing
//   // '\0' characters...
//   // --> make sure it has this capacity
//   if (myCurrentBuffer->space() < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)
//   {
//     // *sigh*: (try to) resize it then...
//     if (myCurrentBuffer->size(myCurrentBuffer->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Message_Block::size(%u), aborting\n"),
//                  (myCurrentBuffer->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//
//       // what else can we do ?
//       return;
//     } // end IF
//     myCurrentBufferIsResized = true;
//
//     // *WARNING*: beyond this point, make sure we resize the buffer back
//     // to its original length...
//     // *NOTE*: this is safe, as realloc() just crops the trailing bytes again...
//   } // end IF
// //   for (int i = 0;
// //        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
// //        i++)
// //     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
//   *(myCurrentBuffer->wr_ptr()) = '\0';
//   *(myCurrentBuffer->wr_ptr() + 1) = '\0';
//
//   if (!scan_begin(myCurrentBuffer->rd_ptr(),
//                   myCurrentBuffer->length() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
  if (!scan_begin(myCurrentBuffer->rd_ptr(),
                  myCurrentBuffer->length()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to scan_begin(%@, %u), aborting\n"),
               myCurrentBuffer->rd_ptr(),
               myCurrentBuffer->length()));

//     // clean up
//     if (myCurrentBufferIsResized)
//     {
//       if (myCurrentBuffer->size(myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
//                    (myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//       myCurrentBufferIsResized = false;
//     } // end IF

    // what else can we do ?
    return;
  } // end IF

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("[%u]: scanning %u byte(s)\n\"%s\"\n"),
//              myCurrentBuffer->getID(),
//              myCurrentBuffer->length(),
//              std::string(myCurrentBuffer->rd_ptr(), myCurrentBuffer->length()).c_str()));

  // scan it !
  myCurrentNumFrames = 0;
  bool finished_scanning = false;
  int scanned_bytes = 0;
  int scanned_chunk = 0;
//   while (myCurrentMessageLength = myScanner.yylex())
  do
  {
    scanned_chunk = RPG_Net_Protocol_IRCBisect_lex(myScannerContext);
//    scanned_chunk = IRCBisectlex(myScannerContext);
    switch (scanned_chunk)
    {
      case 0:
      {
        // --> finished scanning this buffer
        finished_scanning = true; // no (more) frame boundaries found

        // remember how much data was scanned so far...
        myCurrentMessageLength += scanned_bytes;

        break;
      }
      case -1:
      {
        // found a frame border scanned_bytes bytes into the buffer

        // *NOTE*: if scanned_bytes == 0, then it's the corner
        // case where the current buffer starts with either:
        // - a '\n'
        // - a "\r\n"
        // *NOTE*: in EITHER case, a new frame has been found...
        if ((scanned_bytes == 0) &&
            (*myCurrentBuffer->rd_ptr() == '\n'))
        {
          scanned_bytes = 1;
          myCurrentMessageLength += RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE;
        } // end IF
        else
        {
          scanned_bytes += RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE;
          myCurrentMessageLength += scanned_bytes;
        } // end IF

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("buffer (ID: %u, length: %u): frame boundary [#%u] @ offset %u\n\"%s\"\n"),
//                    myCurrentBuffer->getID(),
//                    myCurrentMessageLength,
//                    myCurrentNumFrames,
//                    (scanned_bytes + (myCurrentBuffer->rd_ptr() - myCurrentBuffer->base())),
//                    std::string(myCurrentBuffer->rd_ptr(), scanned_bytes).c_str()));

        RPG_Net_Protocol_Message* message = myCurrentMessage;
        if (myCurrentMessageLength < myCurrentMessage->total_length())
        {
          // more data to scan...

          // *NOTE*: copy ctor shallow-copies the current data block
          myCurrentMessage = dynamic_cast<RPG_Net_Protocol_Message*>(myCurrentBuffer->duplicate());
          ACE_ASSERT(myCurrentMessage);
          // adjust wr_ptr (point to one-past-the-end of the current message)
          myCurrentBuffer->wr_ptr(myCurrentBuffer->rd_ptr() + scanned_bytes);
          ACE_ASSERT(myCurrentMessageLength == message->total_length());
          // adjust rd_ptr (point to the beginning of the next message)
          myCurrentMessage->rd_ptr(scanned_bytes);
        } // end IF
        else
        {
          // NO more data to scan...
          ACE_ASSERT(myCurrentMessageLength == myCurrentMessage->total_length());

          // set new message head
          myCurrentMessage = NULL;
        } // end ELSE

        if (myCrunchMessages)
        {
          message->crunch();
        } // end IF

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("processing message (ID: %u - %u byte(s))...\n"),
//                    message->getID(),
//                    message->total_length()));

        // --> push it downstream...
        if (put_next(message, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%m\", continuing\n")));

          // clean up
          message->release();
        } // end IF

        // set new current buffer
        myCurrentBuffer = myCurrentMessage;
        // reset state
        myCurrentMessageLength = 0;
        scanned_bytes = 0;

        // ...continue scanning !
        break;
      }
      default:
      {
        // scanned one/some character(s)...
        scanned_bytes += scanned_chunk;

        break;
      }
    } // end SWITCH
  } while (!finished_scanning);

  // clean up
  scan_end();
//   // *NOTE*: that even if we've sent some frames downstream in the meantime,
//   // we're still referencing the same buffer we resized earlier - it's always
//   // the new "head" message...
//   if (myCurrentBufferIsResized)
//   {
//     if (myCurrentBuffer->size(myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
//                  (myCurrentBuffer->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//     myCurrentBufferIsResized = false;
//   } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("found %u frame bound(s)...\n"),
//              myCurrentNumFrames));
}

void
RPG_Net_Protocol_Module_IRCSplitter::handleSessionMessage(RPG_Net_Protocol_SessionMessage*& message_inout,
                                                          bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
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
RPG_Net_Protocol_Module_IRCSplitter::collect(RPG_Net_Protocol_RuntimeStatistic& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::collect"));

  // just a dummy...
  ACE_UNUSED_ARG(data_out);

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);

  // step0: init info container POD
  ACE_OS::memset(&data_out,
                 0,
                 sizeof(RPG_Net_Protocol_RuntimeStatistic));

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
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::report"));

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);

  // *TODO*: support (local) reporting here as well ?
  // --> leave this to any downstream modules...
  ACE_ASSERT(false);
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::putStatisticsMessage(const RPG_Net_Protocol_RuntimeStatistic& info_in,
                                                          const ACE_Time_Value& collectionTime_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::putStatisticsMessage"));

  // step1: init info POD
  RPG_Net_Protocol_ConfigPOD data;
  ACE_OS::memset(&data,
                 0,
                 sizeof(RPG_Net_Protocol_ConfigPOD));
  data.currentStatistics = info_in;
  data.lastCollectionTimestamp = collectionTime_in;

  // step2: allocate config container
  SESSIONCONFIG_TYPE* config = NULL;
  ACE_NEW_NORETURN(config,
                   SESSIONCONFIG_TYPE(data));
  if (!config)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate Stream_SessionConfigBase: \"%m\", aborting\n")));

    return false;
  } // end IF

  // step3: send the data downstream...
  // *NOTE*: this is a "fire-and-forget" API, so we don't need to
  // worry about config any longer !
  return inherited::putSessionMessage(mySessionID,
                                      RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS,
                                      config,
                                      inherited::myAllocator);
}

const bool
RPG_Net_Protocol_Module_IRCSplitter::scan_begin(char* data_in,
                                                const size_t& length_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState == NULL);

  // create/init a new buffer state
  // *WARNING*: cannot use yy_scan_buffer(), as flex modifies the data... :-(
//   // *WARNING*: length_in IS already adjusted for two trailing \0's
//   myCurrentState = yy_scan_buffer(data_in,
//                                   length_in,
//                                   myScannerContext);
  myCurrentBufferState = RPG_Net_Protocol_IRCBisect__scan_bytes(data_in,
                                                                length_in,
                                                                myScannerContext);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to yy_scan_bytes(%@,%d), aborting\n"),
               data_in,
               length_in));

    // what else can we do ?
    return false;
  } // end IF

  // *WARNING*: contrary (!) to the documentation, still need to switch_buffers()...
  RPG_Net_Protocol_IRCBisect__switch_to_buffer(myCurrentBufferState, myScannerContext);

  return true;
}

void
RPG_Net_Protocol_Module_IRCSplitter::scan_end()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState);

  // clean state
  RPG_Net_Protocol_IRCBisect__delete_buffer(myCurrentBufferState, myScannerContext);
  myCurrentBufferState = NULL;
}
