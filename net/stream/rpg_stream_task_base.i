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

#include <ace/Reactor.h>
#include <ace/Message_Block.h>
#include <ace/Time_Value.h>

#include "rpg_common_macros.h"

#include "rpg_stream_message_base.h"
#include "rpg_stream_session_message_base.h"

template <typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::RPG_Stream_TaskBase()
 : inherited(NULL, // thread manager instance
             NULL) // message queue handle
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::RPG_Stream_TaskBase"));

  // use the default reactor...
  reactor(ACE_Reactor::instance());
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::~RPG_Stream_TaskBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::~RPG_Stream_TaskBase"));

}

// *** dummy stub methods ***
template <typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::open"));

  ACE_UNUSED_ARG(args_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::close"));

  ACE_UNUSED_ARG(arg_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::module_closed"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::put(ACE_Message_Block* mb_in,
                                              ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::put"));

  ACE_UNUSED_ARG(mb_in);
  ACE_UNUSED_ARG(tv_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::svc"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}
// *** END dummy stub methods ***

// template <typename SessionMessageType,
//           typename ProtocolMessageType>
// void
// RPG_Stream_TaskBase<SessionMessageType,
//                 ProtocolMessageType>::handleDataMessage(ProtocolMessageType*& message_inout,
//                                                         bool& passMessageDownstream_out)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::handleDataMessage"));
//
//   // init return value(s)
//   passMessageDownstream_out = true;
//
//   // sanity check(s)
//   ACE_ASSERT(message_inout);
//
//   ACE_UNUSED_ARG(message_inout);
// }

template <typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::handleSessionMessage(SessionMessageType*& message_inout,
                                                               bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::handleSessionMessage"));

  // init return value(s)
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // *NOTE*: the default behavior is to simply dump our state at the
  // end of a session...
  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STEP:
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      try
      {
        // OK: dump some status info...
        dump_state();
      }
      catch (...)
      {
        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught exception in dump_state(), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(name())));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in dump_state(), continuing\n")));
      }

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
    {
      break;
    }
    default:
    {
      std::string type_string;
      RPG_Stream_SessionMessage::SessionMessageType2String(message_inout->getType(),
                                                       type_string);
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("invalid/unknown session message (type: \"%s\")\n"),
                 type_string.c_str()));

      break;
    }
  } // end SWITCH
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::handleProcessingError(const ACE_Message_Block* const message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::handleProcessingError"));

  ACE_UNUSED_ARG(message_in);

  if (module())
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("module: \"%s\": failed to process message, continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(name())));
  else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to process message, continuing\n")));
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::dump_state"));

//   if (module())
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT(" ***** MODULE: \"%s\" has not implemented the dump_state() API *****\n"),
//                ACE_TEXT_ALWAYS_CHAR(name())));
//   else
//     ACE_DEBUG((LM_WARNING,
//                ACE_TEXT("dump_state() API not implemented\n")));
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::handleMessage(ACE_Message_Block* mb_in,
                                                        bool& stopProcessing_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::handleMessage"));

  // sanity check
  ACE_ASSERT(mb_in);

  // init return value(s)
  stopProcessing_out = false;

  // default behavior is to pass EVERYTHING downstream...
  bool passMessageDownstream = true;
  switch (mb_in->msg_type())
  {
    // DATA handling
    case RPG_Stream_MessageBase::MB_STREAM_DATA:
    case RPG_Stream_MessageBase::MB_STREAM_OBJ:
    {
      ProtocolMessageType* message = NULL;
      // downcast message
      message = dynamic_cast<ProtocolMessageType*>(mb_in);
//       // *OPTIMIZATION*: not as safe, but (arguably) a lot faster !...
//       message = static_cast<RPG_Stream_MessageBase*> (//                                 mb_in);
      if (!message)
      {
        std::string type;
        RPG_Stream_MessageBase::MessageType2String(mb_in->msg_type(),
                                                   type);

        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": dynamic_cast<RPG_Stream_MessageBase)> (type: \"%s\" failed, aborting\n"),
                     ACE_TEXT_ALWAYS_CHAR(module()->name()),
                     type.c_str()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("dynamic_cast<RPG_Stream_MessageBase)> (type: \"%s\" failed, aborting\n"),
                     type.c_str()));

        // clean up
        mb_in->release();

        return;
      } // end IF

      // OK: process data...
      try
      {
        // invoke specific implementation...
        // *WARNING*: need to invoke our OWN implementation here, otherwise, the ld linker
        // complains about a missing reference to StreamITaskBase::handleDataMessage...
//         inherited2::handleDataMessage(message,
//                                       passMessageDownstream);
        this->handleDataMessage(message,
                                passMessageDownstream);
      }
      catch (...)
      {
        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleDataMessage() (message ID: %u), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(name()),
                     message->getID()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught an exception in handleDataMessage() (message ID: %u), continuing\n"),
                     message->getID()));
      }

      break;
    }
    // *NOTE*: anything that is NOT data is defined as a control message...
    default:
    {
      // OK: process control message...
      try
      {
        // invoke specific implementation...
        handleControlMessage(mb_in,
                             stopProcessing_out,
                             passMessageDownstream);
      }
      catch (...)
      {
        std::string type;
        RPG_Stream_MessageBase::MessageType2String(mb_in->msg_type(),
                                                   type);

        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleControlMessage() (type: \"%s\"), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(name()),
                     type.c_str()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught an exception in handleControlMessage() (type: \"%s\"), continuing\n"),
                     type.c_str()));
      }

      break;
    }
  } // end SWITCH

  // pass message downstream (if there IS a stream)...
  if (passMessageDownstream)
  {
    // *NOTE*: tasks that are not part of a stream have no notion of
    // this concept, so we decide for them...
    if (!module())
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("cannot put_next(): not a module, continuing\n")));

      // clean up
      mb_in->release();
    } // end IF
    else if (put_next(mb_in, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to put_next(): \"%m\", continuing\n")));

      // clean up
      mb_in->release();
    } // end IF
  } // end IF
}

template <typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_TaskBase<SessionMessageType,
                    ProtocolMessageType>::handleControlMessage(ACE_Message_Block* controlMessage_in,
                                                               bool& stopProcessing_out,
                                                               bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskBase::handleControlMessage"));

  // init return value(s)
  stopProcessing_out = false;
  passMessageDownstream_out = true;

  switch (controlMessage_in->msg_type())
  {
    // currently, we only use these...
    case RPG_Stream_MessageBase::MB_STREAM_SESSION:
    {
      SessionMessageType* sessionMessage = NULL;
      // downcast message
      sessionMessage = dynamic_cast<SessionMessageType*>(controlMessage_in);
      if (!sessionMessage)
      {
        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": dynamic_cast<type: %d) failed> (aborting\n"),
                     ACE_TEXT_ALWAYS_CHAR(name()),
                     controlMessage_in->msg_type()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("dynamic_cast<type: %d) failed> (aborting\n"),
                     controlMessage_in->msg_type()));

        // clean up
        passMessageDownstream_out = false;
        controlMessage_in->release();

        // what else can we do ?
        break;
      } // end IF

      // OK: process session message...
      try
      {
        // invoke specific implementation...
        handleSessionMessage(sessionMessage,
                             passMessageDownstream_out);
      }
      catch (...)
      {
        if (module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleSessionMessage(), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(name())));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught an exception in handleSessionMessage(), continuing\n")));
      }

      // *NOTE*: if this was a RPG_Stream_SessionMessage::SESSION_END, we need to
      // stop processing (see above) !
      if (sessionMessage->getType() == RPG_Stream_SessionMessage::MB_STREAM_SESSION_END)
      {
        // OK: tell our worker thread to stop whatever it's doing ASAP...
        stopProcessing_out = true;
      } // end IF

      break;
    }
    default:
    {
      // *NOTE*: if someone defines their own control message type and enqueues it
      // on the stream, it will land here (this is just a sanity check warning...)
      if (module())
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("module \"%s\": received an unknown control message (type: %d), continuing\n"),
                   ACE_TEXT_ALWAYS_CHAR(name()),
                   controlMessage_in->msg_type()));
      else
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("received an unknown control message (type: %d), continuing\n"),
                   controlMessage_in->msg_type()));

      break;
    }
  } // end SWITCH
}
