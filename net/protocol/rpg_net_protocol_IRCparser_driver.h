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

#include <ace/Global_Macros.h>

#include <string>

// forward declaration(s)
class RPG_Net_Protocol_IRCMessage;
class ACE_Message_Block;
// class IRCParse::RPG_Net_Protocol_IRCParser;
// typedef void* yyscan_t;
typedef struct yy_buffer_state* YY_BUFFER_STATE;

// Tell Flex the lexer's prototype ...
#define YY_DECL                                                       \
  yy::RPG_Net_Protocol_IRCParser::token_type                    \
  yylex(yy::RPG_Net_Protocol_IRCParser::semantic_type* yylval,  \
        yy::RPG_Net_Protocol_IRCParser::location_type* yylloc,  \
        RPG_Net_Protocol_IRCParserDriver& driver,                     \
        yyscan_t& yyscanner)
// ... and declare it for the parser's sake.
YY_DECL;

class RPG_Net_Protocol_IRCParserDriver
{
  // allow access to our internals (i.e. the current message)
  friend class yy::RPG_Net_Protocol_IRCParser;
  // allow access to our internals (i.e. error reporting)
//   friend class RPG_Net_Protocol_IRCScanner;

 public:
  RPG_Net_Protocol_IRCParserDriver(const bool& = false,  // trace scanning ?
                                   const bool& = false); // trace parsing ?
  virtual ~RPG_Net_Protocol_IRCParserDriver();

  // target data, needs to be set PRIOR to invoking parse() !
  void init(RPG_Net_Protocol_IRCMessage&); // target data
  // *WARNING*: the argument needs to have been prepared for usage by flex:
  // --> buffers need two trailing '\0's BEYOND their data
  //    (at positions length() + 1, length() + 2)
  const bool parse(ACE_Message_Block*); // data
//   const RPG_Net_Protocol_IRCMessage getIRCMessage() const;

  // error-handling
  void error(const yy::location&, // location
             const std::string&);       // message
  void error(const std::string&);       // message

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver(const RPG_Net_Protocol_IRCParserDriver&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCParserDriver& operator=(const RPG_Net_Protocol_IRCParserDriver&));

  // helper methods
  const bool scan_begin(char*,          // base address
                        const size_t&); // length of data block
  void scan_end();

//   // clear current message
//   void reset();

  // scanner
  bool                           myTraceScanning;
  yyscan_t                       myScannerContext;
//   IRCBisectFlexLexer    myScanner;
  unsigned long                  myCurrentNumMessages;
  YY_BUFFER_STATE                myCurrentState;

  // parser
  bool                           myTraceParsing;
  yy::RPG_Net_Protocol_IRCParser myParser;

  // target
  RPG_Net_Protocol_IRCMessage*   myCurrentMessage;
  bool                           myIsInitialized;
};

#endif
