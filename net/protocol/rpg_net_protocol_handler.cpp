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

#include "rpg_net_protocol_handler.h"

#include <iostream>

#include "common_timer_manager_common.h"

#include "stream_iallocator.h"

#include "rpg_common_macros.h"

#include "rpg_net_remote_comm.h"

#include "rpg_net_protocol_network.h"

const char rpg_net_protocol_handler_module_name_string[] =
  ACE_TEXT_ALWAYS_CHAR ("ProtocolHandler");

#if defined (ACE_WIN32) || defined (ACE_WIN64)
RPG_Net_Protocol_Handler::RPG_Net_Protocol_Handler (ISTREAM_T* stream_in)
#else
RPG_Net_Protocol_Handler::RPG_Net_Protocol_Handler (typename inherited::ISTREAM_T* stream_in)
#endif // ACE_WIN32 || ACE_WIN64
 : inherited (stream_in)
 , pingHandler_ (this,  // dispatch ourselves
                 false) // ping peer at regular intervals...
 , timerId_ (-1)
 , connectionId_ (ACE_INVALID_HANDLE)
 , counter_ (1)
 , sessionId_ (0)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::RPG_Net_Protocol_Handler"));

}

RPG_Net_Protocol_Handler::~RPG_Net_Protocol_Handler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::~RPG_Net_Protocol_Handler"));

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
RPG_Net_Protocol_Handler::initialize (const struct RPG_Net_Protocol_ModuleHandlerConfiguration& configuration_in,
                                      Stream_IAllocator* allocator_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::initialize"));

  int result = -1;

  if (inherited::TASK_BASE_T::isInitialized_)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: re-initializing...\n"),
                inherited::mod_->name ()));

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
                    ACE_TEXT ("%s: cancelled \"ping\" timer (id: %d)\n"),
                    inherited::mod_->name (),
                    timerId_));
      timerId_ = -1;
    } // end IF
    counter_ = 1;

//    result = parserQueue_.activate ();
//    if (unlikely (result == -1))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("%s: failed to ACE_Message_Queue::activate(): \"%m\", aborting\n"),
//                  inherited::mod_->name ()));
//      return false;
//    } // end IF
  } // end IF

  connectionId_ = ACE_INVALID_HANDLE;
  sessionId_ = 0;

  return inherited::initialize (configuration_in,
                                allocator_in);
}

// void
// RPG_Net_Protocol_Handler::handleDataMessage (RPG_Net_Protocol_Message*& message_inout,
//                                             bool& passMessageDownstream_out)
// {
//   RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::handleDataMessage"));

//   int result = -1;

//   // don't care (implies yes per default, if part of a stream)
//   ACE_UNUSED_ARG (passMessageDownstream_out);

//   // retrieve type of message and other details...
//   RPG_Net_Remote_Comm::MessageHeader* message_header_p =
//     reinterpret_cast<RPG_Net_Remote_Comm::MessageHeader*> (message_inout->rd_ptr ());
//   switch (message_header_p->messageType)
//   {
//     case RPG_Net_Remote_Comm::RPG_NET_PING:
//     {
//       // auto-answer ?
//       if (inherited::configuration_->protocolOptions->pingAutoAnswer)
//       {
//         // --> reply with a "PONG"

//         // step0: create reply structure
//         // --> get a message buffer
//         RPG_Net_Protocol_Message* message_p =
//           allocateMessage (sizeof (RPG_Net_Remote_Comm::PongMessage));
//         if (!message_p)
//         {
//           ACE_DEBUG ((LM_ERROR,
//                       ACE_TEXT ("failed to allocate reply message(%u), aborting\n"),
//                       sizeof (RPG_Net_Remote_Comm::PongMessage)));
//           return;
//         } // end IF
//         // step1: initialize reply
//         RPG_Net_Remote_Comm::PongMessage* reply_struct_p =
//           reinterpret_cast<RPG_Net_Remote_Comm::PongMessage*>(message_p->wr_ptr ());
//         ACE_OS::memset (reply_struct_p, 0, sizeof (RPG_Net_Remote_Comm::PongMessage));
//         reply_struct_p->messageHeader.messageLength =
//          sizeof (RPG_Net_Remote_Comm::PongMessage) - sizeof (unsigned int);
//         reply_struct_p->messageHeader.messageType =
//          RPG_Net_Remote_Comm::RPG_NET_PONG;
//         message_p->wr_ptr (sizeof (RPG_Net_Remote_Comm::PongMessage));
//         // step2: send it upstream
//         result = inherited::reply (message_p, NULL);
//         if (result == -1)
//         {
//           ACE_DEBUG ((LM_ERROR,
//                       ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", aborting\n")));
//           message_p->release (); message_p = NULL;
//           return;
//         } // end IF
//       } // end IF

//       break;
//     }
//     case RPG_Net_Remote_Comm::RPG_NET_PONG:
//     {
//       //ACE_DEBUG ((LM_DEBUG,
//       //            ACE_TEXT ("received PONG (connection ID: %u)...\n"),
//       //            mySessionID));

//       if (inherited::configuration_->protocolOptions->printPongMessages)
//         std::clog << '.';

//       break;
//     }
//     default:
//     {
//       ACE_DEBUG ((LM_ERROR,
//                   ACE_TEXT ("[%u:%u]: unknown message type: \"%s\": protocol error, continuing\n"),
//                   message_inout->sessionId (), message_inout->id (),
//                   ACE_TEXT (RPG_Net_Protocol_Message::CommandTypeToString (message_header_p->messageType).c_str ())));
//       break;
//     }
//   } // end SWITCH
// }

void
RPG_Net_Protocol_Handler::handleSessionMessage (RPG_Net_Protocol_SessionMessage*& message_inout,
                                                bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);
  ACE_ASSERT (inherited::TASK_BASE_T::configuration_);
  ACE_ASSERT (inherited::TASK_BASE_T::configuration_->protocolOptions);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      const RPG_Net_Protocol_SessionData_t& session_data_container_r =
        message_inout->getR ();
      struct RPG_Net_Protocol_SessionData& session_data_r =
        const_cast<struct RPG_Net_Protocol_SessionData&> (session_data_container_r.getR ());
      ACE_ASSERT (!session_data_r.connection);
      ACE_ASSERT (!session_data_r.connectionStates.empty ());
      ACE_HANDLE handle_h =
        (*session_data_r.connectionStates.begin ()).first;
      ACE_ASSERT (handle_h != ACE_INVALID_HANDLE);
      RPG_Net_Protocol_Connection_Manager_t* connection_manager_p =
        RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ();
      ACE_ASSERT (connection_manager_p);
      session_data_r.connection = connection_manager_p->get (handle_h);
      ACE_ASSERT (session_data_r.connection);

      ACE_INET_Addr local_address, peer_address;
      session_data_r.connection->info (connectionId_,
                                       local_address,
                                       peer_address);
      sessionId_ = message_inout->sessionId ();

      if (inherited::TASK_BASE_T::configuration_->protocolOptions->pingInterval != ACE_Time_Value::zero)
      {
        // schedule ourselves...
        ACE_ASSERT (timerId_ == -1);
        ACE_Event_Handler* handler_p = &pingHandler_;
        timerId_ =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (handler_p,                                                                  // event handler
                                                                NULL,                                                                       // ACT
                                                                COMMON_TIME_NOW + inherited::TASK_BASE_T::configuration_->protocolOptions->pingInterval, // first wakeup time
                                                                inherited::TASK_BASE_T::configuration_->protocolOptions->pingInterval);                  // interval
        if (unlikely (timerId_ == -1))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Common_Timer_Manager::schedule(), aborting\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: session %u: scheduled \"ping\" timer (id: %d), interval: %#T...\n"),
                    inherited::mod_->name (),
                    sessionId_,
                    timerId_,
                    &inherited::TASK_BASE_T::configuration_->protocolOptions->pingInterval));
      } // end IF

      result = inherited::activate ();
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Task_T::activate(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF

      break;

error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

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
                      ACE_TEXT ("%s: session %u: failed to cancel \"ping\" timer (ID: %d): \"%m\", continuing\n"),
                      inherited::mod_->name (),
                      sessionId_,
                      timerId_));
        else
         ACE_DEBUG ((LM_DEBUG,
                     ACE_TEXT("%s: session %u: cancelled \"ping\" timer (ID: %d)\n"),
                     inherited::mod_->name (),
                     sessionId_,
                     timerId_));
        timerId_ = -1;
      } // end IF

      if (likely (inherited::thr_count_))
      {
        inherited::stop ();
        inherited::wait ();
      } // end IF

//      result = parserQueue_.deactivate ();
//      if (unlikely (result == -1))
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("%s: failed to ACE_Message_Queue::deactivate(): \"%m\", continuing\n"),
//                    inherited::mod_->name ()));
      result = parserQueue_.flush ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Message_Queue::flush(): \"%m\", continuing\n"),
                    inherited::mod_->name ()));
      else if (result)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: flushed %d messages\n"),
                    inherited::mod_->name (),
                    result));

      if (inherited::headFragment_)
      {
        inherited::headFragment_->release (); inherited::headFragment_ = NULL;
      } // end IF

      if (inherited::resetQueue_)
        inherited::TASK_BASE_T::configuration_->parserConfiguration->messageQueue = NULL;

      const RPG_Net_Protocol_SessionData_t& session_data_container_r =
        message_inout->getR ();
      struct RPG_Net_Protocol_SessionData& session_data_r =
        const_cast<struct RPG_Net_Protocol_SessionData&> (session_data_container_r.getR ());
      if (session_data_r.connection)
      {
        session_data_r.connection->decrease (); session_data_r.connection = NULL;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH
}

void
RPG_Net_Protocol_Handler::handle (const void* arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::handle"));

  // int result = -1;

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
                ACE_TEXT ("%s: failed to allocate ping message(%u), returning\n"),
                inherited::mod_->name (),
                sizeof (RPG_Net_Remote_Comm::PingMessage)));
    return;
  } // end IF

  // step1: initialize ping data
  // *TODO*: clean this up and handle endianness consistently !
  // RPG_Net_Remote_Comm::PingMessage* ping_struct_p =
  //   reinterpret_cast<RPG_Net_Remote_Comm::PingMessage*> (message_p->wr_ptr ());
  // ACE_OS::memset (ping_struct_p, 0, sizeof (RPG_Net_Remote_Comm::PingMessage));
  // ping_struct_p->messageHeader.messageLength =
  //   (sizeof (RPG_Net_Remote_Comm::PingMessage) -
  //    sizeof (unsigned int));
  // ping_struct_p->messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PING;
  // ping_struct_p->counter = counter_++;
  // message_p->wr_ptr (sizeof (RPG_Net_Remote_Comm::PingMessage));

  // step2: send it upstream
  // result = inherited::reply (message_p, NULL);
  // if (result == -1)
  // {
  //   ACE_DEBUG ((LM_ERROR,
  //               ACE_TEXT ("failed to ACE_Task::reply(): \"%m\", returning\n")));
  //   message_p->release (); message_p = NULL;
  //   return;
  // } // end IF
  delete message_p;
}

void
RPG_Net_Protocol_Handler::record (struct RPG_Net_Protocol_Command& record_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::record"));

  // sanity check(s)
  ACE_ASSERT (inherited::TASK_BASE_T::configuration_);
  ACE_ASSERT (inherited::TASK_BASE_T::configuration_->allocatorConfiguration);

  RPG_Net_Protocol_Message* message_p =
    allocateMessage (inherited::TASK_BASE_T::configuration_->allocatorConfiguration->defaultBufferSize);
  if (unlikely (!message_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to allocate message(%u), returning\n"),
                inherited::mod_->name (),
                inherited::TASK_BASE_T::configuration_->allocatorConfiguration->defaultBufferSize));
    return;
  } // end IF

  struct RPG_Net_Protocol_MessageData message_data_s;
  message_data_s.connectionId = connectionId_;
  message_data_s.command = record_in;
  message_p->initialize (message_data_s,
                         sessionId_,
                         NULL);

  int result = inherited::put_next (message_p,
                                    NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task::put_next(), returning\n"),
                inherited::mod_->name ()));
    delete message_p;
    return;
  } // end IF

  // *NOTE*: free parsed data... Note that switchBuffer() will run cont() on
  //         headFragment_ in yywrap(), so do NOT free it in the edge case
  //         if length_i === bytes_to_release_i OR headFragment_->cont() == NULL
  ACE_ASSERT (inherited::scannedBytes_ <= inherited::length_);
  if (inherited::scannedBytes_ >= inherited::length_)
  {
    size_t bytes_to_release_i = inherited::scannedBytes_;
    while (bytes_to_release_i)
    {
      size_t length_i = inherited::headFragment_->length ();
      ACE_Message_Block* message_block_p = inherited::headFragment_->cont ();
      if ((length_i < bytes_to_release_i) && message_block_p)
      {
        inherited::headFragment_->cont (NULL);
        inherited::headFragment_->release (); inherited::headFragment_ = NULL;
        inherited::headFragment_ =
          static_cast<RPG_Net_Protocol_Message*> (message_block_p);
        inherited::fragment_ = inherited::headFragment_;
        bytes_to_release_i -= length_i;
        // break if length_i == 0
        if (!length_i)
        {
          inherited::PARSER_DRIVER_T::finished_ = true;
          break;
        } // end IF
      } // end IF
      else
      {
        if (length_i <= bytes_to_release_i)
        { // *TODO*: "frame" messages instead ?
          inherited::headFragment_->rd_ptr (length_i);
          if (length_i == bytes_to_release_i)
            inherited::PARSER_DRIVER_T::finished_ = true;
          break;
        } // end ELSE
        else
        { // *TODO*: "frame" messages instead ?
          ACE_ASSERT (length_i > bytes_to_release_i);
          inherited::headFragment_->rd_ptr (bytes_to_release_i);
          break;
        } // end ELSE
      } // end ELSE
    } // end WHILE
  } // end IF
}

void
RPG_Net_Protocol_Handler::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Protocol_Message*
RPG_Net_Protocol_Handler::allocateMessage (unsigned int requestedSize_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Handler::allocateMessage"));

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
