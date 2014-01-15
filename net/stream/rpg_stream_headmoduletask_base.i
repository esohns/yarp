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

#include "rpg_common_defines.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_defines.h"
#include "rpg_stream_iallocator.h"

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::RPG_Stream_HeadModuleTaskBase(const bool& isActive_in,
                                                                                  const bool& autoStart_in)
 : myAllocator(NULL),
   mySessionID(0),
   myIsActive(isActive_in),
   myCondition(myLock),
   myCurrentNumThreads(RPG_STREAM_DEF_NUM_STREAM_HEAD_THREADS),
   myQueue(RPG_STREAM_MAX_QUEUE_SLOTS),
   myAutoStart(autoStart_in)//,
   //myUserData()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::RPG_Stream_HeadModuleTaskBase"));

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(myUserData));

  // tell the task to use our message queue...
  inherited::msg_queue(&myQueue);

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_STREAM_TASK_GROUP_ID);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::~RPG_Stream_HeadModuleTaskBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::~RPG_Stream_HeadModuleTaskBase"));

  // flush/deactivate our queue (check whether it was empty...)
  int flushed_messages = 0;
  flushed_messages = myQueue.flush();

  if (flushed_messages)
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("flushed %d message(s)...\n"),
               flushed_messages));

//   // *TODO*: check if this sequence actually works...
//   myQueue.deactivate();
//   myQueue.wait();
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::put(ACE_Message_Block* mb_in,
                                                        ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::put"));

  // if active, simply drop the message into the queue...
  if (myIsActive)
    return inherited::putq(mb_in, tv_in);

  // otherwise, process manually...
  bool stop_processing = false;
  inherited::handleMessage(mb_in,
                           stop_processing);

  // finished ?
  if (stop_processing)
  {
    // *WARNING*: mb_in has already been released() at this point !

    stop();
  } // end IF

  return 0;
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::open"));

  // sanity check
  // *WARNING*: DataType == void* --> args_in COULD be NULL...
//   ACE_ASSERT(args_in);

  // step0: init user data
  myUserData = *static_cast<DataType*>(args_in);

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
               ACE_TEXT("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));

    return -1;
  } // end IF

  // standard usecase: being implicitly invoked by ACE_Stream::push()...
  // --> don't do anything, unless auto-starting
  if (myAutoStart)
  {
    if (inherited::module())
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("auto-starting \"%s\"...\n"),
                 ACE_TEXT_ALWAYS_CHAR(inherited::name())));
    } // end IF
    else
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("auto-starting...\n")));
    } // end ELSE

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

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::close"));

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
//       if (inherited::module())
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("\"%s\" worker thread (ID: %t) leaving...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(inherited::name())));
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
      // --> refer to module_closed() hook
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

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::module_closed"));

  // *NOTE*: this will be a NOP IF the stream was
  // stop()ped BEFORE it is deleted !

  // *NOTE*: this method is invoked by an external thread
  // either from the ACE_Module dtor or during explicit ACE_Module::close()

  // *NOTE*: when we get here, we SHOULD ALREADY BE in a final state...

  // sanity check
  // *WARNING*: this check CAN NOT prevent a potential race condition...
  if (isRunning())
  {
		// *NOTE*: MAY happen after application receives a SIGINT
		// select() returns -1, reactor invokes remove_handler --> remove_reference --> delete this
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("stream is still running --> check implementation !, continuing\n")));

    stop();
  } // end IF

  return 0;
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
int
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::svc"));

  ACE_Message_Block* ace_mb          = NULL;
  bool               stop_processing = false;

  // step0: increment thread count
  {
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myCurrentNumThreads++;
  } // end IF

  // step1: send initial session message downstream...
  if (!putSessionMessage(mySessionID,
                         RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN,
                         myUserData,
                         RPG_COMMON_TIME_POLICY(), // timestamp: start of session
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

      // done
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

// template <typename DataType,
//           typename SessionConfigType,
//           typename SessionMessageType>
// void
// RPG_Stream_HeadModuleTaskBase<DataType,SessionConfigType,SessionMessageType>::handleDataMessage(RPG_Stream_MessageBase*& message_inout,
//                                                        bool& passMessageDownstream_out)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::handleDataMessage"));
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

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::handleControlMessage(ACE_Message_Block* controlMessage_in,
                                                                         bool& stopProcessing_out,
                                                                         bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::handleControlMessage"));

  // init return value(s)
  stopProcessing_out = false;

  switch (controlMessage_in->msg_type())
  {
    case ACE_Message_Block::MB_STOP:
    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("received MB_STOP message, shutting down...\n")));

      // clean up --> we DON'T pass these along...
      passMessageDownstream_out = false;
      controlMessage_in->release();

      // *NOTE*: forward a SESSION_END message to notify any modules downstream
      stopProcessing_out = true;

      break;
    }
    default:
    {
      // ...otherwise, behave like a regular module...
      inherited::handleControlMessage(controlMessage_in,
                                      stopProcessing_out,
                                      passMessageDownstream_out);

      break;
    }
  } // end SWITCH
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::start"));

  // --> start a worker thread, if active
  changeState(inherited2::RUNNING);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::stop"));

  // (try to) change state
  changeState(inherited2::STOPPED);

  waitForCompletion();
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::pause()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::pause"));

  // (try to) change state
  changeState(inherited2::PAUSED);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::rewind()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::rewind"));

  // *TODO*: implement this !
  ACE_ASSERT(false);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::waitForCompletion()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::waitForCompletion"));

  if (myIsActive)
  {
    // step1: wait for workers to finish
    {
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      while (myCurrentNumThreads)
        myCondition.wait();
    } // end IF

		// step2: wait for workers to join
    if (inherited::wait() == -1)
	  ACE_DEBUG((LM_ERROR,
		         ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
  } // end IF
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::isRunning"));

  return (getState() == inherited2::RUNNING);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::finished()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::finished"));

  // (try to) set new state
  changeState(inherited2::FINISHED);

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("leaving finished()...\n")));
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::onStateChange(const Control_StateType& newState_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::onStateChange"));

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
        // resume worker ?
        if (myIsActive)
          if (inherited::resume() == -1)
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to resume(): \"%m\", continuing\n")));

        break;
      } // end IF

      if (myIsActive)
      {
        // OK: start worker
        ACE_hthread_t thread_handles[1];
        thread_handles[0] = 0;
        ACE_thread_t thread_ids[1];
        thread_ids[0] = 0;
        char thread_name[RPG_COMMON_BUFSIZE];
        ACE_OS::memset(thread_name, 0, sizeof(thread_name));
        ACE_OS::strcpy(thread_name, RPG_STREAM_DEF_HANDLER_THREAD_NAME);
        const char* thread_names[1];
        thread_names[0] = thread_name;
        if (inherited::activate((THR_NEW_LWP |
                                 THR_JOINABLE |
                                 THR_INHERIT_SCHED),         // flags
                                1,                           // number of threads
                                0,                           // force spawning
                                ACE_DEFAULT_THREAD_PRIORITY, // priority
                                inherited::grp_id(),         // group id (see above)
                                NULL,                        // corresp. task --> use 'this'
                                thread_handles,              // thread handle(s)
                                NULL,                        // thread stack(s)
                                NULL,                        // thread stack size(s)
                                thread_ids,                  // thread id(s)
                                thread_names) == -1)         // thread name(s)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", aborting\n")));

          break;
        } // end IF
      } // end IF
      else
      {
        // send initial session message downstream...
        if (!putSessionMessage(mySessionID,
                               RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN,
                               myUserData,
                               RPG_COMMON_TIME_POLICY(), // timestamp: start of session
                               false))                   // N/A
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("putSessionMessage(SESSION_BEGIN) failed, aborting\n")));

          break;
        } // end IF
      } // end ELSE

//       if (inherited::module())
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("module \"%s\" started worker thread (group: %d, id: %u)...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(inherited::name()),
//                    inherited::grp_id(),
//                    thread_ids[0]));
//       else
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("started worker thread (group: %d, id: %u)...\n"),
//                    inherited::grp_id(),
//                    thread_ids[0]));

      break;
    }
    case inherited2::STOPPED:
    {
      if (myIsActive)
      {
        // OK: drop a control message into the queue...
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
                                           NULL));                             // message allocator
        if (!stop_mb)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));

          break;
        } // end IF

				if (inherited::putq(stop_mb, NULL) == -1)
				{
					ACE_DEBUG((LM_ERROR,
						         ACE_TEXT("failed to ACE_Task::putq(): \"%m\", continuing\n")));

					// clean up
					stop_mb->release();
				} // end IF
      } // end IF
      else
      {
        // send final session message downstream...
        if (!putSessionMessage(mySessionID,
                               RPG_Stream_SessionMessage::MB_STREAM_SESSION_END,
                               myUserData,
                               ACE_Time_Value::zero, // N/A
                               false))               // N/A
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("putSessionMessage(SESSION_END) failed, aborting\n")));

				// signal the controller
				finished();
      } // end ELSE

      break;
    }
    case inherited2::FINISHED:
    {
      // signal waiting thread(s)
      {
//        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        myCurrentNumThreads--;

        myCondition.broadcast();
      } // end lock scope

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("finished successfully !\n")));

      break;
    }
    case inherited2::PAUSED:
    {
      // suspend the worker ?
      if (myIsActive)
        if (inherited::suspend() == -1)
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to suspend(): \"%m\", continuing\n")));

      break;
    }
    default:
    {
      // *NOTE*: if we get here, an invalid/unknown state change happened...

      // debug info
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

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::putSessionMessage(const unsigned int& sessionID_in,
                                                                      const RPG_Stream_SessionMessageType& messageType_in,
                                                                      SessionConfigType*& config_inout,
                                                                      RPG_Stream_IAllocator* allocator_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::putSessionMessage"));

  // create session message
  SessionMessageType* message = NULL;
  if (allocator_in)
  {
    try
    {
      // *IMPORTANT NOTE*: 0 --> session message !
      message = static_cast<SessionMessageType*>(allocator_in->malloc(0));
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
  { // *IMPORTANT NOTE*: session message assumes responsibility for session_config
    ACE_NEW_NORETURN(message,
                     SessionMessageType(sessionID_in,
                                        messageType_in,
                                        config_inout));
  } // end ELSE

  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate SessionMessageType: \"%m\", aborting\n")));

    // clean up
    config_inout->decrease();
    config_inout = NULL;

    return false;
  } // end IF
  if (allocator_in)
  { // *IMPORTANT NOTE*: session message assumes responsibility for session_config
    message->init(sessionID_in,
                  messageType_in,
                  config_inout);
  } // end IF

  // pass message downstream...
  if (const_cast<own_type*>(this)->put_next(message, NULL) == -1)
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

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_HeadModuleTaskBase<DataType,
                              SessionConfigType,
                              SessionMessageType,
                              ProtocolMessageType>::putSessionMessage(const unsigned int& sessionID_in,
                                                                      const RPG_Stream_SessionMessageType& messageType_in,
                                                                      const DataType& userData_in,
                                                                      const ACE_Time_Value& startOfSession_in,
                                                                      const bool& userAbort_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_HeadModuleTaskBase::putSessionMessage"));

  // create session data
  SessionConfigType* session_config = NULL;

  // switch
  switch (messageType_in)
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_STEP:
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      ACE_NEW_NORETURN(session_config,
                       SessionConfigType(userData_in,
                                         startOfSession_in,
                                         userAbort_in));
      if (!session_config)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate SessionConfigType: \"%m\", aborting\n")));

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

  // *IMPORTANT NOTE*: "fire-and-forget"-API for session_config
  return putSessionMessage(sessionID_in,
                           messageType_in,
                           session_config,
                           myAllocator);
}
