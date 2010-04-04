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

// #include "rps_flb_common_packet_headers.h"
// #include "rps_flb_common_network_tools.h"
// #include "rps_flb_common_protocol_layer.h"
// #include "rps_flb_common_ethernetframeheader.h"
#include "rpg_net_common_tools.h"

#include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/Message_Block.h>

#include <sstream>

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
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("re-initializing...\n")));

    // reset various maps...
    myUnknownPacketTypes2MessageID.clear();
    myIEEE802_3Frames.clear();
    myFailedMessages.clear();

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

  // *TODO*
}

void
RPG_Net_Module_HeaderParser::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::dump_state"));

  std::string line_string;
  std::ostringstream converter;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
//              ACE_TEXT_ALWAYS_CHAR(name())));
}
