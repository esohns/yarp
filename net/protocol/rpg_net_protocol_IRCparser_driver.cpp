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

#include "rpg_net_protocol_IRCparser_driver.h"

#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

// #include <iostream>
#include <sstream>

RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver(const bool& traceScanning_in,
                                                                   const bool& traceParsing_in)
 : myTraceScanning(traceScanning_in),
   myTraceParsing(traceParsing_in),
   myCurrentState(NULL),
   myParser(*this)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver"));

  // init parser
  myParser.set_debug_level(myTraceParsing);

  // reset the current messge
  reset();
}

RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver ()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver"));

}

const bool
RPG_Net_Protocol_IRCParserDriver::parse(const ACE_Message_Block* data_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::parse"));

  // start scanner
  if (!scan_begin(data_in->rd_ptr(), data_in->length()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse IRC message, aborting\n")));

    return false;
  } // end IF

  // parse our data
  int result = myParser.parse();

  // fini scanner
  scan_end();

  return (result == 0);
}

const RPG_Net_Protocol_IRCMessage
RPG_Net_Protocol_IRCParserDriver::getIRCMessage() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::getIRCMessage"));

  return myMessage;
}

void
RPG_Net_Protocol_IRCParserDriver::error(const IRCParse::location& location_in,
                                        const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  std::ostringstream converter;
  converter << location_in;
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse IRC message (location: %s): \"%s\"...\n"),
             converter.str(),
             message_in.c_str()));

//   std::clog << location_in << ": " << message_in << std::endl;
}

void
RPG_Net_Protocol_IRCParserDriver::error(const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse IRC message: \"%s\"...\n"),
             message_in.c_str()));

//   std::clog << message_in << std::endl;
}

void
RPG_Net_Protocol_IRCParserDriver::reset()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::reset"));

  ACE_OS::memset(&myMessage,
                 0,
                 sizeof(RPG_Net_Protocol_IRCMessage));
}
