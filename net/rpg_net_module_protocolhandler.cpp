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

#include "rpg_net_module_protocolHandler.h"

#include <iostream>

#include "common_timer_manager_common.h"

#include "stream_iallocator.h"

#include "rpg_common_macros.h"

#include "rpg_net_remote_comm.h"

const char rpg_net_protocolhandler_module_name_string[] =
  ACE_TEXT_ALWAYS_CHAR ("ProtocolHandler");

RPG_Net_ProtocolHandler::RPG_Net_ProtocolHandler (ISTREAM_T* stream_in)
 : inherited (stream_in)
 , pingHandler_ (this,  // dispatch ourselves
                 false) // ping peer at regular intervals...
 , timerId_ (-1)
 //, allocator_ (NULL)
 , sessionId_ (0)
 , counter_ (1)
 , pingInterval_ (0) // [0: --> OFF]
 , automaticPong_ (true)
 , printPongDot_ (false)
 , isInitialized_ (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::RPG_Net_ProtocolHandler"));

}

RPG_Net_ProtocolHandler::~RPG_Net_ProtocolHandler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::~RPG_Net_ProtocolHandler"));

  int result = -1;

  // clean up timer if necessary
  if (unlikely (timerId_ != -1))
  {
    const void* act_p = NULL;
    result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timerId_,
                                                                 &act_p);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to cancel \"ping\" timer (id: %d): \"%m\", continuing\n"),
                  inherited::mod_->name (),
                  timerId_));
    else
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("%s: cancelled \"ping\" timer (id: %d)\n"),
                  inherited::mod_->name (),
                  timerId_));
  } // end IF
}

bool
RPG_Net_ProtocolHandler::initialize (Stream_IAllocator* allocator_in,
                                     unsigned int pingInterval_in,
                                     bool autoAnswerPings_in,
                                     bool printPongDot_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::initialize"));

  int result = -1;

  if (isInitialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    // reset state
    if (timerId_ != -1)
    {
      const void* act_p = NULL;
      result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timerId_,
                                                                   &act_p);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to cancel \"ping\" timer (id: %d): \"%m\", continuing\n"),
                    inherited::mod_->name (),
                    timerId_));
      else
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("cancelled \"ping\" timer (id: %d)\n"),
                    inherited::mod_->name (),
                    timerId_));
      timerId_ = -1;
    } // end IF
    counter_ = 1;

    isInitialized_ = false;
  } // end IF

  allocator_ = allocator_in;
  sessionId_ = 0;
  pingInterval_ = pingInterval_in;
  automaticPong_ = autoAnswerPings_in;
  //if (automaticPong_)
  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("auto-answering \"ping\" messages\n")));
  printPongDot_ = printPongDot_in;

  isInitialized_ = true;

  return isInitialized_;
}

void
RPG_Net_ProtocolHandler::handleDataMessage (RPG_Net_Protocol_Message*& message_inout,
                                            bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::handleDataMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // retrieve type of message and other details...
  RPG_Net_Remote_Comm::MessageHeader* message_header_p =
    reinterpret_cast<RPG_Net_Remote_Comm::MessageHeader*> (message_inout->rd_ptr ());
  switch (message_header_p->messageType)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      // auto-answer ?
      if (automaticPong_)
      {
        // --> reply with a "PONG"

        // step0: create reply structure
        // --> get a message buffer
        RPG_Net_Protocol_Message* message_p =
          allocateMessage (sizeof (RPG_Net_Remote_Comm::PongMessage));
        if (!message_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to allocate reply message(%u), aborting\n"),
                      sizeof (RPG_Net_Remote_Comm::PongMessage)));
          return;
        } // end IF
        // step1: initialize reply
        RPG_Net_Remote_Comm::PongMessage* reply_struct_p =
          reinterpret_cast<RPG_Net_Remote_Comm::PongMessage*>(message_p->wr_ptr ());
        ACE_OS::memset (reply_struct_p, 0, sizeof (RPG_Net_Remote_Comm::PongMessage));
        reply_struct_p->messageHeader.messageLength =
         sizeof (RPG_Net_Remote_Comm::PongMessage) - sizeof (unsigned int);
        reply_struct_p->messageHeader.messageType =
         RPG_Net_Remote_Comm::RPG_NET_PONG;
        message_p->wr_ptr (sizeof (RPG_Net_Remote_Comm::PongMessage));
        // step2: send it upstream
        result = inherited::reply (message_p, NULL);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", aborting\n")));
          message_p->release (); message_p = NULL;
          return;
        } // end IF
      } // end IF

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("received PONG (connection ID: %u)...\n"),
      //            mySessionID));

      if (printPongDot_)
        std::clog << '.';

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%u:%u]: unknown message type: \"%s\": protocol error, continuing\n"),
                  message_inout->sessionId (), message_inout->id (),
                  ACE_TEXT (RPG_Net_Protocol_Message::CommandTypeToString (message_header_p->messageType).c_str ())));
      break;
    }
  } // end SWITCH
}

void
RPG_Net_ProtocolHandler::handleSessionMessage (RPG_Net_Protocol_SessionMessage*& message_inout,
                                               bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);
  ACE_ASSERT (isInitialized_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      sessionId_ = message_inout->sessionId ();

      if (pingInterval_)
      {
        // schedule ourselves...
        ACE_Time_Value interval ((pingInterval_ / 1000),
                                 ((pingInterval_ % 1000) * 1000));
        ACE_ASSERT (timerId_ == -1);
        ACE_Event_Handler* handler_p = &pingHandler_;
        timerId_ =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (handler_p,                  // event handler
                                                                NULL,                       // ACT
                                                                COMMON_TIME_NOW + interval, // first wakeup time
                                                                interval);                  // interval
        if (unlikely (timerId_ == -1))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Common_Timer_Manager::schedule(), aborting\n")));
          return;
        } // end IF
#if defined (_DEBUG)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("session %u: scheduled \"ping\" timer (id: %d), interval: %#T...\n"),
                    sessionId_,
                    timerId_,
                    &interval));
#endif // _DEBUG
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      if (timerId_ != -1)
      {
        const void* act_p = NULL;
        result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timerId_,
                                                                     &act_p);
        if (unlikely (result == -1))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
                      sessionId_,
                      timerId_));
#if defined (_DEBUG)
        else
         ACE_DEBUG ((LM_DEBUG,
                     ACE_TEXT("session %u: cancelled \"ping\" timer (ID: %d)\n"),
                     sessionId_,
                     timerId_));
#endif // _DEBUG
        timerId_ = -1;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH
}

void
RPG_Net_ProtocolHandler::handle (const void* arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::handle"));

  int result = -1;

  ACE_UNUSED_ARG (arg_in);

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("timer (ID: %d) expired...sending ping\n"),
//               timerId_));

  // step0: create ping structure --> get a message buffer
  RPG_Net_Protocol_Message* message_p =
   allocateMessage (sizeof (RPG_Net_Remote_Comm::PingMessage));
  if (!message_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate ping message(%u), returning\n"),
                sizeof (RPG_Net_Remote_Comm::PingMessage)));
    return;
  } // end IF

  // step1: initialize ping data
  // *TODO*: clean this up and handle endianness consistently !
  RPG_Net_Remote_Comm::PingMessage* ping_struct_p =
    reinterpret_cast<RPG_Net_Remote_Comm::PingMessage*> (message_p->wr_ptr ());
  ACE_OS::memset (ping_struct_p, 0, sizeof (RPG_Net_Remote_Comm::PingMessage));
  ping_struct_p->messageHeader.messageLength =
    (sizeof (RPG_Net_Remote_Comm::PingMessage) -
     sizeof (unsigned int));
  ping_struct_p->messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PING;
  ping_struct_p->counter = counter_++;
  message_p->wr_ptr (sizeof (RPG_Net_Remote_Comm::PingMessage));

  // step2: send it upstream
  result = inherited::reply (message_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", returning\n")));
    message_p->release (); message_p = NULL;
    return;
  } // end IF
}

void
RPG_Net_ProtocolHandler::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Protocol_Message*
RPG_Net_ProtocolHandler::allocateMessage (unsigned int requestedSize_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_ProtocolHandler::allocateMessage"));

  // initialize return value(s)
  RPG_Net_Protocol_Message* message_p = NULL;

  if (likely (inherited::allocator_))
  {
allocate:
    try {
      message_p =
        static_cast<RPG_Net_Protocol_Message*> (inherited::allocator_->malloc (requestedSize_in));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
                  requestedSize_in));
      return NULL;
    }

    // keep retrying ?
    if (!message_p &&
        !inherited::allocator_->block ())
      goto allocate;
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      RPG_Net_Protocol_Message (requestedSize_in));
  if (unlikely (!message_p))
  {
    if (inherited::allocator_)
    {
      if (inherited::allocator_->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate message: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate message: \"%m\", aborting\n")));
  } // end IF

  return message_p;
}
