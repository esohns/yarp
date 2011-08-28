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

#include "rpg_map_scanner.h"
#include "rpg_map_common_tools.h"

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>

#include <sstream>

RPG_Map_ParserDriver::RPG_Map_ParserDriver(const bool& traceScanning_in,
                                           const bool& traceParsing_in)
 : myTraceScanning(traceScanning_in),
   myScannerContext(NULL),
   myCurrentNumLines(0),
   myCurrentBufferState(NULL),
   myParser(*this,             // driver
            myCurrentNumLines, // counter
            myScannerContext), // scanner context
   myCurrentSizeX(0),
   myCurrentPosition(std::make_pair(0, 0)),
   myCurrentPlan(NULL),
   myCurrentSeedPoints(NULL),
   myCurrentStartPosition(NULL),
   myCurrentName(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::RPG_Map_ParserDriver"));

  // init scanner context
  if (MapScannerlex_init_extra(this,               // extra data
                               &myScannerContext)) // scanner context handle
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to MapScannerlex_init_extra(): \"%m\", continuing\n")));

  // trace ?
  myParser.set_debug_level((traceParsing_in ? 1 : 0)); // binary (see bison manual)
}

RPG_Map_ParserDriver::~RPG_Map_ParserDriver ()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::~RPG_Map_ParserDriver"));

  // fini scanner context
  if (myScannerContext)
    MapScannerlex_destroy(myScannerContext);
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
  myTraceScanning = traceScanning_in;
  if (traceParsing_in)
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
  FILE* fp = NULL;
  fp = ACE_OS::fopen(filename_in.c_str(),
                     ACE_TEXT_ALWAYS_CHAR("rb"));
  if (!fp)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // init scan buffer
  if (!scan_begin(fp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse IRC message fragment, aborting\n")));

    // clean up
    if (ACE_OS::fclose(fp))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, aborting\n"),
                 filename_in.c_str()));

    return false;
  } // end IF

  // parse file
  int result = -1;
  result = myParser.parse();
  if (result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse map file, aborting\n")));

    // clean up
    scan_end();
    if (ACE_OS::fclose(fp))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, aborting\n"),
                 filename_in.c_str()));

    return false;
  } // end IF

  // set dimensions
  myCurrentPlan->size_x = myCurrentSizeX;
  myCurrentPlan->size_y = myCurrentPosition.second;

  // fini buffer/scanner
  scan_end();

  // clean up
  if (ACE_OS::fclose(fp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to close file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

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
RPG_Map_ParserDriver::scan_begin(FILE* file_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(file_in);
  ACE_ASSERT(myCurrentBufferState == NULL);

  myCurrentBufferState = MapScanner_create_buffer(file_in,
                                                  YY_BUF_SIZE,
                                                  myScannerContext);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::MapScanner_create_buffer(%@, %d, %@), aborting\n"),
               file_in,
               YY_BUF_SIZE,
               myScannerContext));

    return false;
  } // end IF
  MapScanner_switch_to_buffer(myCurrentBufferState,
                              myScannerContext);

  return true;
}

void
RPG_Map_ParserDriver::scan_end()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_ParserDriver::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState);

  // clean state
  MapScanner_delete_buffer(myCurrentBufferState,
                           myScannerContext);
  myCurrentBufferState = NULL;
}
