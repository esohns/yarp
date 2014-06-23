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

#include "rpg_stream_session_message.h"

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::RPG_Stream_Task()
 : inherited(NULL, // thread manager instance
             NULL) // queue handle
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::RPG_Stream_Task"));

  // use the default reactor...
  inherited::reactor(ACE_Reactor::instance());
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::~RPG_Stream_Task()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::~RPG_Stream_Task"));

}

// *** dummy stub methods ***
template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::open"));

  ACE_UNUSED_ARG(args_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::close"));

  ACE_UNUSED_ARG(arg_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::module_closed"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::put(ACE_Message_Block* mb_in,
                                     ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::put"));

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

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
int
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::svc"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}
// *** END dummy stub methods ***

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::handleSessionMessage(RPG_Stream_SessionMessage*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::handleSessionMessage"));

  // init return value(s)
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // *NOTE*: the default behavior is to simply dump state at the end of a
  // session...
  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STEP:
      break;
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
      break;
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      try
      {
        dump_state();
      }
      catch (...)
      {
        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught exception in dump_state(), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::name())));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in dump_state(), continuing\n")));
      }

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
      break;
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

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::handleProcessingError(const ACE_Message_Block* const message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::handleProcessingError"));

  ACE_UNUSED_ARG(message_in);

  if (inherited::module())
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("module: \"%s\": failed to process message, continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(inherited::name())));
  else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to process message, continuing\n")));
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::dump_state"));

//   if (inherited::module())
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT(" ***** MODULE: \"%s\" has not implemented the dump_state() API *****\n"),
//                ACE_TEXT_ALWAYS_CHAR(inherited::name())));
//   else
//     ACE_DEBUG((LM_WARNING,
//                ACE_TEXT("dump_state() API not implemented\n")));
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::handleMessage(ACE_Message_Block* mb_in,
                                               bool& stopProcessing_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::handleMessage"));

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
      RPG_Stream_MessageBase* message = NULL;
      // downcast message
      message = dynamic_cast<RPG_Stream_MessageBase*> (mb_in);
//       // *OPTIMIZATION*: not as safe, but (arguably) a lot faster !...
//       message = static_cast<RPG_Stream_MessageBase*>(mb_in);
      if (!message)
      {
        std::string type;
        RPG_Stream_MessageBase::MessageType2String(mb_in->msg_type(),
                                               type);

        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": dynamic_cast<RPG_Stream_MessageBase*)> (type: \"%s\" failed, aborting\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::module()->name()),
                     type.c_str()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("dynamic_cast<RPG_Stream_MessageBase*)> (type: \"%s\" failed, aborting\n"),
                     type.c_str()));

        // clean up
        mb_in->release();

        return;
      } // end IF

      // OK: process data...
      try
      {
        // invoke specific implementation...
        handleDataMessage(message,
                          passMessageDownstream);
      }
      catch (...)
      {
        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleDataMessage() (message ID: %u), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::name()),
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

        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleControlMessage() (type: \"%s\"), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::name()),
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
    if (!inherited::module())
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("cannot put_next(): not a module, continuing\n")));

      // clean up
      mb_in->release();
    } // end IF
    else if (inherited::put_next(mb_in, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to put_next(): \"%m\", continuing\n")));

      // clean up
      mb_in->release();
    } // end IF
  } // end IF
}

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
void
RPG_Stream_Task<TaskSynchStrategyType,
                TimePolicyType>::handleControlMessage(ACE_Message_Block* controlMessage_in,
                                                      bool& stopProcessing_out,
                                                      bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Task::handleControlMessage"));

  // init return value(s)
  stopProcessing_out = false;
  passMessageDownstream_out = true;

  switch (controlMessage_in->msg_type())
  {
    // currently, we only use these...
    case RPG_Stream_MessageBase::MB_STREAM_SESSION:
    {
      RPG_Stream_SessionMessage* sessionMessage = NULL;
      // downcast message
      sessionMessage = dynamic_cast<RPG_Stream_SessionMessage*> (controlMessage_in);
      if (!sessionMessage)
      {
        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": dynamic_cast<type: %d) failed> (aborting\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::name()),
                     controlMessage_in->msg_type()));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("dynamic_cast<type: %d) failed> (aborting\n"),
                     controlMessage_in->msg_type()));

        // clean up
        passMessageDownstream_out = false;
        controlMessage_in->release();

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
        if (inherited::module())
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("module \"%s\": caught an exception in handleSessionMessage(), continuing\n"),
                     ACE_TEXT_ALWAYS_CHAR(inherited::name())));
        else
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught an exception in handleSessionMessage(), continuing\n")));
      }

      // *NOTE*: if this was a RPG_Stream_SessionMessage::MB_STREAM_SESSION_END, we need to
      // stop processing (see above) !
      if (sessionMessage->getType() == RPG_Stream_SessionMessage::MB_STREAM_SESSION_END)
      {
        // OK: tell worker thread to stop whatever it's doing ASAP...
        stopProcessing_out = true;
      } // end IF

      break;
    }
    default:
    {
      // *NOTE*: if someone defines his own control message type and enqueues it
      // on the stream, it will land here (this is just a sanity check warning...)
      if (inherited::module())
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("module \"%s\": received an unknown control message (type: %d), continuing\n"),
                   ACE_TEXT_ALWAYS_CHAR(inherited::name()),
                   controlMessage_in->msg_type()));
      else
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("received an unknown control message (type: %d), continuing\n"),
                   controlMessage_in->msg_type()));

      break;
    }
  } // end SWITCH
}
