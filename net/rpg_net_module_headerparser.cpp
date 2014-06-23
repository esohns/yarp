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

#include "rpg_net_module_headerparser.h"

#include "rpg_net_sessionmessage.h"
#include "rpg_net_message.h"

#include "rpg_common_macros.h"

RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser()
 : //inherited(),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser"));

}

RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser"));

}

const bool
RPG_Net_Module_HeaderParser::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    myIsInitialized = false;
  } // end IF

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_HeaderParser::handleDataMessage(RPG_Net_Message*& message_inout,
                                               bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

  // interpret the message header...

  if (message_inout->length() < sizeof(RPG_Net_MessageHeader))
  {
    if (!message_inout->crunchForHeader(sizeof(RPG_Net_MessageHeader)))
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("[%u]: failed to RPG_Net_Message::crunchForHeader(%u), aborting\n"),
                message_inout->getID(),
                sizeof(RPG_Net_MessageHeader)));

      // clean up
      message_inout->release();
      passMessageDownstream_out = false;

      return;
    } // end IF
  } // end IF

//   // OK: retrieve type of message and other details...
//   RPG_Net_MessageHeader* message_header = reinterpret_cast<RPG_Net_MessageHeader*> (//                                                                message_inout->rd_ptr());

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("received protocol message (ID: %u): [length: %u; type: \"%s\"]...\n"),
//              message_inout->getID(),
//              message_header->messageLength,
//              RPG_Net_Common_Tools::messageType2String(message_header->messageType).c_str()));
}

void
RPG_Net_Module_HeaderParser::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::dump_state"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}
