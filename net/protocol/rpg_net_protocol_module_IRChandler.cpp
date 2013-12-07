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

#include "rpg_net_protocol_module_IRChandler.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_inotify.h"
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_tools.h"

#include "rpg_stream_iallocator.h"

#include "rpg_common_macros.h"

#include <iostream>
#include <sstream>

RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler()
 : //inherited(),
//    mySubscribers(),
   myAllocator(NULL),
   myDefaultBufferSize(RPG_NET_PROTOCOL_BUFFER_SIZE),
   myAutomaticPong(false), // *NOTE*: the idea really is not to play PONG...
   myPrintPingPongDot(false),
   myIsInitialized(false),
   myCondition(myConditionLock),
   myConnectionIsAlive(false),
   myReceivedInitialNotice(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler"));

}

RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler"));

}

const bool
RPG_Net_Protocol_Module_IRCHandler::init(RPG_Stream_IAllocator* allocator_in,
                                         const unsigned int& defaultBufferSize_in,
                                         const bool& autoAnswerPings_in,
                                         const bool& printPingPongDot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset state
    myAllocator = NULL;
    myDefaultBufferSize = RPG_NET_PROTOCOL_BUFFER_SIZE;
    myAutomaticPong = false;
    myPrintPingPongDot = false;
    myIsInitialized = false;
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

      myConnectionIsAlive = false;
    } // end lock scope
    myReceivedInitialNotice = false;
  } // end IF

  myAllocator = allocator_in;
  myDefaultBufferSize = defaultBufferSize_in;
  myAutomaticPong = autoAnswerPings_in;
//   if (myAutomaticPong)
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("auto-answering ping messages...\n")));
  myPrintPingPongDot = printPingPongDot_in;

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCHandler::handleDataMessage(RPG_Net_Protocol_Message*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout->getData());

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

  switch (message_inout->getData()->command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
//       // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: received \"%s\" [%u]\n"),
//                  message_inout->getID(),
//                  RPG_Net_Protocol_Tools::IRCCode2String(message_inout->getData()->command.numeric).c_str(),
//                  message_inout->getData()->command.numeric));

      switch (message_inout->getData()->command.numeric)
      {
        // *NOTE* these are the "regular" (== known) codes
        // [sent by ircd-hybrid-7.2.3 and others]...
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:              //   1
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:             //   2
        case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:              //   3
        case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:               //   4
        case RPG_Net_Protocol_IRC_Codes::RPL_PROTOCTL:             //   5
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURID:               //  42
        case RPG_Net_Protocol_IRC_Codes::RPL_STATSDLINE:           // 250
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:          // 251
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSEROP:              // 252
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERUNKNOWN:         // 253
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCHANNELS:        // 254
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERME:              // 255
        case RPG_Net_Protocol_IRC_Codes::RPL_TRYAGAIN:             // 263
        case RPG_Net_Protocol_IRC_Codes::RPL_LOCALUSERS:           // 265
        case RPG_Net_Protocol_IRC_Codes::RPL_GLOBALUSERS:          // 266
        case RPG_Net_Protocol_IRC_Codes::RPL_USERHOST:             // 302
        case RPG_Net_Protocol_IRC_Codes::RPL_UNAWAY:               // 305
        case RPG_Net_Protocol_IRC_Codes::RPL_NOWAWAY:              // 306
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHO:             // 315
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTSTART:            // 321
        case RPG_Net_Protocol_IRC_Codes::RPL_LIST:                 // 322
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTEND:              // 323
        case RPG_Net_Protocol_IRC_Codes::RPL_TOPIC:                // 332
        case RPG_Net_Protocol_IRC_Codes::RPL_TOPICWHOTIME:         // 333
        case RPG_Net_Protocol_IRC_Codes::RPL_INVITING:             // 341
        case RPG_Net_Protocol_IRC_Codes::RPL_WHOREPLY:             // 352
        case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY:             // 353
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES:           // 366
        case RPG_Net_Protocol_IRC_Codes::RPL_BANLIST:              // 367
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFBANLIST:         // 368
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTD:                 // 372
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTDSTART:            // 375
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMOTD:            // 376
        case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHNICK:           // 401
        case RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE:        // 433
        case RPG_Net_Protocol_IRC_Codes::ERR_NEEDMOREPARAMS:       // 461
        case RPG_Net_Protocol_IRC_Codes::ERR_YOUREBANNEDCREEP:     // 465
        case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNAME:          // 479
        case RPG_Net_Protocol_IRC_Codes::ERR_CHANOPRIVSNEEDED:     // 482
        case RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG:     // 501
        {

          break;
        }
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: received unknown (numeric) command/reply: \"%s\" (%u), continuing\n"),
                     message_inout->getID(),
                     RPG_Net_Protocol_Tools::IRCCode2String(message_inout->getData()->command.numeric).c_str(),
                     message_inout->getData()->command.numeric));

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      switch (RPG_Net_Protocol_Tools::IRCCommandString2Type(*message_inout->getData()->command.string))
      {
        case RPG_Net_Protocol_IRCMessage::NICK:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"NICK\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::QUIT:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"QUIT\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::JOIN:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"JOIN\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PART:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"PART\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"MODE\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::TOPIC:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"TOPIC\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::NAMES:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"NAMES\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::LIST:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"LIST\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::INVITE:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"INVITE\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::KICK:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"KICK\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PRIVMSG:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"PRIVMSG\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::NOTICE:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"NOTICE\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          // remember first contact
          if (!myReceivedInitialNotice)
            myReceivedInitialNotice = true;

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PING:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"PING\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          // auto-answer ?
          if (myAutomaticPong)
          {
            // --> reply with a "PONG"

            // step0: init reply
            RPG_Net_Protocol_IRCMessage* reply_struct = NULL;
            ACE_NEW_NORETURN(reply_struct,
                             RPG_Net_Protocol_IRCMessage());
            ACE_ASSERT(reply_struct);
            ACE_NEW_NORETURN(reply_struct->command.string,
                             std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::PONG)));
            ACE_ASSERT(reply_struct->command.string);
            reply_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
            reply_struct->params.push_back(message_inout->getData()->params.back());

            // step1: send it upstream
            sendMessage(reply_struct);
          } // end IF

          if (myPrintPingPongDot)
          {
            std::clog << '.';
          } // end IF

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PONG:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"PONG\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
#if defined ACE_WIN32 || defined ACE_WIN64
#pragma message("applying quirk code for this compiler...")
		case RPG_Net_Protocol_IRCMessage::__QUIRK__ERROR:
#else
        case RPG_Net_Protocol_IRCMessage::ERROR:
#endif
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"ERROR\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::AWAY:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"AWAY\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::USERS:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"USERS\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::USERHOST:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("[%u]: received \"USERHOST\": \"%s\"\n"),
//                      message_inout->getID(),
//                      message_inout->getData()->params.back().c_str()));

          break;
        }
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: received unknown command (was: \"%s\"), continuing\n"),
                     message_inout->getID(),
                     message_inout->getData()->command.string->c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%u]: invalid command type (was: %u), continuing\n"),
                 message_inout->getID(),
                 message_inout->getData()->command.discriminator));

      break;
    }
  } // end SWITCH

  // refer the data back to our subscriber(s)

  // synch access to our subscribers
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    // *WARNING* if the user unsubscribes() within the callback
    // BAD THINGS (TM) WILL happen, because the current iter WILL be invalidated
    // --> use a slightly modified for-loop (advance first and THEN invoke the callback,
    // works for MOST containers)
    // *NOTE*: this can only happen due to the ACE_RECURSIVE_Thread_Mutex
    // we use as a lock in order to avoid deadlocks in precisely this situation...
    for (SubscribersIterator_t iter = mySubscribers.begin();
         iter != mySubscribers.end();)
    {
      try
      {
        (*iter++)->notify(*message_inout->getData());
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Net_Protocol_INotify::notify(), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

void
RPG_Net_Protocol_Module_IRCHandler::handleSessionMessage(RPG_Net_Protocol_SessionMessage*& message_inout,
                                                         bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  switch (message_inout->getType())
  {
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // remember connection has been opened...
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

        myConnectionIsAlive = true;

        // signal any waiter(s)
        myCondition.broadcast();
      } // end lock scope

      // refer this information back to our subscriber(s)
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("session starting, notifying %u subscriber(s)...\n"),
//                    mySubscribers.size()));

        // *WARNING* if the user unsubscribes() within the callback
        // BAD THINGS (TM) WILL happen, because the current iter WILL be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the callback,
        // works for MOST containers)
        // *NOTE*: this can only happen due to the ACE_RECURSIVE_Thread_Mutex
        // we use as a lock in order to avoid deadlocks in precisely this situation...
        for (SubscribersIterator_t iter = mySubscribers.begin();
             iter != mySubscribers.end();)
        {
          try
          {
            (*iter++)->start();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_Protocol_INotify::start(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    case RPG_Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // refer this information back to our subscriber(s)
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("session ending, notifying %u subscriber(s)...\n"),
//                    mySubscribers.size()));

        // *WARNING* if the user unsubscribes() within the callback
        // BAD THINGS (TM) WILL happen, because the current iter WILL be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the callback,
        // works for MOST containers)
        // *NOTE*: this can only happen due to the ACE_RECURSIVE_Thread_Mutex
        // we use as a lock in order to avoid deadlocks in precisely this situation...
        for (SubscribersIterator_t iter = mySubscribers.begin();
             iter != mySubscribers.end();)
        {
          try
          {
            (*(iter++))->end();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_Protocol_INotify::end(), continuing\n")));
          }
        } // end FOR

        if (!mySubscribers.empty())
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("removing %u subscription(s)...\n"),
                     mySubscribers.size()));

        // clean subscribers
        mySubscribers.clear();
      } // end lock scope

      // remember connection has been closed...
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

        myConnectionIsAlive = false;

        // signal any waiter(s)
        myCondition.broadcast();
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
RPG_Net_Protocol_Module_IRCHandler::registerConnection(const RPG_Net_Protocol_IRCLoginOptions& loginOptions_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::registerConnection"));

  // "registering" an IRC connection implies the following 4 distinct steps:
  // --> see also RFC1459
  // 1. establish a connection (done ?!)
  // [2. wait for initial NOTICE (done ?!)]
  // 3. send PASS
  // 4. send NICK
  // 5. send USER

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);
  // step1: ...is done ?
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

    if (!myConnectionIsAlive)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("waiting for connection...\n")));

      // *NOTE*: can happen when trying to register IMMEDIATELY after connecting
      // --> allow a little delay for:
      // - connection to establish
      // [- the initial NOTICEs to arrive]
      // before proceeding...
      ACE_Time_Value abs_deadline = ACE_OS::gettimeofday();
      abs_deadline += RPG_NET_PROTOCOL_IRC_MAX_WELCOME_DELAY;
      if ((myCondition.wait(&abs_deadline) == -1) &&
          (ACE_OS::last_error() != ETIME))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Thread_Condition::wait(), aborting\n")));

        return;
      } // end IF

      if (!myConnectionIsAlive)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("not (yet ?) connected, aborting\n")));

        return;
      } // end IF

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("proceeding...\n")));
    } // end IF
  } // end lock scope
  // step2: ...is done ?
//   if (!myReceivedInitialNotice)
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("waiting for connection...\n")));
//
//     // *NOTE*: can happen when trying to register IMMEDIATELY after connecting
//     // --> allow a little delay for the welcome NOTICE to arrive before proceeding
//     ACE_Time_Value abs_deadline = ACE_OS::gettimeofday();
//     abs_deadline += RPG_NET_PROTOCOL_IRC_MAX_WELCOME_DELAY;
//     if ((myCondition.wait(&abs_deadline) == -1) &&
//         (ACE_OS::last_error() != ETIME))
//     {
//       ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("failed to ACE_Thread_Condition::wait(), aborting\n")));
//
//       return;
//     } // end IF
//
//     if (!myReceivedInitialNotice)
//     {
//       ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("not (yet ?) connected, aborting\n")));
//
//       return;
//     } // end IF
//
//     ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("proceeding...\n")));
//   } // end IF

  // step3a: init PASS
  RPG_Net_Protocol_IRCMessage* pass_struct = NULL;
  ACE_NEW_NORETURN(pass_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(pass_struct);
  ACE_NEW_NORETURN(pass_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::PASS)));
  ACE_ASSERT(pass_struct->command.string);
  pass_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  pass_struct->params.push_back(loginOptions_in.password);

  // step3b: send it upstream
  sendMessage(pass_struct);

  // step4a: init NICK
  RPG_Net_Protocol_IRCMessage* nick_struct = NULL;
  ACE_NEW_NORETURN(nick_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(nick_struct);
  ACE_NEW_NORETURN(nick_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::NICK)));
  ACE_ASSERT(nick_struct->command.string);
  nick_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  nick_struct->params.push_back(loginOptions_in.nick);

  // step4b: send it upstream
  sendMessage(nick_struct);

  // step5a: init USER
  RPG_Net_Protocol_IRCMessage* user_struct = NULL;
  ACE_NEW_NORETURN(user_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(user_struct);
  ACE_NEW_NORETURN(user_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::USER)));
  ACE_ASSERT(user_struct->command.string);
  user_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  user_struct->params.push_back(loginOptions_in.user.username);
  switch (loginOptions_in.user.hostname.discriminator)
  {
    case RPG_Net_Protocol_IRCLoginOptions::User::Hostname::STRING:
    {
      user_struct->params.push_back(*loginOptions_in.user.hostname.string);

      break;
    }
    case RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK:
    {
      std::ostringstream converter;
      converter << static_cast<unsigned long> (loginOptions_in.user.hostname.mode);
      user_struct->params.push_back(std::string(converter.str()));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid USER <hostname> parameter field type (was: %d), aborting\n"),
                 loginOptions_in.user.hostname.discriminator));

      // what else can we do ?
      return;
    }
  } // end SWITCH
  user_struct->params.push_back(loginOptions_in.user.servername);
  user_struct->params.push_back(loginOptions_in.user.realname);

  // step5b: send it upstream
  sendMessage(user_struct);

//   // step5a: init JOIN
//   RPG_Net_Protocol_IRCMessage* join_struct = NULL;
//   ACE_NEW_NORETURN(join_struct,
//                    RPG_Net_Protocol_IRCMessage());
//   ACE_ASSERT(join_struct);
//   ACE_NEW_NORETURN(join_struct->command.string,
//                    std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::JOIN)));
//   ACE_ASSERT(join_struct->command.string);
//   join_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
// //   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("#");
// //   channel_name += loginOptions_in.channel;
//   join_struct->params.push_back(loginOptions_in.channel);
//
//   // step5b: send it upstream
//   sendMessage(join_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::subscribe(RPG_Net_Protocol_INotify* dataCallback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::subscribe"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  mySubscribers.push_back(dataCallback_in);
}

void
RPG_Net_Protocol_Module_IRCHandler::unsubscribe(RPG_Net_Protocol_INotify* dataCallback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  SubscribersIterator_t iterator = mySubscribers.begin();
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

void
RPG_Net_Protocol_Module_IRCHandler::nick(const std::string& nick_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::nick"));

  // step1: init NICK
  RPG_Net_Protocol_IRCMessage* nick_struct = NULL;
  ACE_NEW_NORETURN(nick_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(nick_struct);
  ACE_NEW_NORETURN(nick_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::NICK)));
  ACE_ASSERT(nick_struct->command.string);
  nick_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  nick_struct->params.push_back(nick_in);

  // step2: send it upstream
  sendMessage(nick_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::quit(const std::string& reason_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::quit"));

  // step1: init QUIT
  RPG_Net_Protocol_IRCMessage* quit_struct = NULL;
  ACE_NEW_NORETURN(quit_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(quit_struct);
  ACE_NEW_NORETURN(quit_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::QUIT)));
  ACE_ASSERT(quit_struct->command.string);
  quit_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  if (!reason_in.empty())
    quit_struct->params.push_back(reason_in);

  // step2: send it upstream
  sendMessage(quit_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::join(const string_list_t& channels_in,
                                         const string_list_t& keys_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::join"));

  // step1: init JOIN
  RPG_Net_Protocol_IRCMessage* join_struct = NULL;
  ACE_NEW_NORETURN(join_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(join_struct);
  ACE_NEW_NORETURN(join_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::JOIN)));
  ACE_ASSERT(join_struct->command.string);
  join_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  // compute ranges ?
  if (!keys_in.empty())
  {
    list_item_range_t range;
    if (channels_in.size() > 1)
    {
      range.first = 0;
      range.second = (channels_in.size() - 1);
      join_struct->list_param_ranges.push_back(range);
    } // end IF
    if (keys_in.size() > 1)
    {
      range.first = channels_in.size();
      range.second = (channels_in.size() + keys_in.size() - 1);
      join_struct->list_param_ranges.push_back(range);
    } // end IF
  } // end IF
  join_struct->params = channels_in;
  // append any keys
  if (!keys_in.empty())
    join_struct->params.insert(join_struct->params.end(),
                               keys_in.begin(),
                               keys_in.end());

  // step2: send it upstream
  sendMessage(join_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::part(const string_list_t& channels_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::part"));

  // step1: init PART
  RPG_Net_Protocol_IRCMessage* part_struct = NULL;
  ACE_NEW_NORETURN(part_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(part_struct);
  ACE_NEW_NORETURN(part_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::PART)));
  ACE_ASSERT(part_struct->command.string);
  part_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  part_struct->params = channels_in;

  // step2: send it upstream
  sendMessage(part_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::mode(const std::string& target_in,
                                         const char& mode_in,
                                         const bool& enable_in,
                                         const string_list_t& parameters_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::mode"));

  // step1: init MODE
  RPG_Net_Protocol_IRCMessage* mode_struct = NULL;
  ACE_NEW_NORETURN(mode_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(mode_struct);
  ACE_NEW_NORETURN(mode_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::MODE)));
  ACE_ASSERT(mode_struct->command.string);
  mode_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  // construct parameter
  std::string mode_string = (enable_in ? ACE_TEXT_ALWAYS_CHAR("+")
                                       : ACE_TEXT_ALWAYS_CHAR("-"));
  mode_string += mode_in;

  mode_struct->params.push_back(target_in);
  mode_struct->params.push_back(mode_string);
  // append any parameters
  mode_struct->params.insert(mode_struct->params.end(),
                             parameters_in.begin(),
                             parameters_in.end());

  // step2: send it upstream
  sendMessage(mode_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::topic(const std::string& channel_in,
                                          const std::string& topic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::topic"));

  // sanity check(s)
  ACE_ASSERT(!topic_in.empty());

  // step1: init TOPIC
  RPG_Net_Protocol_IRCMessage* topic_struct = NULL;
  ACE_NEW_NORETURN(topic_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(topic_struct);
  ACE_NEW_NORETURN(topic_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::TOPIC)));
  ACE_ASSERT(topic_struct->command.string);
  topic_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  topic_struct->params.push_back(channel_in);
  topic_struct->params.push_back(topic_in);

  // step2: send it upstream
  sendMessage(topic_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::names(const string_list_t& channels_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::names"));

  // step1: init NAMES
  RPG_Net_Protocol_IRCMessage* names_struct = NULL;
  ACE_NEW_NORETURN(names_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(names_struct);
  ACE_NEW_NORETURN(names_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::NAMES)));
  ACE_ASSERT(names_struct->command.string);
  names_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  // compute range ?
  if (channels_in.size() > 1)
  {
    list_item_range_t range;
    range.first = 0;
    range.second = (channels_in.size() - 1);
    names_struct->list_param_ranges.push_back(range);
  } // end IF
  names_struct->params = channels_in;

  // step2: send it upstream
  sendMessage(names_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::list(const string_list_t& channels_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::list"));

  // step1: init LIST
  RPG_Net_Protocol_IRCMessage* list_struct = NULL;
  ACE_NEW_NORETURN(list_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(list_struct);
  ACE_NEW_NORETURN(list_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::LIST)));
  ACE_ASSERT(list_struct->command.string);
  list_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  // compute range ?
  if (channels_in.size() > 1)
  {
    list_item_range_t range;
    range.first = 0;
    range.second = (channels_in.size() - 1);
    list_struct->list_param_ranges.push_back(range);
  } // end IF
  list_struct->params =  channels_in;

  // step2: send it upstream
  sendMessage(list_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::invite(const std::string& nick_in,
                                           const std::string& channel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::invite"));

  // step1: init INVITE
  RPG_Net_Protocol_IRCMessage* invite_struct = NULL;
  ACE_NEW_NORETURN(invite_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(invite_struct);
  ACE_NEW_NORETURN(invite_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::INVITE)));
  ACE_ASSERT(invite_struct->command.string);
  invite_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  invite_struct->params.push_back(nick_in);
  invite_struct->params.push_back(channel_in);

  // step2: send it upstream
  sendMessage(invite_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::kick(const std::string& channel_in,
                                         const std::string& nick_in,
                                         const std::string& comment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::kick"));

  // step1: init KICK
  RPG_Net_Protocol_IRCMessage* kick_struct = NULL;
  ACE_NEW_NORETURN(kick_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(kick_struct);
  ACE_NEW_NORETURN(kick_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::KICK)));
  ACE_ASSERT(kick_struct->command.string);
  kick_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  kick_struct->params.push_back(channel_in);
  kick_struct->params.push_back(nick_in);
  if (!comment_in.empty())
    kick_struct->params.push_back(comment_in);

  // step2: send it upstream
  sendMessage(kick_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::send(const string_list_t& receivers_in,
                                         const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::send"));

  // sanity check
  if (message_in.empty())
    return; // nothing to do...

  // step1: init PRIVMSG
  RPG_Net_Protocol_IRCMessage* msg_struct = NULL;
  ACE_NEW_NORETURN(msg_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(msg_struct);
  ACE_NEW_NORETURN(msg_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::PRIVMSG)));
  ACE_ASSERT(msg_struct->command.string);
  msg_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  // compute range ?
  if (receivers_in.size() > 1)
  {
    list_item_range_t range;
    range.first = 0;
    range.second = (receivers_in.size() - 1);
    msg_struct->list_param_ranges.push_back(range);
  } // end IF
  msg_struct->params = receivers_in;
  msg_struct->params.push_back(message_in);

  // step2: send it upstream
  sendMessage(msg_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::who(const std::string& name_in,
                                        const bool& operatorsOnly_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::who"));

  // step1: init WHOIS
  RPG_Net_Protocol_IRCMessage* who_struct = NULL;
  ACE_NEW_NORETURN(who_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(who_struct);
  ACE_NEW_NORETURN(who_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::WHO)));
  ACE_ASSERT(who_struct->command.string);
  who_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  who_struct->params.push_back(name_in);
  if (operatorsOnly_in)
    who_struct->params.push_back(std::string("o"));

  // step2: send it upstream
  sendMessage(who_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::whois(const std::string& servername_in,
                                          const string_list_t& nicknames_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::whois"));

  // step1: init WHOIS
  RPG_Net_Protocol_IRCMessage* whois_struct = NULL;
  ACE_NEW_NORETURN(whois_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(whois_struct);
  ACE_NEW_NORETURN(whois_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::WHOIS)));
  ACE_ASSERT(whois_struct->command.string);
  whois_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  whois_struct->params = nicknames_in;
  if (!servername_in.empty())
    whois_struct->params.push_front(servername_in);

  // step2: send it upstream
  sendMessage(whois_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::whowas(const std::string& nick_in,
                                           const unsigned long& count_in,
                                           const std::string& servername_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::whowas"));

  // step1: init WHOWAS
  RPG_Net_Protocol_IRCMessage* whowas_struct = NULL;
  ACE_NEW_NORETURN(whowas_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(whowas_struct);
  ACE_NEW_NORETURN(whowas_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::WHOWAS)));
  ACE_ASSERT(whowas_struct->command.string);
  whowas_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  whowas_struct->params.push_back(nick_in);
  if (count_in)
  {
    std::stringstream converter;
    converter << count_in;
    std::string count_string;
    converter >> count_string;
    whowas_struct->params.push_back(count_string);
  } // end IF
  if (!servername_in.empty())
    whowas_struct->params.push_back(servername_in);

  // step2: send it upstream
  sendMessage(whowas_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::away(const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::away"));

  // step1: init AWAY
  RPG_Net_Protocol_IRCMessage* away_struct = NULL;
  ACE_NEW_NORETURN(away_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(away_struct);
  ACE_NEW_NORETURN(away_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::AWAY)));
  ACE_ASSERT(away_struct->command.string);
  away_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  // *NOTE*: if there is no away message parameter, the semantic is to
  // "un-away" the user
  if (!message_in.empty())
    away_struct->params.push_back(message_in);

  // step2: send it upstream
  sendMessage(away_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::users(const std::string& server_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::users"));

  // step1: init USERS
  RPG_Net_Protocol_IRCMessage* users_struct = NULL;
  ACE_NEW_NORETURN(users_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(users_struct);
  ACE_NEW_NORETURN(users_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::USERS)));
  ACE_ASSERT(users_struct->command.string);
  users_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  if (!server_in.empty())
    users_struct->params.push_back(server_in);

  // step2: send it upstream
  sendMessage(users_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::userhost(const string_list_t& nicknames_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::userhost"));

  // step1: init USERHOST
  RPG_Net_Protocol_IRCMessage* userhost_struct = NULL;
  ACE_NEW_NORETURN(userhost_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(userhost_struct);
  ACE_NEW_NORETURN(userhost_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::USERHOST)));
  ACE_ASSERT(userhost_struct->command.string);
  userhost_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;

  userhost_struct->params = nicknames_in;

  // step2: send it upstream
  sendMessage(userhost_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Protocol_Message*
RPG_Net_Protocol_Module_IRCHandler::allocateMessage(const unsigned int& requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::allocateMessage"));

  // init return value(s)
  RPG_Net_Protocol_Message* message_out = NULL;

  try
  {
    message_out = static_cast<RPG_Net_Protocol_Message*>(myAllocator->malloc(requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  } // end IF

  return message_out;
}

void
RPG_Net_Protocol_Module_IRCHandler::sendMessage(RPG_Net_Protocol_IRCMessage*& command_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::sendMessage"));

  // step1: get a message buffer
  RPG_Net_Protocol_Message* message = allocateMessage(myDefaultBufferSize);
  if (message == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate message(%u), aborting\n"),
               myDefaultBufferSize));

    return;
  } // end IF

  // step2: attach the command
  // *NOTE*: message assumes control over the reference...
  message->init(command_in);
  // --> bye bye...
  command_in = NULL;

  // step3: send it upstream
  // *NOTE*: while there is NO way to prevent async close of the CONNECTION,
  // this does protect against async closure of the STREAM WHILE we propagate
  // our message...
  // --> grab our lock and check myConnectionIsAlive
  ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);
  // sanity check
  if (!myConnectionIsAlive)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no connection - cannot send message, aborting\n")));

    // clean up
    message->release();

    // what else can we do ?
    return;
  } // end IF

  if (reply(message, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::reply(): \"%m\", aborting\n")));

    // clean up
    message->release();

    return;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("pushed message...\n")));
}
