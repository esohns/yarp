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

#ifndef RPG_NET_PROTOCOL_IRCPARSER_DRIVER_H
#define RPG_NET_PROTOCOL_IRCPARSER_DRIVER_H

#include "rpg_net_protocol_IRCparser.h"
#include "rpg_net_protocol_common.h"

#include <ace/Global_Macros.h>

#include <string>

// Tell Flex the lexer's prototype ...
#define YY_DECL                                   \
  IRCParse::RPG_Net_Protocol_IRCParser::token_type                    \
  yylex(IRCParse::RPG_Net_Protocol_IRCParser::semantic_type* yylval,  \
        IRCParse::RPG_Net_Protocol_IRCParser::location_type* yylloc,  \
        RPG_Net_Protocol_IRCParserDriver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

// forward declaration(s)
class ACE_Message_Block;

class RPG_Net_Protocol_IRCParserDriver
{
  // allow access to our internals (i.e. the current message)
  friend class IRCParse::RPG_Net_Protocol_IRCParser;

 public:
  RPG_Net_Protocol_IRCParserDriver(const bool& = false,  // trace scanning ?
                                   const bool& = false); // trace parsing ?
  virtual ~RPG_Net_Protocol_IRCParserDriver();

  const bool parse(const ACE_Message_Block*); // data
  const RPG_Net_Protocol_IRCMessage getIRCMessage() const;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver(const RPG_Net_Protocol_IRCParserDriver&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver& operator=(const RPG_Net_Protocol_IRCParserDriver&));

  // helper methods
  void scan_begin(const char*,    // base address
                  const size_t&); // length of data block
  void scan_end();

  // error-handling
  void error(const IRCParse::location&, // location
             const std::string&);       // message
  void error(const std::string&);       // message

  // clear current message
  void reset();

  bool                                 myTraceScanning;
  bool                                 myTraceParsing;
  YY_BUFFER_STATE                      myCurrentState;
  IRCParse::RPG_Net_Protocol_IRCParser myParser;
  RPG_Net_Protocol_IRCMessage          myCurrentMessage;
};

#endif
