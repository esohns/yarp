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

#include "rpg_net_module_protocolhandler.h"

#include "rpg_net_message.h"
#include "rpg_net_common_tools.h"

#include <rpg_common_timer_manager.h>

#include <stream_iallocator.h>

#include <iostream>

RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler()
 : //inherited(),
   myClientPingHandler(this,   // dispatch ourselves
                       false), // ping client at REGULAR intervals...
   myClientPingTimerID(0),
   myAllocator(NULL),
   myCounter(1),
   myAutomaticPong(false), // *NOTE*: the idea really is not to play PONG...
   myPrintPongDot(false),
   myIsInitialized(false)//,
//    mySessionID(0)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler"));

}

RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler"));

  // clean up timer if necessary
  if (myClientPingTimerID)
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myClientPingTimerID);
}

const bool
RPG_Net_Module_ProtocolHandler::init(Stream_IAllocator* allocator_in,
                                     const unsigned long& clientPingInterval_in,
                                     const bool& autoAnswerPings_in,
                                     const bool& printPongDot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset state
    if (myClientPingTimerID != -1)
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(myClientPingTimerID);
    myClientPingTimerID = 0;
    myAllocator = NULL;
    myCounter = 1;
    myAutomaticPong = false;
    myPrintPongDot = false;

    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;

  if (clientPingInterval_in)
  {
    // schedule ourselves...
    ACE_Time_Value clientPing_interval(clientPingInterval_in, 0);
    if (!RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->scheduleTimer(myClientPingHandler,  // handler
                                                                      clientPing_interval,  // interval
                                                                      false,                // recurrent
                                                                      myClientPingTimerID)) // return value: timer ID
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_Timer_Manager::scheduleTimer(%u), aborting\n"),
                 clientPingInterval_in));

      return false;
    } // end IF

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("scheduled client ping (interval: %u second(s))...\n"),
//                clientPingInterval_in));
  } // end IF

  myAutomaticPong = autoAnswerPings_in;
//   if (myAutomaticPong)
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("auto-answering ping messages...\n")));
  myPrintPongDot = printPongDot_in;

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_ProtocolHandler::handleDataMessage(Stream_MessageBase*& message_inout,
                                                  bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // retrieve type of message and other details...
  RPG_Net_MessageHeader* message_header = ACE_reinterpret_cast(RPG_Net_MessageHeader*,
                                                               message_inout->rd_ptr());
  switch (message_header->messageType)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      // auto-answer ?
      if (myAutomaticPong)
      {
        // --> reply with a "PONG"

        // step0: create reply structure
        // --> get a message buffer
        RPG_Net_Message* reply_message = allocateMessage(sizeof(RPG_Net_Remote_Comm::PongMessage));
        if (reply_message == NULL)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to allocate reply message(%u), aborting\n"),
                     sizeof(RPG_Net_Remote_Comm::PongMessage)));

          return;
        } // end IF
        // step1: init reply
        RPG_Net_Remote_Comm::PongMessage* reply_struct = ACE_reinterpret_cast(RPG_Net_Remote_Comm::PongMessage*,
                                                                              reply_message->wr_ptr());
        ACE_OS::memset(reply_struct,
                       0,
                       sizeof(RPG_Net_Remote_Comm::PongMessage));
        reply_struct->messageHeader.messageLength = sizeof(RPG_Net_Remote_Comm::PongMessage) - sizeof(unsigned long);
        reply_struct->messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PONG;
        reply_message->wr_ptr(sizeof(RPG_Net_Remote_Comm::PongMessage));
        // step2: send it upstream
        if (reply(reply_message, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::reply(): \"%m\", aborting\n")));

          // clean up
          reply_message->release();

          return;
        } // end IF
      } // end IF

      if (myPrintPongDot)
      {
        std::clog << '.';
      } // end IF

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      // nothing to do...
      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%u]: unknown message type: \"%s\": protocol error, aborting\n"),
                 message_inout->getID(),
                 RPG_Net_Common_Tools::messageType2String(message_header->messageType).c_str()));

      break;
    }
  } // end SWITCH
}

// void
// RPG_Net_Module_ProtocolHandler::handleSessionMessage(RPG_Net_SessionMessage*& message_inout,
//                                                      bool& passMessageDownstream_out)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleSessionMessage"));
//
//   // don't care (implies yes per default, if we're part of a stream)
//   ACE_UNUSED_ARG(passMessageDownstream_out);
//
//   // sanity check(s)
//   ACE_ASSERT(message_inout);
//   ACE_ASSERT(myIsInitialized);
//
//   switch (message_inout->getType())
//   {
//     case Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
//     {
//       // remember session ID for reporting...
//       mySessionID = message_inout->getConfig()->getUserData().sessionID;
//
//       break;
//     }
//     default:
//     {
//       // don't do anything...
//       break;
//     }
//   } // end SWITCH
// }

void
RPG_Net_Module_ProtocolHandler::handleTimeout(const void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleTimeout"));

  ACE_UNUSED_ARG(arg_in);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("timer (ID: %d) expired...sending ping\n"),
//              myTimerID));

  // step0: create ping structure --> get a message buffer
  RPG_Net_Message* ping_message = allocateMessage(sizeof(RPG_Net_Remote_Comm::PingMessage));
  if (ping_message == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate ping message(%u), aborting\n"),
               sizeof(RPG_Net_Remote_Comm::PingMessage)));

    // what else can we do ?
    return;
  } // end IF

  // step1: init ping data
  // *TODO*: clean this up and handle endianness consistently !
  RPG_Net_Remote_Comm::PingMessage* ping_struct = ACE_reinterpret_cast(RPG_Net_Remote_Comm::PingMessage*,
                                                                       ping_message->wr_ptr());
  ACE_OS::memset(ping_struct,
                 0,
                 sizeof(RPG_Net_Remote_Comm::PingMessage));
  ping_struct->messageHeader.messageLength = (sizeof(RPG_Net_Remote_Comm::PingMessage) -
                                              sizeof(unsigned long));
  ping_struct->messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PING;
  ping_struct->counter = myCounter++;
  ping_message->wr_ptr(sizeof(RPG_Net_Remote_Comm::PingMessage));

  // step2: send it upstream
  if (reply(ping_message, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::reply(): \"%m\", returning\n")));

    // clean up
    ping_message->release();

    // what else can we do ?
    return;
  } // end IF
}

void
RPG_Net_Module_ProtocolHandler::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Message*
RPG_Net_Module_ProtocolHandler::allocateMessage(const unsigned long& requestedSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::allocateMessage"));

  // init return value(s)
  RPG_Net_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Message*,
                                  myAllocator->malloc(requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  } // end IF

  return message_out;
}
