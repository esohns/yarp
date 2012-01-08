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

#include "rpg_stream_task_asynch.h"

#include "rpg_stream_defines.h"

#include <rpg_common_macros.h>

#include <ace/OS.h>
#include <ace/Message_Block.h>
#include <ace/Time_Value.h>

#include <string>

RPG_Stream_TaskAsynch::RPG_Stream_TaskAsynch()
 : myThreadID(0),
   myQueue(RPG_STREAM_DEF_MAX_QUEUE_SLOTS)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::RPG_Stream_TaskAsynch"));

  // tell the task to use our message queue...
  msg_queue(&myQueue);

  // set group ID for worker thread(s)
  // *TODO*: pass this in from outside...
  grp_id(RPG_STREAM_DEF_GROUP_ID_TASK);
}

RPG_Stream_TaskAsynch::~RPG_Stream_TaskAsynch()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::~RPG_Stream_TaskAsynch"));

  // flush/deactivate our queue (check whether it was empty...)
  int flushed_messages = 0;
  flushed_messages = myQueue.flush();

  // *NOTE*: this should probably NEVER happen !
  // debug info
  if (flushed_messages)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("flushed %d message(s)...\n"),
               flushed_messages));
  } // end IF

//   // *TODO*: check if this sequence actually works...
//   myQueue.deactivate();
//   myQueue.wait();
}

int
RPG_Stream_TaskAsynch::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::open"));

  ACE_UNUSED_ARG(args_in);

  // step1: (re-)activate() our queue
  // *NOTE*: the first time around, our queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state
  // - hopefully, this is what we want.
  // If we come here a second time (i.e. we have been stopped/started, our queue
  // will have been deactivated in the process, and getq() (see svc()) will fail
  // miserably (ESHUTDOWN) --> (re-)activate() our queue
  if (myQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));

    return -1;
  } // end IF

  // step2: (try to) start new worker thread...
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;

  // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  int ret = 0;
  ret = activate((THR_NEW_LWP |
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
  if (ret == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", aborting\n")));

    return -1;
  } // end IF

  // remember our worker thread ID...
//   myThreadID = thread_handles[0];
  myThreadID = thread_ids[0];

//   // debug info
//   if (module())
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("module \"%s\" started worker thread (group: %d, id: %u)...\n"),
//                ACE_TEXT_ALWAYS_CHAR(name()),
//                grp_id(),
//                myThreadID));
//   } // end IF
//   else
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("started worker thread (group: %d, id: %u)...\n"),
//                grp_id(),
//                myThreadID));
//   } // end IF

  return 0;
}

int
RPG_Stream_TaskAsynch::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::close"));

  // *IMPORTANT NOTE*: this method may be invoked
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

      // don't (need to) do anything...
      break;
    }
    case 1:
    {
      // *WARNING*: SHOULD NEVER GET HERE
      // --> module_closed() hook is implemented below !!!
      ACE_ASSERT(false);

      // what else can we do ?
      return -1;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument: %u, aborting\n"),
                 arg_in));

      // what else can we do ?
      return -1;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Stream_TaskAsynch::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::module_closed"));

  // just wait for our worker thread(s) to die
  // *NOTE*: this works because we assume that by the time we get here,
  // a control message has been sent downstream and will be processed by our
  // worker thread sooner or later, which should make it kill itself...
  wait();

  return 0;
}

int
RPG_Stream_TaskAsynch::put(ACE_Message_Block* mb_in,
                       ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::put"));

  // drop the message into our queue...
  return putq(mb_in,
              tv_in);
}

int
RPG_Stream_TaskAsynch::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::svc"));

  ACE_Message_Block* ace_mb          = NULL;
  bool               stop_processing = false;

  while (getq(ace_mb,
               NULL) != -1)
  {
    inherited::handleMessage(ace_mb,
                             stop_processing);

    // finished ?
    if (stop_processing)
    {
      // *WARNING*: ace_mb has already been released() at this point !

      // leave loop, we're finished !
      return 0;
    } // end IF

    // init
    ace_mb = NULL;
  } // end WHILE

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

  // SHOULD NEVER GET HERE !
  ACE_ASSERT(false);

  ACE_NOTREACHED(ACE_TEXT("not reached..."));

  return -1;
}

void
RPG_Stream_TaskAsynch::waitForIdleState() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::waitForIdleState"));

  // simply delegate this to our queue...
  try
  {
    myQueue.waitForIdleState();
  }
  catch (...)
  {
    // *IMPORTANT NOTE*: this will probably have some fatal repercussions...
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IMessageQueueSynch::waitForIdleState, continuing\n")));
  }
}

void
RPG_Stream_TaskAsynch::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskAsynch::shutdown"));

//   // debug info
//   if (module())
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("initiating shutdown of worker thread(s) for module \"%s\"...\n"),
//                ACE_TEXT_ALWAYS_CHAR(name())));
//   } // end IF
//   else
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("initiating shutdown of worker thread(s)...\n")));
//   } // end ELSE

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

    // what else can we do ?
    return;
  } // end IF

  if (put(stop_mb, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::put(): \"%m\", continuing\n")));

    // clean up
    stop_mb->release();
  } // end IF

//   // debug info
//   if (module())
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("initiating shutdown of worker thread(s) for module \"%s\"...DONE\n"),
//                ACE_TEXT_ALWAYS_CHAR(name())));
//   } // end IF
//   else
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("initiating shutdown of worker thread(s)...DONE\n")));
//   } // end ELSE
}
