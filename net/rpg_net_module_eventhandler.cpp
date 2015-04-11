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

#include "rpg_net_module_eventhandler.h"

#include "rpg_common_macros.h"

RPG_Net_Module_EventHandler::RPG_Net_Module_EventHandler ()
 : inherited ()
 , delete_ (false)
 , lock_ (NULL)
 , subscribers_ (NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::RPG_Net_Module_EventHandler"));

}

RPG_Net_Module_EventHandler::~RPG_Net_Module_EventHandler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::~RPG_Net_Module_EventHandler"));

  // clean up
  if (delete_)
  {
    delete lock_;
    delete subscribers_;
  } // end IF
}

void
RPG_Net_Module_EventHandler::initialize (Net_Subscribers_t* subscribers_in,
                                         ACE_Recursive_Thread_Mutex* lock_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::initialize"));

  // sanity check(s)
  ACE_ASSERT ((subscribers_in && lock_in) ||
              (!subscribers_in && !lock_in));

  // clean up ?
  if (delete_)
  {
    delete_ = false;
    delete lock_;
    lock_ = NULL;
    delete subscribers_;
    subscribers_ = NULL;
  } // end IF

  delete_ = (!lock_in && !subscribers_in);
  if (lock_in)
    lock_ = lock_in;
  else
  {
    ACE_NEW_NORETURN (lock_,
                      ACE_Recursive_Thread_Mutex (NULL, NULL));
    if (!lock_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));

      // clean up
      delete_ = false;

      return;
    } // end IF
  } // end IF
  if (subscribers_in)
    subscribers_ = subscribers_in;
  else
  {
    ACE_NEW_NORETURN (subscribers_,
                      Net_Subscribers_t ());
    if (!subscribers_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));

      // clean up
      delete_ = false;
      delete lock_;

      return;
    } // end IF
  } // end IF
}

void
RPG_Net_Module_EventHandler::handleDataMessage (Net_Message*& message_inout,
                                                bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (message_inout);

  //   try
  //   {
  //     message_inout->getData ()->dump_state ();
  //   }
  //   catch (...)
  //   {
  //     ACE_DEBUG ((LM_ERROR,
  //                 ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
  //   }

  // refer the data back to any subscriber(s)

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

    // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
    // would happen, as the current iter would be invalidated
    // --> use a slightly modified for-loop (advance first and THEN invoke the
    // callback (*NOTE*: works for MOST containers...)
    // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
    for (Net_SubscribersIterator_t iterator = subscribers_->begin ();
         iterator != subscribers_->end ();
         )
    {
      try
      {
        (*iterator++)->notify (*message_inout);
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Common_INotify::notify (), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

void
RPG_Net_Module_EventHandler::handleSessionMessage (Net_SessionMessage*& message_inout,
                                                   bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (message_inout);

  switch (message_inout->getType ())
  {
    case SESSION_BEGIN:
    {
      // refer the data back to any subscriber(s)
      //      inherited::MODULE_T* module_p = inherited::module ();
      RPG_Net_Module_EventHandler_Module* module_p =
        dynamic_cast<RPG_Net_Module_EventHandler_Module*> (inherited::module ());
      ACE_ASSERT (module_p);
      Stream_ModuleConfiguration_t* configuration_p = NULL;
      module_p->get (configuration_p);
      ACE_ASSERT (configuration_p);

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iterator would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke
        //     the callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works because the lock is recursive
        for (Net_SubscribersIterator_t iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try
          {
            (*iterator++)->start (*configuration_p);
          }
          catch (...)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in Common_INotify::start(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    case SESSION_END:
    {
      // refer the data back to any subscriber(s)

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iter would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the
        // callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
        for (Net_SubscribersIterator_t iterator = subscribers_->begin ();
             iterator != subscribers_->end ();
             )
        {
          try
          {
            (*(iterator++))->end ();
          }
          catch (...)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in Common_INotify::end(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    default:
      break;
  } // end SWITCH
}

void
RPG_Net_Module_EventHandler::subscribe (Net_Notification_t* interfaceHandle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::subscribe"));

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);
  //if (!interfaceHandle_in)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("invalid argument (was: %@), returning\n"),
  //              interfaceHandle_in));
  //  return;
  //} // end IF

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

  subscribers_->push_back (interfaceHandle_in);
}

void
RPG_Net_Module_EventHandler::unsubscribe (Net_Notification_t* interfaceHandle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT (lock_ && subscribers_);
  ACE_ASSERT (interfaceHandle_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

  Net_SubscribersIterator_t iterator = subscribers_->begin ();
  for (;
       iterator != subscribers_->end ();
       iterator++)
    if ((*iterator) == interfaceHandle_in)
      break;

  if (iterator != subscribers_->end ())
    subscribers_->erase (iterator);
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was: %@), continuing\n"),
                interfaceHandle_in));
}

Common_Module_t*
RPG_Net_Module_EventHandler::clone ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Module_EventHandler::clone"));

  // init return value(s)
  Common_Module_t* result = NULL;

  ACE_NEW_NORETURN (result,
                    RPG_Net_Module_EventHandler_Module (ACE_TEXT_ALWAYS_CHAR (inherited::name ()),
                    NULL));
  if (!result)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
                sizeof (RPG_Net_Module_EventHandler_Module)));
  else
  {
    RPG_Net_Module_EventHandler* eventHandler_impl = NULL;
    eventHandler_impl =
      dynamic_cast<RPG_Net_Module_EventHandler*> (result->writer ());
    if (!eventHandler_impl)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<RPG_Net_Module_EventHandler> failed, aborting\n")));

      // clean up
      delete result;

      return NULL;
    } // end IF
    eventHandler_impl->initialize (delete_ ? subscribers_ : NULL,
                                   delete_ ? lock_ : NULL);
  } // end ELSE

  return result;
}
