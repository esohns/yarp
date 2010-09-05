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

#include "rpg_net_protocol_module_IRChandler.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_inotify.h"
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_tools.h"

#include <stream_iallocator.h>

#include <iostream>
#include <sstream>

RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler()
 : //inherited(),
//    mySubscribers(),
   myAllocator(NULL),
   myDefaultBufferSize(RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE),
   myAutomaticPong(false), // *NOTE*: the idea really is not to play PONG...
   myPrintPingPongDot(false),
   myIsInitialized(false),
   myConnectionIsAlive(false),
   myCondition(myConditionLock),
   myReceivedInitialNotice(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler"));

}

RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler"));

}

const bool
RPG_Net_Protocol_Module_IRCHandler::init(Stream_IAllocator* allocator_in,
                                         const unsigned long& defaultBufferSize_in,
                                         const bool& autoAnswerPings_in,
                                         const bool& printPingPongDot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset state
    myAllocator = NULL;
    myDefaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
    myAutomaticPong = false;
    myPrintPingPongDot = false;
    myIsInitialized = false;
    myConnectionIsAlive = false;
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

      myReceivedInitialNotice = false;
    } // end lock scope
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
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::handleDataMessage"));

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
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:
        case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:
        case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:
        case RPG_Net_Protocol_IRC_Codes::RPL_BOUNCE:
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCHANNELS:
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERME:
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTDSTART:
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTD:
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMOTD:
        case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY:
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES:
        {

          break;
        }
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: received (numeric) command/reply: \"%s\" (%u), continuing\n"),
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

          // remember first contact (and signal any potential waiters)
//           bool signal_waiters = false;
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);

            if (!myReceivedInitialNotice)
            {
              myReceivedInitialNotice = true;
              myCondition.broadcast();
//               signal_waiters = true;
            } // end IF
          } // end lock scope
//           if (signal_waiters)
//             myCondition.broadcast();

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
        case RPG_Net_Protocol_IRCMessage::ERROR:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"ERROR\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

          break;
        }
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: received unknown command (was: \"%s\"), aborting\n"),
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
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    for (SubscribersConstIterator_t iter = mySubscribers.begin();
         iter != mySubscribers.end();
         iter++)
    {
      try
      {
        (*iter)->notify(*message_inout->getData());
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
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

  switch (message_inout->getType())
  {
    case Stream_SessionMessage::MB_STREAM_SESSION_BEGIN:
    {
      // remember connection has been opened...
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        myConnectionIsAlive = true;
      } // end lock scope

      // refer this information back to our subscriber(s)
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        for (SubscribersConstIterator_t iter = mySubscribers.begin();
             iter != mySubscribers.end();
             iter++)
        {
          try
          {
            (*iter)->start();
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
    case Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // remember connection has been closed...
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        myConnectionIsAlive = false;
      } // end lock scope

      // refer this information back to our subscriber(s)
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        for (SubscribersConstIterator_t iter = mySubscribers.begin();
             iter != mySubscribers.end();
             iter++)
        {
          try
          {
            (*iter)->end();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_Protocol_INotify::end(), continuing\n")));
          }
        } // end FOR

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("removing %u subscription(s)...\n"),
                   mySubscribers.size()));

        // clean subscribers
        mySubscribers.clear();
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
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::registerConnection"));

  // "registering" an IRC connection implies the following 4 distinct steps:
  // --> see also RFC1459
  // 1. establish a connection (done ?!)
  // 2. wait for initial welcome NOTICE (done ?!)
  // 3. send PASS
  // 4. send NICK
  // 5. send USER

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);
  // step1: ...is done ?
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if (!myConnectionIsAlive)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("not (yet ?) connected, aborting\n")));

      return;
    } // end IF
  } // end lock scope
  // step2: ...is done ?
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myConditionLock);
    if (!myReceivedInitialNotice)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting for initial NOTICE...\n")));

      // *NOTE*: can happen when trying to register IMMEDIATELY after connecting
      // --> allow a little delay for the welcome NOTICE to arrive before proceeding
      ACE_Time_Value abs_deadline = ACE_OS::gettimeofday();
      abs_deadline += RPG_NET_PROTOCOL_IRC_MAX_WELCOME_DELAY;
      if ((myCondition.wait(&abs_deadline) == -1) &&
          (ACE_OS::last_error() != ETIME))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Thread_Condition::wait(), aborting\n")));

        return;
      } // end IF

      if (!myReceivedInitialNotice)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("not (yet ?) connected, aborting\n")));

        return;
      } // end IF

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("proceeding...\n")));
    } // end IF
  } // end lock scope

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
      converter << ACE_static_cast(unsigned long,
                                   loginOptions_in.user.hostname.mode);
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
RPG_Net_Protocol_Module_IRCHandler::notify(RPG_Net_Protocol_INotify* dataCallback_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::notify"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  // synch access to subscribers
  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  mySubscribers.push_back(dataCallback_in);
}

void
RPG_Net_Protocol_Module_IRCHandler::join(const std::string& channel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::join"));

  // step1: init JOIN
  RPG_Net_Protocol_IRCMessage* join_struct = NULL;
  ACE_NEW_NORETURN(join_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(join_struct);
  ACE_NEW_NORETURN(join_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::JOIN)));
  ACE_ASSERT(join_struct->command.string);
  join_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  //   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("#");
  //   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("&");
  //   channel_name += channel_in;
  join_struct->params.push_back(channel_in);

  // step2: send it upstream
  sendMessage(join_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::part(const std::string& channel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::part"));

  // step1: init QUIT
  RPG_Net_Protocol_IRCMessage* part_struct = NULL;
  ACE_NEW_NORETURN(part_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(part_struct);
  ACE_NEW_NORETURN(part_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::commandType2String(RPG_Net_Protocol_IRCMessage::PART)));
  ACE_ASSERT(part_struct->command.string);
  part_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  //   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("#");
  //   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("&");
  //   channel_name += channel_in;
  part_struct->params.push_back(channel_in);

  // step2: send it upstream
  sendMessage(part_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::send(const std::string& channel_in,
                                         const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::send"));

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
  msg_struct->params.push_back(channel_in);
  msg_struct->params.push_back(message_in);

  // step2: send it upstream
  sendMessage(msg_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::quit(const std::string& reason_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::quit"));

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
RPG_Net_Protocol_Module_IRCHandler::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Protocol_Message*
RPG_Net_Protocol_Module_IRCHandler::allocateMessage(const unsigned long& requestedSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::allocateMessage"));

  // init return value(s)
  RPG_Net_Protocol_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Protocol_Message*,
                                  myAllocator->malloc(requestedSize_in));
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
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::sendMessage"));

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
  // this does protect against async closure of the STREAM while we propagate
  // our message...
  // --> grab our lock and check myConnectionIsAlive
  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
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
}
