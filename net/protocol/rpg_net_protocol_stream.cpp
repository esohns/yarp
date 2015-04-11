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

#include "rpg_net_protocol_stream.h"

#include <string>

#include "rpg_net_protocol_common_modules.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_sessionmessage.h"

RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream ()
 : inherited ()
 , myIRCMarshal (std::string ("IRCMarshal"),
                 NULL)
 , myIRCParser (std::string ("IRCParser"),
                NULL)
 //, myIRCHandler (std::string ("IRCHandler"),
 //                NULL)
 , myRuntimeStatistic (std::string ("RuntimeStatistic"),
                       NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream"));

  // remember the ones we "own"...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  inherited::availableModules_.insert_tail (&myIRCMarshal);
  inherited::availableModules_.insert_tail (&myIRCParser);
  inherited::availableModules_.insert_tail (&myRuntimeStatistic);

  // fix ACE bug: modules should initialize their "next" member to NULL !
//   for (MODULE_CONTAINERITERATOR_TYPE iter = myAvailableModules.begin();
  inherited::MODULE_T* module_p = NULL;
  for (ACE_DLList_Iterator<inherited::MODULE_T> iterator (inherited::availableModules_);
       iterator.next (module_p);
       iterator.advance ())
    module_p->next (NULL);
}

RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
RPG_Net_Protocol_Stream::initialize (const RPG_Net_Protocol_Configuration& configuration_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::initialize"));

  // things to be done here:
  // - create modules (done for the ones we "own")
  // - init modules
  // - push them onto the stream (tail-first) !
  // ******************* Runtime Statistics ************************
  RPG_Net_Protocol_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl =
    dynamic_cast<RPG_Net_Protocol_Module_Statistic_WriterTask_t*> (myRuntimeStatistic.writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF
  if (!runtimeStatistic_impl->initialize (configuration_in.statisticReportingInterval,
                                          configuration_in.messageAllocator))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (myRuntimeStatistic.name ())));
    return false;
  } // end IF

  // enqueue the module...
  if (inherited::push (&myRuntimeStatistic))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
                ACE_TEXT (myRuntimeStatistic.name ())));
    return false;
  } // end IF

//   // ******************* IRC Handler ************************
//   RPG_Net_Protocol_Module_IRCHandler* IRCHandler_impl = NULL;
//   IRCHandler_impl = dynamic_cast<RPG_Net_Protocol_Module_IRCHandler*> (//                                      myIRCHandler.writer());
//   if (!IRCHandler_impl)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("dynamic_cast<RPG_Net_Protocol_Module_IRCHandler) failed> (aborting\n")));
//     return false;
//   } // end IF
//   if (!IRCHandler_impl->init(configuration_in.messageAllocator,
//                              (configuration_in.clientPingInterval ? false // servers shouldn't receive "pings" in the first place
//                                                                   : RPG_NET_DEF_CLIENT_PING_PONG), // auto-answer "ping" as a client ?...
//                              (configuration_in.clientPingInterval == 0))) // clients print ('.') dots for received "pings"...
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
//                myIRCHandler.name()));
//     return false;
//   } // end IF
//
//   // enqueue the module...
//   if (inherited::push(&myIRCHandler))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
//                myIRCHandler.name()));
//     return false;
//   } // end IF

  // ******************* IRC Parser ************************
  RPG_Net_Protocol_Module_IRCParser* IRCParser_impl = NULL;
  IRCParser_impl =
   dynamic_cast<RPG_Net_Protocol_Module_IRCParser*> (myIRCParser.writer());
  if (!IRCParser_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Protocol_Module_IRCParser) failed> (aborting\n")));
    return false;
  } // end IF
  if (!IRCParser_impl->initialize (configuration_in.messageAllocator,     // message allocator
                                   configuration_in.crunchMessageBuffers, // "crunch" messages ?
                                   configuration_in.debugScanner,         // debug scanner ?
                                   configuration_in.debugParser))         // debug parser ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (myIRCParser.name ())));
    return false;
  } // end IF

  // enqueue the module...
  if (inherited::push (&myIRCParser))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
                ACE_TEXT (myIRCParser.name ())));
    return false;
  } // end IF

  // ******************* IRC Marshal ************************
  RPG_Net_Protocol_Module_IRCSplitter* IRCSplitter_impl = NULL;
  IRCSplitter_impl =
   dynamic_cast<RPG_Net_Protocol_Module_IRCSplitter*> (myIRCMarshal.writer());
  if (!IRCSplitter_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Protocol_Module_IRCSplitter> failed, aborting\n")));
    return false;
  } // end IF
  if (!IRCSplitter_impl->initialize (configuration_in.messageAllocator, // message allocator
                                     false,                             // "crunch" messages ?
                                     0,                                 // DON'T collect statistics
                                     configuration_in.debugScanner))    // debug scanning ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (myIRCMarshal.name ())));
    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  myIRCMarshal.arg (&const_cast<RPG_Net_Protocol_Configuration&> (configuration_in));
  if (inherited::push (&myIRCMarshal))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
                ACE_TEXT (myIRCMarshal.name ())));
    return false;
  } // end IF

  // set (session) message allocator
  // *TODO*: clean this up ! --> sanity check
  ACE_ASSERT (configuration_in.messageAllocator);
  inherited::allocator_ = configuration_in.messageAllocator;

//   // debug info
//   inherited::dump_state();

  return true;
}

bool
RPG_Net_Protocol_Stream::collect (RPG_Net_Protocol_RuntimeStatistic& data_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::collect"));

  RPG_Net_Protocol_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl =
    dynamic_cast<RPG_Net_Protocol_Module_Statistic_WriterTask_t*> (const_cast<RPG_Net_Protocol_Module_RuntimeStatistic_Module&> (myRuntimeStatistic).writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF

  // delegate to this module...
  return (runtimeStatistic_impl->collect (data_out));
}

void
RPG_Net_Protocol_Stream::report () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::report"));

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
  ACE_ASSERT (false);

  ACE_NOTREACHED (return;)
}
