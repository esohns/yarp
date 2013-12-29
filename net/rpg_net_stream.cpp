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

#include "rpg_net_stream.h"

#include "rpg_net_defines.h"
#include "rpg_net_sessionmessage.h"
#include "rpg_net_message.h"

#include "rpg_stream_iallocator.h"

#include <string>

RPG_Net_Stream::RPG_Net_Stream()
 : //inherited(),
   mySocketHandler(std::string("SocketHandler"),
                   NULL),
   myHeaderParser(std::string("HeaderParser"),
                  NULL),
   myRuntimeStatistic(std::string("RuntimeStatistic"),
                      NULL),
   myProtocolHandler(std::string("ProtocolHandler"),
                     NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::RPG_Net_Stream"));

  // remember the ones we "own"...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  myAvailableModules.insert_tail(&mySocketHandler);
  myAvailableModules.insert_tail(&myHeaderParser);
  myAvailableModules.insert_tail(&myRuntimeStatistic);
  myAvailableModules.insert_tail(&myProtocolHandler);

  // *CHECK* fix ACE bug: modules should initialize their "next" member to NULL !
  inherited::MODULE_TYPE* module = NULL;
  for (ACE_DLList_Iterator<inherited::MODULE_TYPE> iterator(myAvailableModules);
       iterator.next(module);
       iterator.advance())
    module->next(NULL);
}

RPG_Net_Stream::~RPG_Net_Stream()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::~RPG_Net_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown();
}

bool
RPG_Net_Stream::init(const RPG_Net_ConfigPOD& config_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::init"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream)
  // - init modules
  // - push them onto the stream (tail-first) !

	// ******************* Protocol Handler ************************
  RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
  protocolHandler_impl = dynamic_cast<RPG_Net_Module_ProtocolHandler*>(myProtocolHandler.writer());
  if (!protocolHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_ProtocolHandler> failed> (aborting\n")));

    return false;
  } // end IF
  if (!protocolHandler_impl->init(config_in.messageAllocator,
                                  config_in.sessionID,
                                  config_in.pingInterval,
                                  config_in.pingAutoAnswer,
                                  config_in.printPingMessages)) // print ('.') for received "ping"s...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myProtocolHandler.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myProtocolHandler) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // ******************* Runtime Statistics ************************
  RPG_NET_MODULE_RUNTIMESTATISTICS_T* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = dynamic_cast<RPG_NET_MODULE_RUNTIMESTATISTICS_T*>(myRuntimeStatistic.writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_RuntimeStatistic> failed> (aborting\n")));

    return false;
  } // end IF
  if (!runtimeStatistic_impl->init(config_in.statisticsReportingInterval, // reporting interval (seconds)
                                   config_in.messageAllocator))           // print cache info ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myRuntimeStatistic.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myRuntimeStatistic) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // ******************* Header Parser ************************
  RPG_Net_Module_HeaderParser* headerParser_impl = NULL;
  headerParser_impl = dynamic_cast<RPG_Net_Module_HeaderParser*>(myHeaderParser.writer());
  if (!headerParser_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_HeaderParser> failed> (aborting\n")));

    return false;
  } // end IF
  if (!headerParser_impl->init())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myHeaderParser.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myHeaderParser) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // ******************* Socket Handler ************************
  RPG_Net_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl = dynamic_cast<RPG_Net_Module_SocketHandler*>(mySocketHandler.writer());
  if (!socketHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_SocketHandler> failed> (aborting\n")));

    return false;
  } // end IF
  if (!socketHandler_impl->init(config_in.messageAllocator,
                                config_in.useThreadPerConnection,
                                RPG_NET_STATISTICS_COLLECT_INTERVAL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               mySocketHandler.name()));

    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  mySocketHandler.arg(&const_cast<RPG_Net_ConfigPOD&>(config_in));
  if (push(&mySocketHandler) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // set (session) message allocator
  // *TODO*: clean this up ! --> sanity check
  ACE_ASSERT(config_in.messageAllocator);
  inherited::myAllocator = config_in.messageAllocator;

  // OK: all went well
  inherited::myIsInitialized = true;

//   // debug info
//   inherited::dump_state();

  return true;
}

void
RPG_Net_Stream::ping()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::ping"));

  RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
  protocolHandler_impl = dynamic_cast<RPG_Net_Module_ProtocolHandler*>(myProtocolHandler.writer());
  if (!protocolHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_ProtocolHandler) failed> (aborting\n")));

    return;
  } // end IF

  // delegate to this module...
  protocolHandler_impl->handleTimeout(NULL);
}

unsigned int
RPG_Net_Stream::getSessionID() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::getSessionID"));

  // *TODO*: clean this up
  ACE_Module<ACE_MT_SYNCH, ACE_System_Time_Policy>* module = &const_cast<RPG_Net_Module_SocketHandler_Module&>(mySocketHandler);
  RPG_Net_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl = dynamic_cast<RPG_Net_Module_SocketHandler*>(module->writer());
  if (!socketHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_SocketHandler> failed> (aborting\n")));

    return 0;
  } // end IF

  return socketHandler_impl->getSessionID();
}

bool
RPG_Net_Stream::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::collect"));

  RPG_NET_MODULE_RUNTIMESTATISTICS_T* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = dynamic_cast<RPG_NET_MODULE_RUNTIMESTATISTICS_T*>(const_cast<RPG_Net_Module_RuntimeStatistic_Module&>(myRuntimeStatistic).writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_RuntimeStatistic) failed> (aborting\n")));

    return false;
  } // end IF

  // delegate to this module...
  return runtimeStatistic_impl->collect(data_out);
}

void
RPG_Net_Stream::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Stream::report"));

//   RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<RPG_Net_Module_RuntimeStatistic*> (//                                            myRuntimeStatistic.writer());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("dynamic_cast<RPG_Net_Module_RuntimeStatistic) failed> (aborting\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report());

  // just a dummy
  ACE_ASSERT(false);

  ACE_NOTREACHED(return;)
}
