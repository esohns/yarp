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

const char protocol_stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("YarpProtocolNetworkStream");

RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream ()
 : inherited ()
 //, headerParser_ (std::string ("HeaderParser"),
 //                 NULL)
 // , runtimeStatistic_ (std::string ("RuntimeStatistic"),
 //                      NULL)
 , IOHandler_ (this,
               ACE_TEXT_ALWAYS_CHAR ("IOHandler"))
 , protocolHandler_ (this,
                     ACE_TEXT_ALWAYS_CHAR ("ProtocolHandler"))
 //, eventHandler_ (this,
 //                 ACE_TEXT_ALWAYS_CHAR ("EventHandler"))
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::RPG_Net_Protocol_Stream"));

}

RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::~RPG_Net_Protocol_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
RPG_Net_Protocol_Stream::load (Stream_ILayout* layout_in,
                               bool& delete_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::load"));

  // sanity check(s)
  //ACE_ASSERT (inherited::configuration_);
  //ACE_ASSERT (inherited::configuration_->moduleHandlerConfiguration);

  Stream_Module_t* module_p = &IOHandler_;
  layout_in->append (module_p, NULL, 0);
  module_p = NULL;
  module_p = &protocolHandler_;
  layout_in->append (module_p, NULL, 0);
  module_p = NULL;
  //module_p = &eventHandler_;
  //layout_in->append (module_p, NULL, 0);
  //module_p = NULL;

  delete_out = false;

  return true;
}

bool
RPG_Net_Protocol_Stream::initialize (const inherited::CONFIGURATION_T& configuration_in,
                                     ACE_HANDLE handle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  //  bool result = false;
  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  struct RPG_Net_Protocol_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct RPG_Net_Protocol_ModuleHandlerConfiguration* configuration_p = NULL;
  typename inherited::ISTREAM_T::MODULE_T* module_p = NULL;
  typename inherited::WRITER_T* head_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in,
                              handle_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Module_Net_IO_Stream_T::initialize(), aborting\n"),
                ACE_TEXT (protocol_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  session_data_p =
    &const_cast<struct RPG_Net_Protocol_SessionData&> (inherited::sessionData_->getR ());
  // *TODO*: remove type inferences
  // sanity check(s)
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  configuration_p = (*iterator).second.second;
  ACE_ASSERT (configuration_p);

  // ---------------------------------------------------------------------------

  if (!inherited::setup (configuration_in.configuration_->notificationStrategy))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                ACE_TEXT (protocol_stream_name_string_)));
    goto error;
  } // end IF

  // -------------------------------------------------------------

  // OK: all went well
  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
      setup_pipeline;

  return false;
}

// void
// RPG_Net_Protocol_Stream::ping ()
// {
//   RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::ping"));

//   // delegate to this module...
//   Common_ITimerHandler* timer_handler_p = NULL;
//   timer_handler_p =
//     dynamic_cast<Common_ITimerHandler*> (protocolHandler_.writer ());
//   ACE_ASSERT (timer_handler_p);

//   try {
//     timer_handler_p->handle (NULL);
//   } catch (...) {
//     ACE_DEBUG ((LM_CRITICAL,
//                 ACE_TEXT ("%s: caught exception in Common_ITimerHandler::handle, continuing\n"),
//                 ACE_TEXT (protocol_stream_name_string_)));
//   }
// }

//bool
//RPG_Net_Protocol_Stream::collect (RPG_Net_Protocol_RuntimeStatistic& data_out)
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::collect"));
//
//  RPG_Net_Protocol_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
//  runtimeStatistic_impl =
//    dynamic_cast<RPG_Net_Protocol_Module_Statistic_WriterTask_t*> (const_cast<RPG_Net_Protocol_Module_RuntimeStatistic_Module&> (myRuntimeStatistic).writer ());
//  if (!runtimeStatistic_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<RPG_Net_Module_RuntimeStatistic> failed, aborting\n")));
//    return false;
//  } // end IF
//
//  // delegate to this module...
//  return (runtimeStatistic_impl->collect (data_out));
//}
//
//void
//RPG_Net_Protocol_Stream::report () const
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Stream::report"));
//
////   RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
////   runtimeStatistic_impl = dynamic_cast<RPG_Net_Module_RuntimeStatistic*> (//                                            myRuntimeStatistic.writer());
////   if (!runtimeStatistic_impl)
////   {
////     ACE_DEBUG((LM_ERROR,
////                ACE_TEXT("dynamic_cast<RPG_Net_Module_RuntimeStatistic) failed> (aborting\n")));
////
////     return;
////   } // end IF
////
////   // delegate to this module...
////   return (runtimeStatistic_impl->report());
//
//  // just a dummy
//  ACE_ASSERT (false);
//
//  ACE_NOTREACHED (return;)
//}
