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
   myDataSink(NULL),
   myAllocator(NULL),
   myDefaultBufferSize(RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE),
   myAutomaticPong(false), // *NOTE*: the idea really is not to play PONG...
   myPrintPingPongDot(false),
   myIsInitialized(false),
   myConnectionIsAlive(false),
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
      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("[%u]: received \"%s\" [%u]\n"),
                 message_inout->getID(),
                 RPG_Net_Protocol_Tools::IRCCode2String(message_inout->getData()->command.numeric).c_str(),
                 message_inout->getData()->command.numeric));

      switch (message_inout->getData()->command.numeric)
      {
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:
        case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:
        case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:
        case RPG_Net_Protocol_IRC_Codes::RPL_BOUNCE:
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:
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
                     ACE_TEXT("[%u]: unknown numeric command (was: %u), continuing\n"),
                     message_inout->getID(),
                     message_inout->getData()->command.numeric));

          return;
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
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"JOIN\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"MODE\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::NOTICE:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"NOTICE\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

          // remember first contact...
          if (!myReceivedInitialNotice)
            myReceivedInitialNotice = true;

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PING:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"PING\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

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
                             std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::PONG)));
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
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"PONG\": \"%s\"\n"),
                     message_inout->getID(),
                     message_inout->getData()->params.back().c_str()));

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

  // refer the data back to our client
  if (myDataSink)
  {
    try
    {
      myDataSink->notify(*message_inout->getData());
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("caught exception in RPG_Net_Protocol_INotify::notify(), continuing\n")));
    }
  } // end IF
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
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      myConnectionIsAlive = true;

      break;
    }
    case Stream_SessionMessage::MB_STREAM_SESSION_END:
    {
      // remember connection has been closed...
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      myConnectionIsAlive = false;

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
RPG_Net_Protocol_Module_IRCHandler::joinIRC(const RPG_Net_Protocol_IRCLoginOptions& loginOptions_in,
                                            RPG_Net_Protocol_INotify* dataCallback_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::joinIRC"));

  // in this case, "joining" IRC implies the following 5 distinct steps:
  // --> see also RFC1459
  // 1. establish a connection (done ?)
  // 2. send PASS
  // 3. send NICK
  // 4. send USER
  // 5. join a channel

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);
  ACE_ASSERT(myIsInitialized);
  if (!myReceivedInitialNotice)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not (yet ?) connected, aborting\n")));

    return;
  } // end IF

  // step0: init data callback
  myDataSink = dataCallback_in;

  // step1: ...is done, otherwise "this" shouldn't really exist

  // step2a: init PASS
  RPG_Net_Protocol_IRCMessage* pass_struct = NULL;
  ACE_NEW_NORETURN(pass_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(pass_struct);
  ACE_NEW_NORETURN(pass_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::PASS)));
  ACE_ASSERT(pass_struct->command.string);
  pass_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  pass_struct->params.push_back(loginOptions_in.password);

  // step2b: send it upstream
  sendMessage(pass_struct);

  // step3a: init NICK
  RPG_Net_Protocol_IRCMessage* nick_struct = NULL;
  ACE_NEW_NORETURN(nick_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(nick_struct);
  ACE_NEW_NORETURN(nick_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::NICK)));
  ACE_ASSERT(nick_struct->command.string);
  nick_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  nick_struct->params.push_back(loginOptions_in.nick);

  // step3b: send it upstream
  sendMessage(nick_struct);

  // step4a: init USER
  RPG_Net_Protocol_IRCMessage* user_struct = NULL;
  ACE_NEW_NORETURN(user_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(user_struct);
  ACE_NEW_NORETURN(user_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::USER)));
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

  // step4b: send it upstream
  sendMessage(user_struct);

  // step5a: init JOIN
  RPG_Net_Protocol_IRCMessage* join_struct = NULL;
  ACE_NEW_NORETURN(join_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(join_struct);
  ACE_NEW_NORETURN(join_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::JOIN)));
  ACE_ASSERT(join_struct->command.string);
  join_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
//   std::string channel_name = ACE_TEXT_ALWAYS_CHAR("#");
//   channel_name += loginOptions_in.channel;
  join_struct->params.push_back(loginOptions_in.channel);

  // step5b: send it upstream
  sendMessage(join_struct);

  // remember our channel...
  myChannelName = loginOptions_in.channel;
}

void
RPG_Net_Protocol_Module_IRCHandler::registerNotification(RPG_Net_Protocol_INotify* dataCallback_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::registerNotification"));

  // sanity check(s)
  ACE_ASSERT(dataCallback_in);

  myDataSink = dataCallback_in;
}

void
RPG_Net_Protocol_Module_IRCHandler::sendMessage(const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::sendMessage"));

  // sanity check(s)
  ACE_ASSERT(!myChannelName.empty());

  // step1: init PRIVMSG
  RPG_Net_Protocol_IRCMessage* msg_struct = NULL;
  ACE_NEW_NORETURN(msg_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(msg_struct);
  ACE_NEW_NORETURN(msg_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::PRIVMSG)));
  ACE_ASSERT(msg_struct->command.string);
  msg_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  msg_struct->params.push_back(myChannelName);
  msg_struct->params.push_back(message_in);

  // step2: send it upstream
  sendMessage(msg_struct);
}

void
RPG_Net_Protocol_Module_IRCHandler::leaveIRC(const std::string& reason_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::leaveIRC"));

  // step1: init QUIT
  RPG_Net_Protocol_IRCMessage* quit_struct = NULL;
  ACE_NEW_NORETURN(quit_struct,
                   RPG_Net_Protocol_IRCMessage());
  ACE_ASSERT(quit_struct);
  ACE_NEW_NORETURN(quit_struct->command.string,
                   std::string(RPG_Net_Protocol_Message::messageType2String(RPG_Net_Protocol_IRCMessage::QUIT)));
  ACE_ASSERT(quit_struct->command.string);
  quit_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
  if (!reason_in.empty())
    quit_struct->params.push_back(reason_in);

  // step2: send it upstream
  sendMessage(quit_struct);

  // forget our channel...
  myChannelName.clear();
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
  // *WARNING*: protect against async closure of the connection while we
  // propagate our message...
  // --> grab our lock
  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
  // sanity check
  if (!myConnectionIsAlive)
  {
    // debug info
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
