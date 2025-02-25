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

#include <iostream>
#include <string>

#include "ace/Global_Macros.h"

#include "rpg_map_common.h"
#include "rpg_map_defines.h"
#include "rpg_map_parser.h"

// forward declaration(s)
typedef void* yyscan_t;
typedef struct yy_buffer_state* YY_BUFFER_STATE;

// tell flex the lexer's prototype ...
#define YY_DECL                                                 \
yy::RPG_Map_Parser::token_type                                  \
RPG_Map_Scanner_lex (yy::RPG_Map_Parser::semantic_type* yylval, \
                     yy::RPG_Map_Parser::location_type* yylloc, \
                     RPG_Map_ParserDriver* driver,              \
                     unsigned int* line_count,                  \
                     yyscan_t yyscanner)
// ... and declare it for the parser's sake
YY_DECL;

class RPG_Map_ParserDriver
{
  // allow access to our internals (i.e. the current plan, seed points)
  friend class yy::RPG_Map_Parser;
  // allow access to our internals (i.e. error reporting)
  //friend class RPG_Map_Scanner;

 public:
  RPG_Map_ParserDriver (bool = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
                        bool = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  virtual ~RPG_Map_ParserDriver ();

  // target data, needs to be set PRIOR to invoking parse() !
  void init (RPG_Map_Position_t*,               // target data: start position
             RPG_Map_Positions_t*,              // target data: seed points
             struct RPG_Map_FloorPlan*,         // target data: floor plan
             bool = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
             bool = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  // *WARNING*: IFF the argument IS then buffer, it needs to have been prepared for
  // usage by flex:
  // --> buffers need two trailing '\0's BEYOND their data
  //    (at positions length() + 1, length() + 2)
  bool parse (const std::string&, // FQ filename OR buffer
              bool = false);      // argument IS the buffer ?

  // invoked by the scanner ONLY !!!
  bool getDebugScanner () const;

  // error-handling
  void error (const yy::RPG_Map_Parser::location_type&, // location
              const std::string&);                      // message
  void error (const std::string&); // message

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_ParserDriver ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_ParserDriver (const RPG_Map_ParserDriver&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_ParserDriver& operator= (const RPG_Map_ParserDriver&))

//   // clear current data
//   void reset();

  // helper methods
  //const bool scan_begin(std::istream*); // file handle
  bool scan_begin (FILE*); // file handle
  bool scan_begin (const std::string&); // buffer
  void scan_end ();

  // context
  bool                      myTraceParsing;
  unsigned int              myCurrentNumLines;

  // scanner
  yyscan_t                  myCurrentScannerState;
  YY_BUFFER_STATE           myCurrentBufferState;

  // target data
  struct RPG_Map_FloorPlan* myCurrentPlan;
  RPG_Map_Positions_t*      myCurrentSeedPoints;
  RPG_Map_Position_t*       myCurrentStartPosition;

  // state
  unsigned int              myCurrentSizeX;
  RPG_Map_Position_t        myCurrentPosition;

  std::string               myCurrentFilename;
  bool                      myIsInitialized;
};

#endif
