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
   myProtocolHandler(std::string("ProtocolHandler"),
                     NULL),
   myRuntimeStatistic(std::string("RuntimeStatistic"),
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
  myAvailableModules.insert_tail(&myProtocolHandler);
  myAvailableModules.insert_tail(&myRuntimeStatistic);

    // fix ACE bug: modules should initialize their "next" member to NULL !
//   for (MODULE_CONTAINERITERATOR_TYPE iter = myAvailableModules.begin();
  RPG_Stream_Module::MODULE_TYPE* module = NULL;
  for (ACE_DLList_Iterator<RPG_Stream_Module::MODULE_TYPE> iterator(myAvailableModules);
       iterator.next(module);
       iterator.advance())
  {
    module->next(NULL);
  } // end FOR
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
  // - create modules (done for the ones we "own")
  // - init modules
  // - push them onto the stream (tail-first) !
  // ******************* Runtime Statistics ************************
  RPG_NET_MODULE_RUNTIMESTATISTICS_T* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = dynamic_cast<RPG_NET_MODULE_RUNTIMESTATISTICS_T*>(myRuntimeStatistic.writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_RuntimeStatistic) failed> (aborting\n")));

    return false;
  } // end IF
  if (!runtimeStatistic_impl->init(config_in.statisticsReportingInterval,
                                   config_in.messageAllocator)) // print cache info ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myRuntimeStatistic.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myRuntimeStatistic))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myRuntimeStatistic.name()));

    return false;
  } // end IF

  // ******************* Protocol Handler ************************
  RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
  protocolHandler_impl = dynamic_cast<RPG_Net_Module_ProtocolHandler*>(myProtocolHandler.writer());
  if (!protocolHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_ProtocolHandler) failed> (aborting\n")));

    return false;
  } // end IF
  if (!protocolHandler_impl->init(config_in.messageAllocator,
                                  config_in.clientPingInterval,
                                  (config_in.clientPingInterval ? false // servers shouldn't receive "pings" in the first place
                                                                : RPG_NET_DEF_CLIENT_PING_PONG), // auto-answer "ping" as a client ?...
                                  (config_in.clientPingInterval == 0))) // clients print ('.') dots for received "pings"...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myProtocolHandler.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myProtocolHandler))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myProtocolHandler.name()));

    return false;
  } // end IF

  // ******************* Header Parser ************************
  RPG_Net_Module_HeaderParser* headerParser_impl = NULL;
  headerParser_impl = dynamic_cast<RPG_Net_Module_HeaderParser*>(myHeaderParser.writer());
  if (!headerParser_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_HeaderParser) failed> (aborting\n")));

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
  if (push(&myHeaderParser))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myHeaderParser.name()));

    return false;
  } // end IF

  // ******************* Socket Handler ************************
  RPG_Net_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl = dynamic_cast<RPG_Net_Module_SocketHandler*>(mySocketHandler.writer());
  if (!socketHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_SocketHandler) failed> (aborting\n")));

    return false;
  } // end IF
  if (!socketHandler_impl->init(config_in.messageAllocator,
//                                 config_in.connectionID,
                                RPG_NET_DEF_STATISTICS_COLLECT_INTERVAL))
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
  if (push(&mySocketHandler))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               mySocketHandler.name()));

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
  return (runtimeStatistic_impl->collect(data_out));
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
