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

#include <ace/Message_Block.h>
#include <ace/Time_Value.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

template <typename TaskSynchStrategyType>
RPG_Common_TaskBase<TaskSynchStrategyType>::RPG_Common_TaskBase(const bool& autoStart_in)
 : inherited(NULL, // thread manager instance
             NULL) // message queue handle
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::RPG_Common_TaskBase"));

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_COMMON_UI_THREAD_GROUP_ID);

  // auto-start ?
  if (autoStart_in)
    open(NULL);
}

template <typename TaskSynchStrategyType>
RPG_Common_TaskBase<TaskSynchStrategyType>::~RPG_Common_TaskBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::~RPG_Common_TaskBase"));

  // sanity check(s)
  if (inherited::thr_count() > 0)
    shutdown();

  if (inherited::wait() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

template <typename TaskSynchStrategyType>
int
RPG_Common_TaskBase<TaskSynchStrategyType>::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::open"));

  ACE_UNUSED_ARG(args_in);

  // sanity check(s)
  if (inherited::thr_count() > 0)
    return 0; // nothing to do

	// spawn the dispatching worker thread
	ACE_thread_t thread_ids[1];
	thread_ids[0] = 0;
	ACE_hthread_t thread_handles[1];
	thread_handles[0] = 0;
	char thread_name[RPG_COMMON_BUFSIZE];
	ACE_OS::memset(thread_name, 0, sizeof(thread_name));
	ACE_OS::strcpy(thread_name, RPG_COMMON_UI_THREAD_NAME);
	const char* thread_names[1];
	thread_names[0] = thread_name;
	int result = inherited::activate((THR_NEW_LWP |
																		THR_JOINABLE |
																		THR_INHERIT_SCHED),           // flags
																	 1,                             // # threads --> 1
																	 0,                             // force active ?
																	 ACE_DEFAULT_THREAD_PRIORITY,   // priority
																	 inherited::grp_id(),           // group id (see above)
																	 NULL,                          // task base
																	 thread_handles,                // thread handle(s)
																	 NULL,                          // stack(s)
																	 NULL,                          // stack size(s)
																	 thread_ids,                    // thread id(s)
																	 thread_names);                 // thread name(s)
	if (result == -1)
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE_Task::activate(): \"%m\", aborting\n")));
	 else
		 ACE_DEBUG((LM_DEBUG,
								ACE_TEXT("spawned UI dispatch thread (ID: %u, group: %d)\n"),
								thread_ids[0],
								RPG_COMMON_UI_THREAD_GROUP_ID));

	return result;
}

template <typename TaskSynchStrategyType>
int
RPG_Common_TaskBase<TaskSynchStrategyType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::close"));

  // *NOTE*: this method may be invoked
  // - by an external thread closing down the active object
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    case 0:
    { // check specifically for the second case...
      if (ACE_OS::thr_equal(ACE_Thread::self(),
                            inherited::last_thread()))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count() == 0)
        return 0; // nothing to do

      shutdown();

      break;
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

template <typename TaskSynchStrategyType>
int
RPG_Common_TaskBase<TaskSynchStrategyType>::put(ACE_Message_Block* mb_in,
                                                ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::put"));

  return inherited::putq(mb_in, tv_in);
}

// *** dummy stub methods ***
template <typename TaskSynchStrategyType>
int
RPG_Common_TaskBase<TaskSynchStrategyType>::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::svc"));

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("(%t) worker starting...\n")));

  ACE_Message_Block* ace_mb = NULL;
  while (inherited::getq(ace_mb,
                         NULL) != -1) // blocking wait
  {
    if (!ace_mb)
      break;

    if (ace_mb->msg_type() == ACE_Message_Block::MB_STOP)
    {
      // clean up
      ace_mb->release();

      return 0; // done
    } // end IF

    // clean up
    ace_mb->release();
    ace_mb = NULL;
  } // end WHILE

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

  return -1;
}

template <typename TaskSynchStrategyType>
int
RPG_Common_TaskBase<TaskSynchStrategyType>::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::module_closed"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

template <typename TaskSynchStrategyType>
void
RPG_Common_TaskBase<TaskSynchStrategyType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::dump_state"));

}
// *** END dummy stub methods ***

template <typename TaskSynchStrategyType>
void
RPG_Common_TaskBase<TaskSynchStrategyType>::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TaskBase::shutdown"));

  // drop a control message into the queue...
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

    return;
  } // end IF

	if (inherited::putq(stop_mb, NULL) == -1)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE_Task::putq(): \"%m\", continuing\n")));

		// clean up
		stop_mb->release();
	} // end IF
}
