%require "2.4.1"
%error-verbose
%define parser_class_name "RPG_Map_Parser"
/* %define api.pure */
/* %locations */
/* %define namespace "" */
/* %name-prefix "MapParse" */

%code requires {
class RPG_Map_ParserDriver;
typedef void* yyscan_t;
}

// parsing context
%parse-param { RPG_Map_ParserDriver& driver }
%parse-param { unsigned long& line_count }
%parse-param { yyscan_t& context }
%lex-param   { RPG_Map_ParserDriver& driver }
%lex-param   { unsigned long& line_count }
%lex-param   { yyscan_t& context }

%initial-action
{
  // Initialize the initial location
  //@$.begin.filename = @$.end.filename = &driver.file;

  // initialize the token value container
  $$.cval = 0;
  $$.sval = NULL;
}

// symbols
%union
{
  char         cval;
  std::string* sval;
}

%code {
#include "rpg_map_common.h"
#include "rpg_map_parser_driver.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <string>
}

%token <sval> NAME       "name"
%token <cval> GLYPH      "glyph"
%token <cval> END_OF_ROW "end_of_row"
%token END 0             "end_of_file"

%printer    { debug_stream() << $$; } <val>
%destructor { $$ = 0; } <val>
%printer    { debug_stream() << *$$; } <sval>
%destructor { delete $$; $$ = NULL; } <sval>

%%
%start map;
%nonassoc NAME END END_OF_ROW;
%left GLYPH;

map:    name glyphs "end_of_file" /* default */
name:   "name"                    { *(driver.myCurrentName) = *$1;
                                  };
glyphs:                           /* empty */
        | glyphs "glyph"          { switch ($2)
                                    {
                                      case ' ':
                                      {
                                        driver.myCurrentPlan->unmapped.insert(driver.myCurrentPosition);
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      case '.':
                                      {
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      case '#':
                                      {
                                        driver.myCurrentPlan->walls.insert(driver.myCurrentPosition);
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      case '=':
                                      {
                                        RPG_Map_Door_t door;
                                        door.position = driver.myCurrentPosition;
                                        door.outside = DIRECTION_INVALID;
                                        door.is_open = false;
                                        door.is_locked = false;
                                        door.is_broken = false;
                                        driver.myCurrentPlan->doors.insert(door);
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      case '@':
                                      {
                                        driver.myCurrentSeedPoints->insert(driver.myCurrentPosition);
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      case 'X':
                                      {
                                        *(driver.myCurrentStartPosition) = driver.myCurrentPosition;
                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                      default:
                                      {
                                        ACE_DEBUG((LM_ERROR,
                                                   ACE_TEXT("invalid/unknown glyph: \"%c\", continuing\n"),
                                                   $2));

                                        driver.myCurrentPosition.first++;
                                        break;
                                      }
                                    } // end SWITCH
                                  };
        | glyphs "end_of_row"  { if (driver.myCurrentSizeX == 0)
                                   driver.myCurrentSizeX = driver.myCurrentPosition.first;
                                 driver.myCurrentPosition.first = 0;
                                 driver.myCurrentPosition.second++;
                               };
%%

void
yy::RPG_Map_Parser::error(const location_type& location_in,
                          const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Parser::error"));

  driver.error(location_in, message_in);
}
