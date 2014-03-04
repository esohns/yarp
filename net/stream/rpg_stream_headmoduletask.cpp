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

#include "rpg_stream_headmoduletask.h"

#include <ace/Message_Block.h>

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_defines.h"
#include "rpg_stream_message_base.h"
#include "rpg_stream_session_config.h"
#include "rpg_stream_iallocator.h"

RPG_Stream_HeadModuleTask::RPG_Stream_HeadModuleTask(const bool& autoStart_in)
 : myAllocator(NULL),
   mySessionID(0),
   myCondition(myLock),
   myIsFinished(true),
   myQueue(RPG_STREAM_MAX_QUEUE_SLOTS),
   myAutoStart(autoStart_in),
   myUserData(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::RPG_Stream_HeadModuleTask"));

  // tell the task to use our message queue...
  msg_queue(&myQueue);

  // set group ID for worker thread(s)
  grp_id(RPG_STREAM_TASK_GROUP_ID);
}

RPG_Stream_HeadModuleTask::~RPG_Stream_HeadModuleTask()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::~RPG_Stream_HeadModuleTask"));

  // flush/deactivate our queue (check whether it was empty...)
  int flushed_messages = 0;
  flushed_messages = myQueue.flush();

  // *IMPORTANT NOTE*: this should NEVER happen !
  // debug info
  if (flushed_messages)
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("flushed %d message(s)...\n"),
               flushed_messages));

//   // *TODO*: check if this sequence actually works...
//   myQueue.deactivate();
//   myQueue.wait();
}

int
RPG_Stream_HeadModuleTask::put(ACE_Message_Block* mb_in,
                               ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::put"));

  // drop the message into our queue...
  return putq(mb_in,
              tv_in);
}

int
RPG_Stream_HeadModuleTask::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::open"));

  // step0: init user data
  myUserData = args_in;

  // step1: (re-)activate() our queue
  // *NOTE*: the first time around, our queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state
  // - hopefully, this is what we want.
  // If we come here a second time (i.e. we have been stopped/started, our queue
  // will have been deactivated in the process, and getq() (see svc()) will fail
  // miserably (ESHUTDOWN) --> (re-)activate() our queue !
  // step1: (re-)activate() our queue
  if (myQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Message_Queue::activate(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return -1;
  } // end IF

  // standard usecase: being implicitly invoked by ACE_Stream::push()...
  // --> don't do anything, unless we're auto-starting
  if (myAutoStart)
  {
    if (module())
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("auto-starting \"%s\"...\n"),
                 ACE_TEXT_ALWAYS_CHAR(name())));
    else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("auto-starting...\n")));

    try
    {
      start();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in start() method, aborting\n")));

      return -1;
    }
  } // end IF

  return 0;
}

int
RPG_Stream_HeadModuleTask::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::close"));

  // *NOTE*: this method may be invoked
  // - by an external thread closing down the active object
  //    --> should NEVER happen as a module !
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
//       // debug info
//       if (module())
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("\"%s\" worker thread (ID: %t) leaving...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(name())));
//       } // end IF
//       else
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("worker thread (ID: %t) leaving...\n")));
//       } // end ELSE

      break;
    }
    case 1:
    {
      // *WARNING*: SHOULD NEVER GET HERE
      // --> module_closed() hook is implemented below !!!
      ACE_ASSERT(false);

      return -1;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument: %u, aborting\n"),
                 arg_in));

      return -1;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Stream_HeadModuleTask::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::module_closed"));

  // *NOTE*: this will be a NOP IF the stream was
  // stop()ped BEFORE it is deleted !

  // *NOTE*: this method is invoked by an external thread
  // either from the ACE_Module dtor or during explicit ACE_Module::close()

  // *NOTE*: when we get here, we SHOULD ALREADY BE in a final state...

  // sanity check
  // *WARNING*: this check CANNOT prevent a potential race condition...
  if (isRunning())
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("isRunning, continuing\n")));

    try
    {
      stop();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in stop(), aborting\n")));

      return -1;
    }
  } // end IF

  // just wait for OUR worker thread to die
  // *NOTE*: this works because we assume that by the time we get here,
  // we're either stop()ed (see above) or otherwise finished with processing, i.e. our
  // worker thread should be dying/dead by now...
  wait();

  return 0;
}

int
RPG_Stream_HeadModuleTask::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::svc"));

  ACE_Message_Block* ace_mb          = NULL;
  bool               stop_processing = false;

  // step1: send initial session message downstream...
  if (!putSessionMessage(mySessionID,
                         RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN,
                         myUserData,
												 RPG_COMMON_TIME_POLICY(), // start of session
                         false))                   // N/A
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage(SESSION_BEGIN) failed, aborting\n")));

    // signal the controller
    finished();

    return -1;
  } // end IF

  // step2: start processing incoming data...
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("entering processing loop...\n")));

  while (inherited::getq(ace_mb,
                         NULL) != -1)
  {
    inherited::handleMessage(ace_mb,
                             stop_processing);

    // finished ?
    if (stop_processing)
    {
      // *WARNING*: ace_mb has already been released() at this point !

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("leaving processing loop...\n")));

      // step3: send final session message downstream...
      if (!putSessionMessage(mySessionID,
                             RPG_Stream_SessionMessage::MB_STREAM_SESSION_END,
                             myUserData,
                             ACE_Time_Value::zero, // N/A
                             true))                // ALWAYS a user abort...
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("putSessionMessage(SESSION_END) failed, aborting\n")));

        // signal the controller
        finished();

        return -1;
      } // end IF

      // signal the controller
      finished();

      return 0;
    } // end IF

    // clean up
    ace_mb = NULL;
  } // end WHILE

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

  // step3: send final session message downstream...
  if (!putSessionMessage(mySessionID,
                         RPG_Stream_SessionMessage::MB_STREAM_SESSION_END,
                         myUserData,
                         ACE_Time_Value::zero, // N/A
                         false))               // N/A
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage(SESSION_END) failed, aborting\n")));

  // signal the controller
  finished();

  return -1;
}

// void
// RPG_Stream_HeadModuleTask::handleDataMessage(RPG_Stream_MessageBase*& message_inout,
//                                          bool& passMessageDownstream_out)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::handleDataMessage"));
//
//   ACE_UNUSED_ARG(message_inout);
//   ACE_UNUSED_ARG(passMessageDownstream_out);
//
//   // *NOTE*: should NEVER be reached !
//   ACE_ASSERT(false);
//
//   // what else can we do ?
//   ACE_NOTREACHED(return;)
// }

void
RPG_Stream_HeadModuleTask::handleControlMessage(ACE_Message_Block* controlMessage_in,
                                                bool& stopProcessing_out,
                                                bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::handleControlMessage"));

  // init return value(s)
  stopProcessing_out = false;

  switch (controlMessage_in->msg_type())
  {
    // currently, we only handle these...
    // --> send a SESSION_END message downstream to tell the other modules...
    case ACE_Message_Block::MB_STOP:
    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("received MB_STOP message, shutting down...\n")));

      // clean up --> we DON'T pass these along...
      passMessageDownstream_out = false;
      controlMessage_in->release();

      // OK: tell our worker thread to stop whatever it's doing ASAP
      // *NOTE*: this triggers forwarding a SESSION_END message to notify any
      // modules downstream !
      stopProcessing_out = true;

      break;
    }
    default:
    {
      // otherwise, just behave like a regular module...
      inherited::handleControlMessage(controlMessage_in,
                                      stopProcessing_out,
                                      passMessageDownstream_out);

      break;
    }
  } // end SWITCH
}

void
RPG_Stream_HeadModuleTask::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::start"));

  // (try to) change state
  // --> start a worker thread
  changeState(inherited2::RUNNING);
}

void
RPG_Stream_HeadModuleTask::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::stop"));

  // (try to) change state
  // --> tell the worker thread to die
  changeState(inherited2::STOPPED);

  // ...and wait for it to happen
  //wait();
}

void
RPG_Stream_HeadModuleTask::pause()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::pause"));

  // (try to) change state
  changeState(inherited2::PAUSED);
}

void
RPG_Stream_HeadModuleTask::rewind()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::rewind"));

  // *TODO*: implement this !
  ACE_ASSERT(false);
}

void
RPG_Stream_HeadModuleTask::waitForCompletion()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::waitForCompletion"));

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    while (!myIsFinished)
      myCondition.wait();
  } // end lock scope

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("leaving...\n")));
}

const bool
RPG_Stream_HeadModuleTask::isRunning()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::isRunning"));

  return (getState() == inherited2::RUNNING);
}

void
RPG_Stream_HeadModuleTask::finished()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::finished"));

  // (try to) set new state
  changeState(inherited2::FINISHED);

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("leaving finished()...\n")));
}

void
RPG_Stream_HeadModuleTask::onStateChange(const Control_StateType& newState_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::onStateChange"));

  switch (newState_in)
  {
    case inherited2::INIT:
    {
      // OK: (re-)initialized
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("(re-)initialized...\n")));

      break;
    }
    case inherited2::RUNNING:
    {
      // *NOTE*: we want to implement tape-recorder logic:
      // PAUSED --> PAUSED is mapped to PAUSED --> RUNNING
      // --> check for this condition before we do anything else...
      if (getState() == inherited2::PAUSED)
      {
        // resume our worker thread
        if (inherited::resume() == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to resume(): \"%s\", continuing\n"),
                     ACE_OS::strerror(errno)));
        } // end IF

        // finished !
        break;
      } // end IF

      // OK: start worker thread
      ACE_thread_t thread_ids[1];
      thread_ids[0] = 0;
      ACE_hthread_t thread_handles[1];
      thread_handles[0] = 0;

      // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
      int result = 0;
      result = inherited::activate((THR_NEW_LWP |
                                    THR_JOINABLE |
                                    THR_INHERIT_SCHED),         // flags
                                   1,                           // number of threads
                                   0,                           // force spawning
                                   ACE_DEFAULT_THREAD_PRIORITY, // priority
                                   grp_id(),                    // group id --> should have been set by now !
                                   NULL,                        // corresp. task --> use 'this'
                                   thread_handles,              // thread handle(s)
                                   NULL,                        // thread stack(s)
                                   NULL,                        // thread stack size(s)
                                   thread_ids);                 // thread id(s)
      if (result == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to activate(): \"%m\", aborting\n")));

        // finished !
        break;
      } // end IF

      {
        // synchronize access to myIsFinished
        // *TODO*: synchronize access to state logic to make the API re-entrant...
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

        myIsFinished = false;
      } // end lock scope

//       if (module())
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("module \"%s\" started worker thread (group: %d, id: %u)...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(name()),
//                    grp_id(),
//                    thread_ids[0]));
//       else
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("started worker thread (group: %d, id: %u)...\n"),
//                    grp_id(),
//                    thread_ids[0]));

      break;
    }
    case inherited2::STOPPED:
    {
      // OK: drop control message into stream...
      // *TODO*: use ACE_Stream::control() instead ?
      ACE_Message_Block* stop_mb = NULL;
      ACE_NEW_NORETURN(stop_mb,
                       ACE_Message_Block(0,                                  // size
                                         ACE_Message_Block::MB_STOP,         // type
                                         NULL,                               // continuation
                                         NULL,                               // data
                                         NULL,                               // buffer allocator
                                         NULL,                               // locking strategy
                                         ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                         ACE_Time_Value::zero,               // execution time
                                         ACE_Time_Value::max_time,           // deadline time
                                         NULL,                               // data block allocator
                                         NULL));                             // message allocator)
      if (!stop_mb)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));

        break;
      } // end IF

      if (putq(stop_mb, NULL) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to putq(): \"%m\", continuing\n")));

        // clean up
        stop_mb->release();
      } // end IF

      break;
    }
    case inherited2::FINISHED:
    {
      // signal waiting thread(s)
      {
        // grab condition lock...
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

        myIsFinished = true;

        myCondition.broadcast();
      } // end lock scope

      // OK: (re-)initialized
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("finished successfully !\n")));

      break;
    }
    case inherited2::PAUSED:
    {
      // suspend our worker thread
      if (inherited::suspend() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to suspend(): \"%m\", continuing\n")));

      break;
    }
    default:
    {
      // *NOTE*: if we get here, an invalid/unknown state change happened...

      std::string currentStateString;
      std::string newStateString;
      ControlState2String(getState(),
                          currentStateString);
      ControlState2String(newState_in,
                          newStateString);
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid state switch: \"%s\" --> \"%s\", continuing\n"),
                 currentStateString.c_str(),
                 newStateString.c_str()));

      break;
    }
  } // end SWITCH
}

bool
RPG_Stream_HeadModuleTask::putSessionMessage(const unsigned int& sessionID_in,
                                             const RPG_Stream_SessionMessage::SessionMessageType& messageType_in,
                                             RPG_Stream_SessionConfig*& config_inout,
                                             RPG_Stream_IAllocator* allocator_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::putSessionMessage"));

  // create session message
  RPG_Stream_SessionMessage* message = NULL;
  if (allocator_in)
  {
    try
    {
      message = static_cast<RPG_Stream_SessionMessage*>(allocator_in->malloc(0)); // we want a session message !
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("caught exception in RPG_Stream_IAllocator::malloc(0), aborting\n")));

      // clean up
      config_inout->decrease();
      config_inout = NULL;

      return false;
    }
  } // end IF
  else
  { // *NOTE*: session message assumes responsibility for session_config !
    ACE_NEW_NORETURN(message,
                     RPG_Stream_SessionMessage(sessionID_in,
                                               messageType_in,
                                               config_inout));
  } // end ELSE

  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate RPG_Stream_SessionMessage: \"%m\", aborting\n")));

    // clean up
    config_inout->decrease();
    config_inout = NULL;

    return false;
  } // end IF
  if (allocator_in)
  { // *NOTE*: session message assumes responsibility for session_config !
    message->init(sessionID_in,
                  messageType_in,
                  config_inout);
  } // end IF

  // pass message downstream...
  if (const_cast<RPG_Stream_HeadModuleTask*>(this)->put_next(message, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to put_next(): \"%m\", aborting\n")));

    // clean up
    message->release();

    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("enqueued session message...\n")));

  return true;
}

bool
RPG_Stream_HeadModuleTask::putSessionMessage(const unsigned int& sessionID_in,
                                             const RPG_Stream_SessionMessage::SessionMessageType& messageType_in,
                                             const void* userData_in,
                                             const ACE_Time_Value& startOfSession_in,
                                             const bool& userAbort_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTask::putSessionMessage"));

  // create/collect session data
  RPG_Stream_SessionConfig* session_configuration = NULL;

  // switch
  switch (messageType_in)
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STEP:
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      ACE_NEW_NORETURN(session_configuration,
                       RPG_Stream_SessionConfig(userData_in,
                                                startOfSession_in,
                                                userAbort_in));
      if (!session_configuration)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate RPG_Stream_SessionConfig: \"%m\", aborting\n")));

        return false;
      } // end IF

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STATISTICS:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid/unknown message type: %d, aborting\n"),
                 messageType_in));

      return false;
    }
  } // end SWITCH

  // *NOTE*: this API is a "fire-and-forget" for session_configuration
  return putSessionMessage(sessionID_in,
                           messageType_in,
                           session_configuration,
                           myAllocator);
}
