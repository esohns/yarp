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

#include "rpg_net_module_headerparser.h"

#include "rpg_net_common_tools.h"

RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser()
 : //inherited(),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser"));

}

RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser"));

}

const bool
RPG_Net_Module_HeaderParser::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    // reset message counters...
    myMessageType2Counter.clear();
    myIsInitialized = false;
  } // end IF

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_HeaderParser::handleDataMessage(Stream_MessageBase*& message_inout,
                                               bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // OK: let's interpret the message header...

  // step0: do we have enough CONTIGUOUS data ?
  while (message_inout->length() < sizeof(RPG_Net_MessageHeader))
  {
    // *sigh*: copy some data from the chain to allow interpretation
    // of the message header
    // *WARNING*: for this to work, message_inout->size() must be
    // AT LEAST sizeof(RPG_Net_MessageHeader)...
    ACE_Message_Block* source = message_inout->cont();
    // skip over any "empty" continuations...
    while (source->length() == 0)
      source = source->cont();

    // copy some data...
    size_t amount = (source->length() > sizeof(RPG_Net_MessageHeader) ? sizeof(RPG_Net_MessageHeader)
                                                                      : source->length());
    if (message_inout->copy(source->rd_ptr(),
                            amount))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::copy(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

        // clean up
      message_inout->release();
      passMessageDownstream_out = false;

      // what else can we do ?
      return;
    } // end IF

      // adjust the continuation accordingly...
    source->rd_ptr(amount);
  } // end WHILE

  // sanity check
  ACE_ASSERT(message_inout->length() >= sizeof(RPG_Net_MessageHeader));

  // step1: retrieve type of message and other details...
  RPG_Net_MessageHeader* message_header = ACE_reinterpret_cast(RPG_Net_MessageHeader*,
                                                               message_inout->rd_ptr());

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("received protocol message (ID: %u): [length: %u; type: \"%s\"]...\n"),
             message_inout->getID(),
             message_header->messageLength,
             RPG_Net_Common_Tools::messageType2String(message_header->messageType).c_str()));

  // increment corresponding counter...
  myMessageType2Counter[message_header->messageType]++;
}

void
RPG_Net_Module_HeaderParser::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}
