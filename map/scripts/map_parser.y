%require          "2.4.1"
%debug
%language         "c++"
%locations
/*%name-prefix      "RPG_Map_Scanner_"*/
%define api.prefix {RPG_Map_Scanner_}
%no-lines
%skeleton         "lalr1.cc"
/* %skeleton         "glr.c" */
%token-table
%code top {
#include "stdafx.h"
}
%defines          "rpg_map_parser.h"
%output           "rpg_map_parser.cpp"
/* %define           api.pure */
/* %define           api.push_pull */
/* %define           parse.lac full */
/*%define namespace "yy"*/
%define api.namespace {yy}
/*%error-verbose*/
%define parse.error verbose
/*%define parser_class_name "RPG_Map_Parser"*/
%define api.parser.class {RPG_Map_Parser}

%code requires {
class RPG_Map_ParserDriver;
typedef void* yyscan_t;
}

// parsing context
%parse-param { RPG_Map_ParserDriver* driver }
%parse-param { unsigned int* line_count }
%parse-param { yyscan_t yyscanner }
%lex-param { RPG_Map_ParserDriver* driver }
%lex-param { unsigned int* line_count }
%lex-param { yyscan_t yyscanner }

%initial-action
{
  // Initialize the initial location
  @$.begin.filename = @$.end.filename = &driver->myCurrentFilename;

  // initialize the token value container
  $$.cval = 0;
}

// symbols
%union
{
  char cval;
}

%code {
#include "rpg_map_common.h"
#include "rpg_map_parser_driver.h"
#include "stack.hh"

#include "rpg_common_macros.h"

#include "ace/Log_Msg.h"

#include <string>
}

%token <cval> GLYPH      "glyph"
%token <cval> END_OF_ROW "end_of_row"
%token END 0             "end_of_file"

%printer    { debug_stream() << $$; } <cval>
%destructor { $$ = 0; }               <cval>

%%
%start map;
%nonassoc END END_OF_ROW;
%left GLYPH;

map:    glyphs "end_of_file"      /* default */
glyphs: %empty                    /* empty */
        | glyphs "glyph"          { switch ($2)
                                    {
                                      case ' ':
                                      {
                                        driver->myCurrentPlan->unmapped.insert (driver->myCurrentPosition);
                                        break;
                                      }
                                      case '.':
                                      {
                                        break;
                                      }
                                      case '#':
                                      {
                                        driver->myCurrentPlan->walls.insert (driver->myCurrentPosition);
                                        break;
                                      }
                                      case '=':
                                      {
                                        struct RPG_Map_Door door;
                                        door.position = driver->myCurrentPosition;
                                        door.outside = RPG_MAP_DIRECTION_INVALID;
                                        door.state = RPG_MAP_DOORSTATE_INVALID;
                                        driver->myCurrentPlan->doors.insert (door);
                                        break;
                                      }
                                      case '@':
                                      {
                                        driver->myCurrentSeedPoints->insert (driver->myCurrentPosition);
                                        break;
                                      }
                                      case 'X':
                                      {
                                        *(driver->myCurrentStartPosition) = driver->myCurrentPosition;
                                        break;
                                      }
                                      default:
                                      {
                                        ACE_DEBUG((LM_ERROR,
                                                   ACE_TEXT("invalid/unknown glyph: \"%c\", continuing\n"),
                                                   $2));
                                        break;
                                      }
                                    } // end SWITCH
                                    driver->myCurrentPosition.first++;
                                  };
        | glyphs "end_of_row"     { ACE_UNUSED_ARG ($2);
                                    if (driver->myCurrentSizeX == 0)
                                      driver->myCurrentSizeX = driver->myCurrentPosition.first;
                                    driver->myCurrentPosition.first = 0;
                                    driver->myCurrentPosition.second++;
                                  };
%%

void
yy::RPG_Map_Parser::error(const location_type& location_in,
                          const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("yy::RPG_Map_Parser::error"));

  ACE_ASSERT(driver);

  driver->error(location_in, message_in);
}
