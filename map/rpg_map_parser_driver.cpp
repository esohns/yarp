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

#include "rpg_map_parser_driver.h"

#include "rpg_map_common_tools.h"

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <fstream>
#include <sstream>

RPG_Map_ParserDriver::RPG_Map_ParserDriver(const bool& traceScanning_in,
                                           const bool& traceParsing_in)
 : myTraceScanning(traceScanning_in),
   //myScanner(),
   myCurrentNumLines(0),
   myCurrentBufferState(NULL),
   myParser(this,       // driver
            myScanner), // scanner
   myCurrentSizeX(0),
   myCurrentPosition(std::make_pair(0, 0)),
   myCurrentPlan(NULL),
   myCurrentSeedPoints(NULL),
   myCurrentStartPosition(NULL),
   myCurrentName(NULL),
//   myCurrentFilename(),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::RPG_Map_ParserDriver"));

  // trace ?
  myScanner.set_debug((traceScanning_in ? 1 : 0));
  myParser.set_debug_level((traceParsing_in ? 1 : 0)); // binary (see bison manual)
}

RPG_Map_ParserDriver::~RPG_Map_ParserDriver ()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::~RPG_Map_ParserDriver"));

}

void
RPG_Map_ParserDriver::init(std::string* name_in,
                           RPG_Map_Position_t* startPosition_in,
                           RPG_Map_Positions_t* seedPoints_in,
                           RPG_Map_FloorPlan_t* plan_in,
                           const bool& traceScanning_in,
                           const bool& traceParsing_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::init"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);
  ACE_ASSERT(name_in);
  ACE_ASSERT(startPosition_in);
  ACE_ASSERT(seedPoints_in);
  ACE_ASSERT(plan_in);

  // (re-)init scanner/parser state
  myCurrentNumLines = 0;
  if (myCurrentBufferState)
    scan_end();
  myCurrentPosition = std::make_pair(0, 0);
  myCurrentSizeX = 0;

  // set parse target data
  myCurrentName = name_in;
  myCurrentStartPosition = startPosition_in;
  myCurrentSeedPoints = seedPoints_in;
  myCurrentPlan = plan_in;
  // init target structures
  myCurrentName->clear();
  *myCurrentStartPosition = std::make_pair(0, 0);
  myCurrentSeedPoints->clear();
  myCurrentPlan->size_x = 0;
  myCurrentPlan->size_y = 0;
  myCurrentPlan->unmapped.clear();
  myCurrentPlan->walls.clear();
  myCurrentPlan->doors.clear();

  // trace ?
  myScanner.set_debug((traceScanning_in ? 1 : 0));
  myParser.set_debug_level((traceParsing_in ? 1 : 0)); // binary (see bison manual)

  // OK
  myIsInitialized = true;
}

const bool
RPG_Map_ParserDriver::parse(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::parse"));

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // open file
  std::ifstream file;
  file.open(filename_in.c_str(), std::ios_base::in);
  if (file.fail())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open file \"%s\", aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // init scan buffer
  if (!scan_begin(&file))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse map file \"%s\", aborting\n"),
               filename_in.c_str()));

    // clean up
    file.close();
    if (file.fail())
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file \"%s\", aborting\n"),
                 filename_in.c_str()));

    return false;
  } // end IF

  // parse file
  myCurrentFilename = filename_in;
  int result = -1;
  result = myParser.parse();
  if (result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse map file \"%s\", aborting\n"),
               filename_in.c_str()));

    // clean up
    scan_end();
    file.close();
    if (file.fail())
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file \"%s\", aborting\n"),
                 filename_in.c_str()));

    return false;
  } // end IF

  // set dimensions
  myCurrentPlan->size_x = myCurrentSizeX;
  myCurrentPlan->size_y = myCurrentPosition.second;

  // fini buffer/scanner
  scan_end();

  // clean up
  myCurrentFilename.clear();
  file.close();
  if (file.fail())
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to close file \"%s\", aborting\n"),
               filename_in.c_str()));

//   if (myParser.debug_level())
//     RPG_Map_Common_Tools::displayFloorPlan(*myCurrentPlan);

  // reset state
  myIsInitialized = false;

  return true;
}

const bool
RPG_Map_ParserDriver::getDebugScanner() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::getDebugScanner"));

  return myTraceScanning;
}

void
RPG_Map_ParserDriver::error(const yy::location& location_in,
                            const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::error"));

  std::ostringstream converter;
  converter << location_in;
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse file (line: %u, @%s): \"%s\"\n"),
             myCurrentNumLines,
             converter.str().c_str(),
             message_in.c_str()));
}

void
RPG_Map_ParserDriver::error(const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::error"));

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse file (line: %u): \"%s\"...\n"),
             myCurrentNumLines,
             message_in.c_str()));
}

const bool
RPG_Map_ParserDriver::scan_begin(std::istream* file_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(file_in);
  ACE_ASSERT(myCurrentBufferState == NULL);

  myCurrentBufferState = myScanner.yy_create_buffer(file_in,
                                                    RPG_MAP_SCANNER_BUFSIZE);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to yy_create_buffer(%@, %d), aborting\n"),
               file_in,
               RPG_MAP_SCANNER_BUFSIZE));

    return false;
  } // end IF
  myScanner.yy_switch_to_buffer(myCurrentBufferState);

  return true;
}

void
RPG_Map_ParserDriver::scan_end()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState);

  // clean state
  myScanner.yy_delete_buffer(myCurrentBufferState);
  myCurrentBufferState = NULL;
}

int
yylex(yy::RPG_Map_Parser::semantic_type* token_in,
      yy::RPG_Map_Parser::location_type* location_in,
      RPG_Map_ParserDriver* driver_in,
      RPG_Map_Scanner& scanner_in)
{
  RPG_TRACE(ACE_TEXT("::yylex"));

  scanner_in.set(token_in, location_in, driver_in);

  return scanner_in.yylex();
}
