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

#ifndef RPG_MAP_SCANNER_H
#define RPG_MAP_SCANNER_H

#include "rpg_map_defines.h"
#include "rpg_map_parser.h"

#include <FlexLexer.h>

#include <ace/Global_Macros.h>

// forward declarations
class RPG_Map_ParserDriver;

class RPG_Map_Scanner
 : public yyFlexLexer
{
 public:
  RPG_Map_Scanner();
  virtual ~RPG_Map_Scanner();

  virtual int yylex();

  void set(yy::RPG_Map_Parser::semantic_type*, // YYSTYPE
           yy::RPG_Map_Parser::location_type*, // location
		   RPG_Map_ParserDriver*);             // driver

 private:
  typedef yyFlexLexer inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Scanner());
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Scanner(const RPG_Map_Scanner&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Scanner& operator=(const RPG_Map_Scanner&));

  yy::RPG_Map_Parser::semantic_type* myToken;
  yy::RPG_Map_Parser::location_type* myLocation;
  RPG_Map_ParserDriver*              myDriver;
  bool								 myIsInitialized;
};
#endif
