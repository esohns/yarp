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
#include "stdafx.h"

#include "rpg_net_module_protocolhandler.h"

#include "rpg_net_sessionmessage.h"
#include "rpg_net_message.h"

#include "rpg_stream_iallocator.h"

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"

#include <iostream>

RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler()
 : //inherited(),
   myPingHandler(this,   // dispatch ourselves
                 false), // ping peer at REGULAR intervals...
   myPingTimerID(-1),
   myAllocator(NULL),
   mySessionID(0),
   myCounter(1),
	 myPingInterval(0), // [0: --> OFF]
   myAutomaticPong(true),
   myPrintPongDot(false),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler"));

}

RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler"));

  // clean up timer if necessary
  if (myPingTimerID != -1)
	{
    if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myPingTimerID) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
								 mySessionID,
                 myPingTimerID));
		else
			ACE_DEBUG((LM_WARNING,
				         ACE_TEXT("session %u: cancelled \"ping\" timer (ID: %d)\n"),
								 mySessionID,
				         myPingTimerID));
	} // end IF
}

bool
RPG_Net_Module_ProtocolHandler::init(RPG_Stream_IAllocator* allocator_in,
                                     const unsigned int& sessionID_in,
                                     const unsigned int& pingInterval_in,
                                     const bool& autoAnswerPings_in,
                                     const bool& printPongDot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in && sessionID_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset state
    if (myPingTimerID != -1)
		{
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myPingTimerID) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
									 mySessionID,
                   myPingTimerID));
			else
				ACE_DEBUG((LM_DEBUG,
									 ACE_TEXT("session %u: cancelled \"ping\" timer (ID: %d)\n"),
									 mySessionID,
									 myPingTimerID));
      myPingTimerID = -1;
		} // end IF
    myAllocator = NULL;
    mySessionID = 0;
    myCounter = 1;
		myPingInterval = 0;
    myAutomaticPong = true;
    myPrintPongDot = false;

    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;
  mySessionID = sessionID_in;
	myPingInterval = pingInterval_in;
  myAutomaticPong = autoAnswerPings_in;
  //if (myAutomaticPong)
  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("auto-answering \"ping\" messages\n")));
  myPrintPongDot = printPongDot_in;

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_ProtocolHandler::handleDataMessage(RPG_Net_Message*& message_inout,
                                                  bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // retrieve type of message and other details...
  RPG_Net_MessageHeader* message_header = reinterpret_cast<RPG_Net_MessageHeader*>(message_inout->rd_ptr());
  switch (message_header->messageType)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("received PING (connection ID: %u)...\n"),
                 mySessionID));

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
        RPG_Net_Remote_Comm::PongMessage* reply_struct = reinterpret_cast<RPG_Net_Remote_Comm::PongMessage*>(reply_message->wr_ptr());
        ACE_OS::memset(reply_struct, 0, sizeof(RPG_Net_Remote_Comm::PongMessage));
        reply_struct->messageHeader.messageLength = sizeof(RPG_Net_Remote_Comm::PongMessage) - sizeof(unsigned int);
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

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("sent PONG to client (connection ID: %u)...\n"),
                   mySessionID));
      } // end IF

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("received PONG (connection ID: %u)...\n"),
                 mySessionID));

      if (myPrintPongDot)
        std::clog << '.';

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%u]: unknown message type: \"%s\": protocol error, aborting\n"),
                 message_inout->getID(),
                 RPG_Net_Message::commandType2String(message_header->messageType).c_str()));

      break;
    }
  } // end SWITCH
}

void
RPG_Net_Module_ProtocolHandler::handleSessionMessage(RPG_Net_SessionMessage*& message_inout,
                                                     bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleSessionMessage"));

	// don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  switch (message_inout->getType())
  {
   	case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
			if (myPingInterval)
			{
				// schedule ourselves...
				ACE_Time_Value ping_interval(myPingInterval, 0);
				ACE_ASSERT(myPingTimerID == -1);
				myPingTimerID = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&myPingHandler,                          // handler
																																								NULL,                                    // act
																																								ACE_OS::gettimeofday () + ping_interval, // wakeup time
																																								ping_interval);                          // interval
				if (myPingTimerID == -1)
				{
					ACE_DEBUG((LM_ERROR,
										 ACE_TEXT("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));

					return;
				} // end IF

				 //ACE_DEBUG((LM_DEBUG,
				 //           ACE_TEXT("scheduled \"ping\" timer (ID: %d), interval: %u second(s)...\n"),
				 //           myPingTimerID,
				 //           pingInterval_in));
			} // end IF

			break;
    }
		case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
			if (myPingTimerID != -1)
			{
				if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myPingTimerID) == -1)
					ACE_DEBUG((LM_ERROR,
										 ACE_TEXT("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
										 mySessionID,
										 myPingTimerID));
				//else
				//	ACE_DEBUG((LM_DEBUG,
				//						 ACE_TEXT("session %u: cancelled \"ping\" timer (ID: %d)\n"),
				//						 mySessionID,
				//						 myPingTimerID));
				myPingTimerID = -1;
			} // end IF

			break;
    }
    default:
      break;
  } // end SWITCH
}

void
RPG_Net_Module_ProtocolHandler::handleTimeout(const void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleTimeout"));

  ACE_UNUSED_ARG(arg_in);

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
  RPG_Net_Remote_Comm::PingMessage* ping_struct = reinterpret_cast<RPG_Net_Remote_Comm::PingMessage*>(ping_message->wr_ptr());
  ACE_OS::memset(ping_struct, 0, sizeof(RPG_Net_Remote_Comm::PingMessage));
  ping_struct->messageHeader.messageLength = (sizeof(RPG_Net_Remote_Comm::PingMessage) -
                                              sizeof(unsigned int));
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
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Message*
RPG_Net_Module_ProtocolHandler::allocateMessage(const unsigned int& requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::allocateMessage"));

  // init return value(s)
  RPG_Net_Message* message_out = NULL;

  try
  {
    message_out = static_cast<RPG_Net_Message*>(myAllocator->malloc(requestedSize_in));
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
