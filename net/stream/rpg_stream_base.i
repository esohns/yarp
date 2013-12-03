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

#include "rpg_stream_session_config_base.h"
#include "rpg_stream_session_message_base.h"
#include "rpg_stream_iallocator.h"

#include <rpg_common_macros.h>

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::RPG_Stream_Base()
// *TODO*: use default ctor and rely on init/fini() ?
 : inherited(NULL,    // argument to module open()
             NULL,    // no head module --> allocate !
             NULL),   // no tail module --> allocate !
   myIsInitialized(false),
   myAllocator(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::RPG_Stream_Base"));

}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::~RPG_Stream_Base()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::~RPG_Stream_Base"));

}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::reset()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::reset"));

  // sanity check: is running ?
  if (isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("cannot reset (currently running), aborting\n")));

    return false;
  } // end IF

  // pop/close all modules
  // *NOTE*: will implicitly (blocking !) wait for any active worker threads
  if (!fini())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to fini(), aborting\n")));

    return false;
  } // end IF

  // - reset reader/writers tasks for ALL modules
  // - reinit head/tail modules
  return init();
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::init"));

  // *NOTE*: fini() invokes close() which will reset the writer/reader tasks
  // of the enqueued modules --> reset this !
  // *NOTE*: cannot write this - it confuses gcc...
//   for (MODULE_CONTAINER_TYPE::const_iterator iter = myAvailableModules.begin();
  RPG_Stream_Module::MODULE_TYPE* module = NULL;
  for (ACE_DLList_Iterator<RPG_Stream_Module::MODULE_TYPE> iterator(myAvailableModules);
       iterator.next(module);
       iterator.advance())
  {
    // need a downcast...
    RPG_Stream_Module* stream_module = dynamic_cast<RPG_Stream_Module*>(module);
    ACE_ASSERT(stream_module);
    try
    {
      stream_module->resetReaderWriter();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Stream_Module::resetReaderWriter(), continuing\n")));
    }
  } // end FOR

  // delegate this to base class open()
  int ret = -1;
  try
  {
    ret = inherited::open(NULL,  // argument to module open()
                          NULL,  // no head module --> allocate !
                          NULL); // no tail module --> allocate !
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_Stream::open(), aborting\n")));
  }
  if (ret == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::open(): \"%m\", aborting\n")));

  return (ret == 0);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::fini"));

  // OK: delegate this to base class close(ACE_Module_Base::M_DELETE_NONE)
  int ret = -1;
  try
  {
    // *NOTE*: this will implicitly:
    // - unwind the stream, which pop()s all (pushed) modules
    // --> pop()ing a module will close() it
    // --> close()ing a module will module_closed() and flush() the associated tasks
    // --> flush()ing a task will close() its queue
    // --> close()ing a queue will deactivate() and flush() it
    ret = inherited::close(ACE_Module_Base::M_DELETE_NONE);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_Stream::close(M_DELETE_NONE), aborting\n")));
  }
  if (ret == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::close(M_DELETE_NONE): \"%m\", aborting\n")));

  return (ret == 0);
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::start"));

  // sanity check: is initialized ?
  if (!isInitialized())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, returning\n")));

    return;
  } // end IF

  // delegate to the head module
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // skip over ACE_Stream_Head...
  module = module->next();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module == tail())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return;
  } // end IF

  RPG_Stream_IStreamControl* control_impl = NULL;
  control_impl = dynamic_cast<RPG_Stream_IStreamControl*>(module->writer());
  if (!control_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_IStreamControl) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  } // end IF

  try
  {
    control_impl->start();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::start (module: \"%s\"), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  }
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::stop"));

  // sanity check: is running ?
  if (!isRunning())
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("not running --> nothing to do, returning\n")));

    return;
  } // end IF

  // delegate to the head module
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // skip over ACE_Stream_Head...
  module = module->next();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module == tail())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return;
  } // end IF

  RPG_Stream_IStreamControl* control_impl = NULL;
  control_impl = dynamic_cast<RPG_Stream_IStreamControl*> (module->writer());
  if (!control_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_IStreamControl) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  } // end IF

  try
  {
    control_impl->stop();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::stop (module: \"%s\"), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  }
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::pause()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::pause"));

  // sanity check: is running ?
  if (!isRunning())
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("not running --> nothing to do, returning\n")));

    return;
  } // end IF

  // delegate to the head module
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // skip over ACE_Stream_Head...
  module = module->next();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module == tail())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return;
  } // end IF

  RPG_Stream_IStreamControl* control_impl = NULL;
  control_impl = dynamic_cast<RPG_Stream_IStreamControl*>(module->writer());
  if (!control_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_IStreamControl) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  } // end IF

  try
  {
    control_impl->pause();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::pause (module: \"%s\"), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  }
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::rewind()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::rewind"));

  // sanity check: is running ?
  if (isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("currently running, returning\n")));

    return;
  } // end IF

  // delegate to the head module
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    // *NOTE*: what else can we do ?
    return;
  } // end IF

  // skip over ACE_Stream_Head...
  module = module->next();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    // *NOTE*: what else can we do ?
    return;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module == tail())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return;
  } // end IF

  RPG_Stream_IStreamControl* control_impl = NULL;
  control_impl = dynamic_cast<RPG_Stream_IStreamControl*>(module->writer());
  if (!control_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_IStreamControl) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  } // end IF

  try
  {
    control_impl->rewind();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::rewind (module: \"%s\"), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  }
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::waitForCompletion()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::waitForCompletion"));

//   // sanity check: is initialized ?
//   if (!isInitialized())
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("stream not initialized, continuing\n")));

  // OK: the logic here is this...
  // step1: wait for processing to finish
  // step2: wait for the pipelined messages to flush...

  // step1
  const ACE_Module<ACE_MT_SYNCH>* module = NULL;
  ACE_Stream_Iterator<ACE_MT_SYNCH> iter(*this);
  // skip over ACE_Stream_Head
  if (!iter.advance())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // get head module
  if (!iter.next(module))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    return;
  } // end IF

  // sanity check: head == tail ?
  // --> reason: no modules have been push()ed (yet) !
  // --> stream hasn't been intialized (at all: too many connections ?)
  // --> there is nothing to do !
  if (module == tail())
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return;
  } // end IF

  // need to downcast
  HEADMODULETASK_BASETYPE* head_task = NULL;
  head_task = dynamic_cast<HEADMODULETASK_BASETYPE*>(const_cast<ACE_Module<ACE_MT_SYNCH>*>(module)->writer());
  if (!head_task)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_HeadModuleTaskBase) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  } // end IF

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("waiting for module (\"%s\") to finish processing...\n"),
//              ACE_TEXT_ALWAYS_CHAR(module->name())));

  // OK: now that we've got a handler... wait on it
  try
  {
    // step1: wait for state switch (xxx --> FINISHED)
    head_task->waitForCompletion();

    // step2: wait for worker thread
    head_task->wait();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::waitForCompletion || wait (module: \"%s\"), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return;
  }

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("waiting for module (\"%s\") to finish processing...DONE\n"),
//              ACE_TEXT_ALWAYS_CHAR(module->name())));
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("waiting for stream to flush...\n")));

  for (iter.advance();
       iter.next(module);
       iter.advance())
  {
    // skip stream tail (last module)
    if (module == tail())
    {
      // skip last module (stream tail)
      continue;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("waiting for module (\"%s\") to finish processing...\n"),
//                ACE_TEXT_ALWAYS_CHAR(module->name())));

    // OK: we've got a handle... wait
    const_cast<ACE_Module<ACE_MT_SYNCH>*>(module)->writer()->wait();

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("waiting for module (\"%s\") to finish processing...DONE\n"),
//                ACE_TEXT_ALWAYS_CHAR(module->name())));

    module = NULL;
  } // end FOR

  // OK: IF no new messages have been enqueued IN THE MEANTIME, the queue should be empty...
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("waiting for stream to flush...DONE\n")));
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::isRunning"));

  // delegate to the head module
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = const_cast<RPG_Stream_Base*>(this)->head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, aborting\n")));

    return false;
  } // end IF

  // skip over ACE_Stream_Head...
  module = module->next();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, aborting\n")));

    return false;
  } // end IF

  // sanity check: head == tail ? --> no modules have been push()ed (yet) !
  if (module == const_cast<RPG_Stream_Base*>(this)->tail())
  {
    // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("no modules have been enqueued yet --> nothing to do !, returning\n")));

    return false;
  } // end IF

  RPG_Stream_IStreamControl* control_impl = NULL;
  control_impl = dynamic_cast<RPG_Stream_IStreamControl*>(module->writer());
  if (!control_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Stream_IStreamControl) failed> (returning\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));

    return false;
  } // end IF

  try
  {
    return control_impl->isRunning();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IStreamControl::isRunning (module: \"%s\"), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(module->name())));
  }

  return false;
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::dump_state"));

  std::string stream_layout;

  const ACE_Module<ACE_MT_SYNCH>* module = NULL;
  for (ACE_Stream_Iterator<ACE_MT_SYNCH> iter(*this);
       iter.next(module);
       iter.advance())
  {
    // silently ignore ACE head/tail modules...
    if ((module == const_cast<RPG_Stream_Base*>(this)->tail()) ||
        (module == const_cast<RPG_Stream_Base*>(this)->head()))
    {
      continue;
    } // end IF

    stream_layout.append(ACE_TEXT_ALWAYS_CHAR(module->name()));

    // avoid trailing "-->"...
    if (const_cast<ACE_Module<ACE_MT_SYNCH>*>(module)->next() !=
        const_cast<RPG_Stream_Base*>(this)->tail())
    {
      stream_layout += ACE_TEXT_ALWAYS_CHAR(" --> ");
    } // end IF

    module = NULL;
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("stream layout: \"%s\"\n"),
             stream_layout.c_str()));
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
bool
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::isInitialized() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::isInitialized"));

  return myIsInitialized;
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::shutdown"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("shutting down stream...\n")));

  // if we haven't properly initialized, we need to deactivate any hitherto enqueued
  // ACTIVE modules, or we will wait forever during closure...
  // --> possible scenarios:
  // - (re-)init() failed halfway through (i.e. MAYBE some modules push()ed correctly)
  if (!myIsInitialized)
  {
    // sanity check: have we actually successfully pushed() any modules ?
    if (head()->next() != tail())
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("not initialized - deactivating module(s)...\n")));

      deactivateModules();

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("not initialized - deactivating module(s)...WAITING\n")));

      // ...and wait for it to complete ?
      // *NOTE*: children can synchronize by calling waitForCompletion...

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("not initialized - deactivating module(s)...DONE\n")));
    } // end IF
  } // end IF

  // step1: retrieve a list of modules which are NOT on the stream
  // --> need to close() these manually (before they do so in their dtors...)
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating offline module(s)...\n")));

//   for (MODULE_CONTAINERITERATOR_TYPE iter = myAvailableModules.begin();
  RPG_Stream_Module::MODULE_TYPE* module = NULL;
  for (ACE_DLList_Iterator<RPG_Stream_Module::MODULE_TYPE> iterator(myAvailableModules);
       iterator.next(module);
       iterator.advance())
  {
    if (module->next() == NULL)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("manually closing module: \"%s\"\n"),
//                  ACE_TEXT_ALWAYS_CHAR((*iter)->name())));

      try
      {
        module->close(ACE_Module_Base::M_DELETE_NONE);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in ACE_Module::close(M_DELETE_NONE), continuing\n")));
      }
    } // end IF
  } // end FOR

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating offline module(s)...DONE\n")));

  // step2: shutdown stream
  // check the ACE documentation on ACE_Stream to see why this is needed !!!
  // Note: ONLY do this if stream_head != 0 !!! (warning: obsolete ?)
  // *NOTE*: this will NOT destroy all modules in the current stream
  // as this leads to exceptions in debug builds under windows (can't delete
  // objects in a different DLL where it was created...)
  // --> we need to do this ourselves !
  // all this does is call close() on each one (--> wait for the worker thread to return)
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating module(s)...\n")));

  fini();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating module(s)...DONE\n")));

  // *NOTE*: every ACTIVE module will join with its worker thread in close()
  // --> ALL stream-related threads should have returned by now !

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("shutting down stream...FINISHED\n")));
}

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
void
RPG_Stream_Base<DataType,
                SessionConfigType,
                SessionMessageType,
                ProtocolMessageType>::deactivateModules()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Base::deactivateModules"));

  // create (dummy) user data
  DataType data;
  ACE_OS::memset(&data,
                 0,
                 sizeof(DataType));

  // create session config
  SessionConfigType* session_config = NULL;
  ACE_NEW_NORETURN(session_config,
                   SessionConfigType(data));
  if (!session_config)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate RPG_Stream_SessionConfig: \"%m\", aborting\n")));

    return;
  } // end IF

  // create MB_STREAM_SESSION_END session message
  SessionMessageType* message = NULL;
  if (myAllocator)
  {
    try
    {
      message = static_cast<SessionMessageType*>(myAllocator->malloc(0)); // we want a session message !
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("caught exception in RPG_Stream_IAllocator::malloc(0), aborting\n")));

      // clean up
      session_config->decrease();
      session_config = NULL;

      return;
    }
  }
  else
  { // *NOTE*: session message assumes responsibility for session_config !
    ACE_NEW_NORETURN(message,
                     SessionMessageType(0, // N/A
                                        RPG_Stream_SessionMessage::MB_STREAM_SESSION_END,
                                        session_config));
  } // end ELSE

  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate SessionMessageType: \"%m\", aborting\n")));

    // clean up
    session_config->decrease();
    session_config = NULL;

    return;
  } // end IF
  if (myAllocator)
  { // *NOTE*: session message assumes responsibility for session_config !
    message->init(0, // N/A
                  RPG_Stream_SessionMessage::MB_STREAM_SESSION_END,
                  session_config);
  } // end IF

  // pass message downstream...
  if (put(message, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::put(): \"%m\", aborting\n")));

    // clean up
    message->release();

    return;
  } // end IF
}
