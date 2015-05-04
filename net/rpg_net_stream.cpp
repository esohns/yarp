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

#include "RPG_Net_Stream.h"

#include <string>

#include "ace/Log_Msg.h"

#include "net_defines.h"

#include "rpg_common_macros.h"

RPG_Net_Stream::RPG_Net_Stream ()
 : inherited ()
 , socketHandler_ (std::string ("SocketHandler"),
                   NULL)
 , headerParser_ (std::string ("HeaderParser"),
                  NULL)
 , protocolHandler_ (std::string ("ProtocolHandler"),
                     NULL)
 , runtimeStatistic_ (std::string ("RuntimeStatistic"),
                      NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::RPG_Net_Stream"));

  // remember the "owned" ones...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  inherited::availableModules_.insert_tail (&socketHandler_);
  inherited::availableModules_.insert_tail (&headerParser_);
  inherited::availableModules_.insert_tail (&runtimeStatistic_);
  inherited::availableModules_.insert_tail (&protocolHandler_);

  // *CHECK* fix ACE bug: modules should initialize their "next" member to NULL !
  inherited::MODULE_T* module = NULL;
  for (ACE_DLList_Iterator<inherited::MODULE_T> iterator (inherited::availableModules_);
       iterator.next (module);
       iterator.advance ())
    module->next (NULL);
}

RPG_Net_Stream::~RPG_Net_Stream ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::~RPG_Net_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
RPG_Net_Stream::initialize (unsigned int sessionID_in,
                            Stream_Configuration_t& configuration_in,
                            const Net_ProtocolConfiguration_t& protocolConfiguration_in,
                            const Net_UserData_t& userData_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isInitialized_);

  // things to be done here:
  // [- init base class]
  // ------------------------------------
  // - initialize notification strategy (if any)
  // ------------------------------------
  // - push the final module onto the stream (if any)
  // ------------------------------------
  // - initialize modules
  // - push them onto the stream (tail-first) !
  // ------------------------------------

//  ACE_OS::memset (&inherited::state_, 0, sizeof (inherited::state_));
  inherited::state_.sessionID = sessionID_in;

  int result = -1;
  inherited::MODULE_T* module_p = NULL;
  if (configuration_in.notificationStrategy)
  {
    module_p = inherited::head ();
    if (!module_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module found, aborting\n")));
      return false;
    } // end IF
    inherited::TASK_T* task_p = module_p->reader ();
    if (!task_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module reader task found, aborting\n")));
      return false;
    } // end IF
    task_p->msg_queue ()->notification_strategy (configuration_in.notificationStrategy);
  } // end IF

  ACE_ASSERT (configuration_in.moduleConfiguration);
  configuration_in.moduleConfiguration->streamState = &state_;

  // ---------------------------------------------------------------------------
  if (configuration_in.module)
  {
    Stream_IModule_t* module_2 =
      dynamic_cast<Stream_IModule_t*> (configuration_in.module);
    if (!module_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<Stream_IModule_t> failed, aborting\n")));
      return false;
    } // end IF
    if (!module_2->initialize (*configuration_in.moduleConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_IModule_t::initialize, aborting\n")));
      return false;
    } // end IF
    result = inherited::push (configuration_in.module);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (configuration_in.module->name ())));
      return false;
    } // end IF
  } // end IF

  // ---------------------------------------------------------------------------

  // ******************* Protocol Handler ************************
  protocolHandler_.initialize (*configuration_in.moduleConfiguration);
  RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
  protocolHandler_impl =
      dynamic_cast<RPG_Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
  if (!protocolHandler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Module_ProtocolHandler> failed, aborting\n")));
    return false;
  } // end IF
  if (!protocolHandler_impl->initialize (configuration_in.messageAllocator,
                                         protocolConfiguration_in.peerPingInterval,
                                         protocolConfiguration_in.pingAutoAnswer,
                                         protocolConfiguration_in.printPongMessages)) // print ('.') for received "pong"s...
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (protocolHandler_.name ())));
    return false;
  } // end IF
  result = inherited::push (&protocolHandler_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (protocolHandler_.name ())));
    return false;
  } // end IF

  // ******************* Runtime Statistics ************************
  runtimeStatistic_.initialize (*configuration_in.moduleConfiguration);
  Net_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl =
      dynamic_cast<Net_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Net_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF
  if (!runtimeStatistic_impl->initialize (configuration_in.statisticReportingInterval, // reporting interval (seconds)
                                          configuration_in.printFinalReport,           // print final report ?
                                          configuration_in.messageAllocator))          // message allocator handle
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (runtimeStatistic_.name ())));
    return false;
  } // end IF
  result = inherited::push (&runtimeStatistic_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (runtimeStatistic_.name ())));
    return false;
  } // end IF

  // ******************* Header Parser ************************
  headerParser_.initialize (*configuration_in.moduleConfiguration);
  Net_Module_HeaderParser* headerParser_impl = NULL;
  headerParser_impl =
      dynamic_cast<Net_Module_HeaderParser*> (headerParser_.writer ());
  if (!headerParser_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Net_Module_HeaderParser> failed, aborting\n")));
    return false;
  } // end IF
  if (!headerParser_impl->initialize ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (headerParser_.name ())));
    return false;
  } // end IF
  result = inherited::push (&headerParser_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (headerParser_.name ())));
    return false;
  } // end IF

  // ******************* Socket Handler ************************
  socketHandler_.initialize (*configuration_in.moduleConfiguration);
  Net_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl =
      dynamic_cast<Net_Module_SocketHandler*> (socketHandler_.writer ());
  if (!socketHandler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Net_Module_SocketHandler> failed, aborting\n")));
    return false;
  } // end IF
  if (!socketHandler_impl->initialize (&state_,
                                       configuration_in.messageAllocator,
                                       configuration_in.useThreadPerConnection,
                                       NET_STATISTICS_COLLECTION_INTERVAL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (socketHandler_.name ())));
    return false;
  } // end IF

  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  socketHandler_.arg (&const_cast<Net_UserData_t&> (userData_in));
  result = inherited::push (&socketHandler_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (socketHandler_.name ())));
    return false;
  } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  inherited::allocator_ = configuration_in.messageAllocator;

  // OK: all went well
  inherited::isInitialized_ = true;
//   inherited::dump_state ();

  return true;
}

void
RPG_Net_Stream::ping ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::ping"));

  RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
  protocolHandler_impl =
    dynamic_cast<RPG_Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
  if (!protocolHandler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Module_ProtocolHandler> failed, returning\n")));
    return;
  } // end IF

  // delegate to this module...
  protocolHandler_impl->handleTimeout (NULL);
}

bool
RPG_Net_Stream::collect (Stream_Statistic_t& data_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::collect"));

  Net_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl =
    dynamic_cast<Net_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Net_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // delegate to the statistics module...
  if (!runtimeStatistic_impl->collect (data_out))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Module_Statistic_WriterTask_t::collect(), aborting\n")));
    return false;
  } // end IF
  data_out = inherited::state_.currentStatistics;

  return true;
}

void
RPG_Net_Stream::report () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Stream::report"));

//   Net_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = dynamic_cast<Net_Module_Statistic_WriterTask_t*> (//runtimeStatistic_.writer ());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("dynamic_cast<Net_Module_Statistic_WriterTask_t> failed, returning\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report ());

  // just a dummy
  ACE_ASSERT (false);

  ACE_NOTREACHED (return;)
}
