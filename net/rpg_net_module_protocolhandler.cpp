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

#include <iostream>

#include "common_timer_manager.h"

#include "stream_iallocator.h"

#include "net_remote_comm.h"
#include "net_sessionmessage.h"
#include "net_message.h"

//#include "rpg_common_macros.h"
//#include "rpg_common.h"

RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler ()
 : inherited ()
 , myPingHandler (this,  // dispatch ourselves
                  false) // ping peer at REGULAR intervals...
 , myPingTimerID (-1)
 , myAllocator (NULL)
 , mySessionID (0)
 , myCounter (1)
 , myPingInterval (0) // [0: --> OFF]
 , myAutomaticPong (true)
 , myPrintPongDot (false)
 , myIsInitialized (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler"));

}

RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler"));

  // clean up timer if necessary
  if (myPingTimerID != -1)
  {
    if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (myPingTimerID) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
                  mySessionID,
                  myPingTimerID));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("session %u: cancelled \"ping\" timer (ID: %d)\n"),
                  mySessionID,
                  myPingTimerID));
  } // end IF
}

bool
RPG_Net_Module_ProtocolHandler::initialize (Stream_IAllocator* allocator_in,
                                            unsigned int sessionID_in,
                                            unsigned int pingInterval_in,
                                            bool autoAnswerPings_in,
                                            bool printPongDot_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::initialize"));

  // sanity check(s)
  ACE_ASSERT (allocator_in && sessionID_in);

  if (myIsInitialized)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    // reset state
    if (myPingTimerID != -1)
    {
      const void* act_p = NULL;
      if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (myPingTimerID,
                                                              &act_p) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
                    mySessionID,
                    myPingTimerID));
      else
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("session %u: cancelled \"ping\" timer (ID: %d)\n"),
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
RPG_Net_Module_ProtocolHandler::handleDataMessage (Net_Message*& message_inout,
                                                   bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // retrieve type of message and other details...
  Net_Remote_Comm::MessageHeader* message_header_p =
    reinterpret_cast<Net_Remote_Comm::MessageHeader*> (message_inout->rd_ptr ());
  switch (message_header_p->messageType)
  {
    case Net_Remote_Comm::NET_PING:
    {
      // auto-answer ?
      if (myAutomaticPong)
      {
        // --> reply with a "PONG"

        // step0: create reply structure
        // --> get a message buffer
        Net_Message* reply_message_p = allocateMessage (sizeof (Net_Remote_Comm::PongMessage));
        if (!reply_message_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to allocate reply message(%u), aborting\n"),
                      sizeof (Net_Remote_Comm::PongMessage)));
          return;
        } // end IF
        // step1: init reply
        Net_Remote_Comm::PongMessage* reply_struct_p =
          reinterpret_cast<Net_Remote_Comm::PongMessage*>(reply_message_p->wr_ptr ());
        ACE_OS::memset (reply_struct_p, 0, sizeof (Net_Remote_Comm::PongMessage));
        reply_struct_p->messageHeader.messageLength =
         sizeof (Net_Remote_Comm::PongMessage) - sizeof (unsigned int);
        reply_struct_p->messageHeader.messageType =
         Net_Remote_Comm::NET_PONG;
        reply_message_p->wr_ptr (sizeof (Net_Remote_Comm::PongMessage));
        // step2: send it upstream
        if (reply (reply_message_p, NULL) == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", aborting\n")));

          // clean up
          reply_message_p->release ();

          return;
        } // end IF
      } // end IF

      break;
    }
    case Net_Remote_Comm::NET_PONG:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("received PONG (connection ID: %u)...\n"),
      //            mySessionID));

      if (myPrintPongDot)
        std::clog << '.';

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%u]: unknown message type: \"%s\": protocol error, continuing\n"),
                  message_inout->getID (),
                  ACE_TEXT (Net_Message::CommandType2String (message_header_p->messageType).c_str ())));

      break;
    }
  } // end SWITCH
}

void
RPG_Net_Module_ProtocolHandler::handleSessionMessage (Net_SessionMessage*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);
  ACE_ASSERT (myIsInitialized);

  switch (message_inout->getType ())
  {
    case SESSION_BEGIN:
    {
      if (myPingInterval)
      {
        // schedule ourselves...
        ACE_Time_Value interval ((myPingInterval / 1000),
                                 ((myPingInterval % 1000) * 1000));
        ACE_ASSERT (myPingTimerID == -1);
        ACE_Event_Handler* eh = &myPingHandler;
        myPingTimerID =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (eh,                               // event handler
                                                                NULL,                             // ACT
                                                                COMMON_TIME_POLICY () + interval, // first wakeup time
                                                                interval);                        // interval
        if (myPingTimerID == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Common_Timer_Manager::schedule(), aborting\n")));
           return;
        } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("scheduled \"ping\" timer (ID: %d), interval: %u second(s)...\n"),
  //           myPingTimerID,
  //           pingInterval_in));
      } // end IF

      break;
    }
    case SESSION_END:
    {
      if (myPingTimerID != -1)
      {
        const void* act_p = NULL;
        if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (myPingTimerID,
                                                                &act_p) == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
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
RPG_Net_Module_ProtocolHandler::handleTimeout (const void* arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::handleTimeout"));

  ACE_UNUSED_ARG (arg_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("timer (ID: %d) expired...sending ping\n"),
//              myTimerID));

  // step0: create ping structure --> get a message buffer
  Net_Message* ping_message_p =
   allocateMessage (sizeof (Net_Remote_Comm::PingMessage));
  if (!ping_message_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate ping message(%u), returning\n"),
                sizeof (Net_Remote_Comm::PingMessage)));
    return;
  } // end IF

  // step1: init ping data
  // *TODO*: clean this up and handle endianness consistently !
  Net_Remote_Comm::PingMessage* ping_struct_p =
    reinterpret_cast<Net_Remote_Comm::PingMessage*> (ping_message_p->wr_ptr ());
  ACE_OS::memset (ping_struct_p, 0, sizeof (Net_Remote_Comm::PingMessage));
  ping_struct_p->messageHeader.messageLength =
    (sizeof (Net_Remote_Comm::PingMessage) -
     sizeof (unsigned int));
  ping_struct_p->messageHeader.messageType = Net_Remote_Comm::NET_PING;
  ping_struct_p->counter = myCounter++;
  ping_message_p->wr_ptr (sizeof (Net_Remote_Comm::PingMessage));

  // step2: send it upstream
  if (reply (ping_message_p, NULL) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", returning\n")));

    // clean up
    ping_message_p->release ();

    return;
  } // end IF
}

void
RPG_Net_Module_ProtocolHandler::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

Net_Message*
RPG_Net_Module_ProtocolHandler::allocateMessage (unsigned int requestedSize_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_ProtocolHandler::allocateMessage"));

  // init return value(s)
  Net_Message* message_out = NULL;

  try
  {
    message_out =
     static_cast<Net_Message*> (myAllocator->malloc (requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
                requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_IAllocator::malloc(%u), aborting\n"),
                requestedSize_in));
  } // end IF

  return message_out;
}
