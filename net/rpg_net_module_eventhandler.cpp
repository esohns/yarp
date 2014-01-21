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

#include "rpg_net_sessionmessage.h"

RPG_Net_Module_EventHandler::RPG_Net_Module_EventHandler()
// : inherited(),
//   myLock(),
//   mySubscribers(),
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::RPG_Net_Module_EventHandler"));

}

RPG_Net_Module_EventHandler::~RPG_Net_Module_EventHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::~RPG_Net_Module_EventHandler"));

}

void
RPG_Net_Module_EventHandler::handleDataMessage(RPG_Net_Message*& message_inout,
                                               bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

//   // debug info
//   try
//   {
//     message_inout->getData()->dump_state();
//   }
//   catch (...)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("caught exception in RPG_Common_IDumpState::dump_state(), continuing\n")));
//   }

  // refer the data back to any subscriber(s)

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
    // would happen, as the current iter would be invalidated
    // --> use a slightly modified for-loop (advance first and THEN invoke the
    // callback (*NOTE*: works for MOST containers...)
    // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
    for (SUBSCRIBERSITERATOR_T iter = mySubscribers.begin();
         iter != mySubscribers.end();)
    {
      try
      {
        (*iter++)->notify(*message_inout);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Net_INotify::notify(), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

void
RPG_Net_Module_EventHandler::handleSessionMessage(RPG_Net_SessionMessage*& message_inout,
                                                  bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // refer the data back to any subscriber(s)

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iter would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the
        // callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
        for (SUBSCRIBERSITERATOR_T iter = mySubscribers.begin();
             iter != mySubscribers.end();)
        {
          try
          {
            (*iter++)->start();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_INotify::start(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // refer the data back to any subscriber(s)

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iter would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the
        // callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
        for (SUBSCRIBERSITERATOR_T iter = mySubscribers.begin();
             iter != mySubscribers.end();)
        {
          try
          {
            (*(iter++))->end();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_INotify::end(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    default:
    {
      // don't do anything...
      break;
    }
  } // end SWITCH
}

void
RPG_Net_Module_EventHandler::subscribe(RPG_Net_INotify_t* dataCallback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::subscribe"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  mySubscribers.push_back(dataCallback_in);
}

void
RPG_Net_Module_EventHandler::unsubscribe(RPG_Net_INotify_t* dataCallback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_EventHandler::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  SUBSCRIBERSITERATOR_T iterator = mySubscribers.begin();
  for (;
       iterator != mySubscribers.end();
       iterator++)
    if ((*iterator) == dataCallback_in)
      break;

  if (iterator != mySubscribers.end())
    mySubscribers.erase(iterator);
  else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was: %@), aborting\n"),
               dataCallback_in));
}
