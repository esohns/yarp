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

#include "rpg_net_protocol_stream.h"

#include <string>

RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream()
 : //inherited(),
   myIRCSplitter(std::string("IRCSplitter"),
                 NULL),
   myIRCParser(std::string("IRCParser"),
               NULL),
   myIRCHandler(std::string("IRCHandler"),
                NULL),
   myRuntimeStatistic(std::string("RuntimeStatistic"),
                      NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream"));

  // remember the ones we "own"...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  myAvailableModules.push_back(&myIRCSplitter);
  myAvailableModules.push_back(&myIRCParser);
  myAvailableModules.push_back(&myIRCHandler);
  myAvailableModules.push_back(&myRuntimeStatistic);

  // fix ACE bug: modules should initialize their "next" member to NULL !
  for (MODULE_CONTAINERITERATOR_TYPE iter = myAvailableModules.begin();
       iter != myAvailableModules.end();
       iter++)
  {
    (*iter)->next(NULL);
  } // end FOR
}

RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown();
}

const bool
RPG_Net_Protocol_Stream::init(const RPG_Net_ConfigPOD& config_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Stream::init"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);

  // things to be done here:
  // - create modules (done for the ones we "own")
  // - init modules
  // - push them onto the stream (tail-first) !
  // ******************* Runtime Statistics ************************
  RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
                                           myRuntimeStatistic.writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));

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

  // ******************* IRC Handler ************************
  RPG_Net_Protocol_Module_IRCHandler* IRCHandler_impl = NULL;
  IRCHandler_impl = ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCHandler*,
                                     myIRCHandler.writer());
  if (!IRCHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCHandler) failed, aborting\n")));

    return false;
  } // end IF
  if (!IRCHandler_impl->init(config_in.messageAllocator,
                             (config_in.clientPingInterval ? false // servers shouldn't receive "pings" in the first place
                                                           : RPG_NET_DEF_CLIENT_PING_PONG), // auto-answer "ping" as a client ?...
                             (config_in.clientPingInterval == 0))) // clients print ('.') dots for received "pings"...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myIRCHandler.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myIRCHandler))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myIRCHandler.name()));

    return false;
  } // end IF

  // ******************* IRC Parser ************************
  RPG_Net_Protocol_Module_IRCParser* IRCParser_impl = NULL;
  IRCParser_impl = ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCParser*,
                                    myIRCParser.writer());
  if (!IRCParser_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCParser) failed, aborting\n")));

    return false;
  } // end IF
  if (!IRCParser_impl->init(config_in.messageAllocator)) // message allocator
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myIRCParser.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myIRCParser))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myIRCParser.name()));

    return false;
  } // end IF

  // ******************* IRC Splitter ************************
  RPG_Net_Protocol_Module_IRCSplitter* IRCSplitter_impl = NULL;
  IRCSplitter_impl = ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCSplitter*,
                                      myIRCSplitter.writer());
  if (!IRCSplitter_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCSplitter) failed, aborting\n")));

    return false;
  } // end IF
  if (!IRCSplitter_impl->init(config_in.messageAllocator, // message allocator
                              true,                       // "crunch" messages
                              0,                          // DON'T collect statistics
                              true))                      // trace scanning
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myIRCSplitter.name()));

    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  myIRCSplitter.arg(&ACE_const_cast(RPG_Net_ConfigPOD&, config_in));
  if (push(&myIRCSplitter))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myIRCSplitter.name()));

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

const bool
RPG_Net_Protocol_Stream::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Stream::collect"));

  RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
                                           ACE_const_cast(RPG_Net_Module_RuntimeStatistic_Module&, myRuntimeStatistic).writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));

    return false;
  } // end IF

  // delegate to this module...
  return (runtimeStatistic_impl->collect(data_out));
}

void
RPG_Net_Protocol_Stream::report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Stream::report"));

//   RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
//                                            myRuntimeStatistic.writer());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report());

  // just a dummy
  ACE_ASSERT(false);

  ACE_NOTREACHED()
}