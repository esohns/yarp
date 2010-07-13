%require "2.4.1"
%error-verbose
%define parser_class_name "RPG_Map_Parser"
/* %define api.pure */
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
}

// symbols
%union
{
  char val;
}

%code {
#include "rpg_map_common.h"
#include "rpg_map_parser_driver.h"

#include <ace/Log_Msg.h>

#include <string>
}

%token <val> GLYPH      "glyph"
%token <val> END_OF_ROW "end_of_row"
%token END 0            "end_of_file"

%printer {
debug_stream() << $$;
} <val>

// *NOTE*: because the last row MAY not be terminated with "\n", the parser
// MAY actually need to gobble the whole file before it can start parsing...
// This is reflected by the second rule of the non-terminal "file" which implies
// a shift/reduce conflict. This conflict is mitigated by giving END a lower
// (left-associative) precedence than the other tokens.
/* %expect 1 */

%%
%start file;
%left GLYPH;
%left END_OF_ROW;

file:    chars "end_of_file"     /* default */
chars:   "glyph" chars           {
                                   switch ($1)
                                   {
                                     case ' ':
                                     {
                                       driver.myCurrentPlan->unmapped.insert(driver.myCurrentPosition);
                                       driver.myCurrentPosition.first++;

                                       break;
                                     }
                                     case '.':
                                     {
                                       //driver.myCurrentPlan.floor.insert(driver.myCurrentPosition);
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
                                       driver.myCurrentPlan->doors.insert(driver.myCurrentPosition);
                                       driver.myCurrentPosition.first++;

                                       break;
                                     }
                                     case '@':
                                     {
                                       //driver.myCurrentPlan->floor.insert(driver.myCurrentPosition);
                                       driver.myCurrentSeedPoints->insert(driver.myCurrentPosition);
                                       driver.myCurrentPosition.first++;

                                       break;
                                     }
                                     default:
                                     {
                                       ACE_DEBUG((LM_ERROR,
                                                  ACE_TEXT("invalid/unknown glyph: \"%c\", continuing\n"),
                                                  $1));

                                       driver.myCurrentPosition.first++;

                                       break;
                                     }
                                   } // end SWITCH
                                 };
         | "end_of_row" chars    {
                                   driver.myCurrentPosition.first = 0;
                                   driver.myCurrentPosition.second++;
                                 };
         |                       /* empty */
%%

void
yy::RPG_Map_Parser::error(const location_type& location_in,
                          const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Parser::error"));

  driver.error(location_in, message_in);
}
