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
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_tools.h"

#include <stream_iallocator.h>

#include <iostream>

RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler()
 : //inherited(),
   myAllocator(NULL),
   myAutomaticPong(false), // *NOTE*: the idea really is not to play PONG...
   myPrintPongDot(false),
   myIsInitialized(false)//,
//    mySessionID(0)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::RPG_Net_Protocol_Module_IRCHandler"));

}

RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::~RPG_Net_Protocol_Module_IRCHandler"));

}

const bool
RPG_Net_Protocol_Module_IRCHandler::init(Stream_IAllocator* allocator_in,
                                         const bool& autoAnswerPings_in,
                                         const bool& printPongDot_in)
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
    myAutomaticPong = false;
    myPrintPongDot = false;
    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;
  myAutomaticPong = autoAnswerPings_in;
//   if (myAutomaticPong)
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("auto-answering ping messages...\n")));
  myPrintPongDot = printPongDot_in;

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCHandler::handleDataMessage(Stream_MessageBase*& message_inout,
                                                      bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  RPG_Net_Protocol_Message* message = ACE_dynamic_cast(RPG_Net_Protocol_Message*,
                                                       message_inout);
  ACE_ASSERT(message);

  switch (message->getData()->command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      switch (message->getData()->command.numeric)
      {
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: unknown numeric command (was: %u), continuing\n"),
                     message_inout->getID(),
                     message->getData()->command.numeric));

          return;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      switch (RPG_Net_Protocol_Tools::IRCCommandString2Type(*message->getData()->command.string))
      {
        case RPG_Net_Protocol_IRCMessage::NOTICE:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"NOTICE\": \"%s\"\n"),
                     message_inout->getID(),
                     message->getData()->params.back().c_str()));

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PING:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("[%u]: received \"PING\": \"%s\"\n"),
                     message_inout->getID(),
                     message->getData()->params.back().c_str()));

          // auto-answer ?
          if (myAutomaticPong)
          {
            // --> reply with a "PONG"

            // step0: create reply structure
            // --> get a message buffer
            RPG_Net_Protocol_Message* reply_message = allocateMessage(RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE);
            if (reply_message == NULL)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to allocate reply message(%u), aborting\n"),
                         RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE));

              return;
            } // end IF

            // step1: init reply
            RPG_Net_Protocol_IRCMessage* reply_struct = NULL;
            ACE_NEW_NORETURN(reply_struct,
                             RPG_Net_Protocol_IRCMessage());
            ACE_NEW_NORETURN(reply_struct->command.string,
                             std::string(RPG_Net_Protocol_Tools::IRCCommandType2String(RPG_Net_Protocol_IRCMessage::PONG)));
            reply_struct->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
            reply_struct->params.push_back(message->getData()->params.back());
            // *NOTE*: reply_message assumes control over reply...
            reply_message->init(reply_struct);

            // step2: send it upstream
            if (reply(reply_message, NULL) == -1)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to ACE_Task::reply(): \"%m\", aborting\n")));

              // clean up
              reply_message->release();

              return;
            } // end IF
          } // end IF

          if (myPrintPongDot)
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
                     message->getData()->params.back().c_str()));

          break;
        }
        default:
        {
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("[%u]: received unknown command (was: \"%s\"), aborting\n"),
                     message_inout->getID(),
                     message->getData()->command.string->c_str()));

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
                 message->getData()->command.discriminator));

      break;
    }
  } // end SWITCH
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
