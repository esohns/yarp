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

#ifndef RPG_MAP_PARSER_DRIVER_H
#define RPG_MAP_PARSER_DRIVER_H

#include "rpg_map_defines.h"
#include "rpg_map_common.h"
#include "rpg_map_parser.h"

#include <ace/Global_Macros.h>

#include <string>

typedef struct yy_buffer_state* YY_BUFFER_STATE;

// tell flex the lexer's prototype ...
#define YY_DECL                                     \
  yy::RPG_Map_Parser::token_type                    \
  yylex(yy::RPG_Map_Parser::semantic_type* yylval,  \
        yy::RPG_Map_Parser::location_type* yylloc,  \
        RPG_Map_ParserDriver& driver,               \
        unsigned long& line_count,                  \
        yyscan_t& yyscanner)
// ... and declare it for the parser's sake
YY_DECL;

class RPG_Map_ParserDriver
{
  // allow access to our internals (i.e. the current plan, seed points)
  friend class yy::RPG_Map_Parser;
  // allow access to our internals (i.e. error reporting)
//   friend class RPG_Map_Scanner;

 public:
  RPG_Map_ParserDriver(const bool& = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
                       const bool& = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  virtual ~RPG_Map_ParserDriver();

  // target data, needs to be set PRIOR to invoking parse() !
  void init(std::string*,         // target data: name
            RPG_Map_Position_t*,  // target data: start position
            RPG_Map_Positions_t*, // target data: seed points
            RPG_Map_FloorPlan_t*, // target data: floor plan
            const bool& = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
            const bool& = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  // *WARNING*: the argument needs to have been prepared for usage by flex:
  // --> buffers need two trailing '\0's BEYOND their data
  //    (at positions length() + 1, length() + 2)
  const bool parse(const std::string&); // FQ filename

  // invoked by the scanner ONLY !!!
  const bool getDebugScanner() const;

  // error-handling
  void error(const yy::location&, // location
             const std::string&); // message
  void error(const std::string&); // message

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_ParserDriver());
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_ParserDriver(const RPG_Map_ParserDriver&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_ParserDriver& operator=(const RPG_Map_ParserDriver&));

//   // clear current data
//   void reset();

  // helper methods
  const bool scan_begin(FILE*); // file handle
  void scan_end();

  // scanner
  bool                 myTraceScanning;
  yyscan_t             myScannerContext;
//   MapFlexLexer    myScanner;
  unsigned long        myCurrentNumLines;

  // scan buffer
  YY_BUFFER_STATE      myCurrentBufferState;

  // parser
  yy::RPG_Map_Parser   myParser;
  unsigned long        myCurrentSizeX;
  RPG_Map_Position_t   myCurrentPosition;

  // target data
  RPG_Map_FloorPlan_t* myCurrentPlan;
  RPG_Map_Positions_t* myCurrentSeedPoints;
  RPG_Map_Position_t*  myCurrentStartPosition;
  std::string*         myCurrentName;

  bool                 myIsInitialized;
};

#endif
