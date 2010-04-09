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

#include "rpg_net_module_protocolhandler.h"

#include "rpg_net_message.h"
#include "rpg_net_common_tools.h"

#include <stream_iallocator.h>

RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler()
 : //inherited(),
   myAllocator(NULL),
   myPlayPong(false), // *NOTE*: the idea really is not to play PONG...
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::RPG_Net_Module_ProtocolHandler"));

}

RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::~RPG_Net_Module_ProtocolHandler"));

}

const bool
RPG_Net_Module_ProtocolHandler::init(Stream_IAllocator* allocator_in,
                                     const bool& autoAnswerPings_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);

  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset state
    myAllocator = NULL;
    myPlayPong = false;

    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;
  myPlayPong = autoAnswerPings_in;

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_ProtocolHandler::handleDataMessage(Stream_MessageBase*& message_inout,
                                                  bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // retrieve type of message and other details...
  RPG_Net_MessageHeader* message_header = ACE_reinterpret_cast(RPG_Net_MessageHeader*,
                                                               message_inout->rd_ptr());
  switch (message_header->messageType)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      // auto-answer ?
      if (myPlayPong)
      {
        // --> reply with a "PONG"

        // step0: create reply structure
        // --> get a message buffer
        RPG_Net_Message* reply_message = allocateMessage(sizeof(RPG_Net_Remote_Comm::PongMessage));
        if (reply_message == NULL)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to allocate reply message(%u), aborting\n"),
                     sizeof(RPG_Net_Remote_Comm::PongMessage)));

          return;
        } // end IF
        // step1: init reply
        RPG_Net_Remote_Comm::PongMessage* reply_struct = ACE_reinterpret_cast(RPG_Net_Remote_Comm::PongMessage*,
                                                                              reply_message->wr_ptr());
        ACE_OS::memset(reply_struct,
                       0,
                       sizeof(RPG_Net_Remote_Comm::PongMessage));
        reply_struct->messageHeader.messageLength = sizeof(RPG_Net_Remote_Comm::PongMessage) - sizeof(unsigned long);
        reply_struct->messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PONG;
        reply_message->wr_ptr(sizeof(RPG_Net_Remote_Comm::PongMessage));
        // step2: send it upstream
        if (reply(reply_message, NULL) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::reply(): \"%p\", aborting\n")));

          // clean up
          reply_message->release();

          return;
        } // end IF
      } // end IF

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      // nothing to do...
      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%u]: unknown message type: \"%s\": protocol error, aborting\n"),
                 message_inout->getID(),
                 RPG_Net_Common_Tools::messageType2String(message_header->messageType).c_str()));

      break;
    }
  } // end SWITCH
}

void
RPG_Net_Module_ProtocolHandler::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}

RPG_Net_Message*
RPG_Net_Module_ProtocolHandler::allocateMessage(const unsigned long& requestedSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_ProtocolHandler::allocateMessage"));

  // init return value(s)
  RPG_Net_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Message*,
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
