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

#ifndef RPG_NET_PROTOCOL_IRCSCANNER_H
#define RPG_NET_PROTOCOL_IRCSCANNER_H

#include "rpg_net_protocol_IRCparser.h"

#include <FlexLexer.h>

#include <ace/Global_Macros.h>

#include <string>

// forward declarations
class RPG_Net_Protocol_IRCParserDriver;

class RPG_Net_Protocol_IRCScanner
 : public yyFlexLexer
{
 public:
  RPG_Net_Protocol_IRCScanner();
  virtual ~RPG_Net_Protocol_IRCScanner();

  virtual int yylex();

  void set(yy::RPG_Net_Protocol_IRCParser::semantic_type*, // YYSTYPE
           yy::RPG_Net_Protocol_IRCParser::location_type*, // location
		   RPG_Net_Protocol_IRCParserDriver*,              // driver
		   unsigned long*,                                 // message count
		   std::string*);                                  // memory

 private:
  typedef yyFlexLexer inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCScanner());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCScanner(const RPG_Net_Protocol_IRCScanner&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_IRCScanner& operator=(const RPG_Net_Protocol_IRCScanner&));

  yy::RPG_Net_Protocol_IRCParser::semantic_type* myToken;
  yy::RPG_Net_Protocol_IRCParser::location_type* myLocation;
  RPG_Net_Protocol_IRCParserDriver*              myDriver;
  unsigned long*                                 myMessageCount;
  std::string*                                   myMemory;
  bool								             myIsInitialized;
};
#endif
