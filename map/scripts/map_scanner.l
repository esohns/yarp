%{
#include "stdafx.h"

#include <string>

#include <ace/Log_Msg.h>
#include <ace/OS_Memory.h>

#include "rpg_map_parser.h"
#include "rpg_map_parser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return yy::RPG_Map_Parser::token::END
%}

%option pointer 8bit reentrant never-interactive nounistd nodefault
/* %option c++ yyclass="RPG_Map_Scanner" stack noline bison-bridge bison-locations */
%option warn batch nomain noyywrap nounput extra-type="RPG_Map_ParserDriver*"
%option debug perf-report verbose yylineno ansi-definitions ansi-prototypes
%option full backup perf-report verbose
%option outfile="rpg_map_scanner.cpp" header-file="rpg_map_scanner.h" prefix="RPG_Map_Scanner_"

/* currently supported glyphs:
' ': unmapped
'.': floor
'#': wall
'=': door
'@': (room) seed
'X': starting position
*/
UNMAPPED       [ ]
FLOOR          [.]
WALL           [#]
DOOR           [=]
SEED           [@]
START          [X]
GLYPH          {UNMAPPED}|{FLOOR}|{WALL}|{DOOR}|{SEED}|{START}

%s             row
%s             last_row

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%
%{
  yylloc->step();
  yy_flex_debug = driver->getDebugScanner();
%}

<INITIAL>{
{GLYPH}+/\n                { BEGIN(row);
                             yyless(0); }
{GLYPH}*                   { BEGIN(last_row);
                             yyless(0); }
\n                         { yylloc->step();
                             (*line_count)++; }
} // end <INITIAL>
<row>{
{GLYPH}                    { yylloc->step();
                             yylval->cval = yytext[0];
                             return yy::RPG_Map_Parser::token::GLYPH; }
\n                         { yylloc->step();
                             BEGIN(INITIAL);
                             (*line_count)++;
                             return yy::RPG_Map_Parser::token::END_OF_ROW; }
} // end <row>
<last_row>{
{GLYPH}                    { yylloc->step();
                             yylval->cval = yytext[0];
                             return yy::RPG_Map_Parser::token::GLYPH; }
} // end <last_row>
<<EOF>>                    { BEGIN(INITIAL);
                             yyterminate(); }
<*>(?s:.)                  { yylloc->step();
                             yylval->cval = yytext[0];

                             // debug info
                             std::string error_message = ACE_TEXT("invalid character: \"");
                             error_message += yytext[0];
                             error_message += ACE_TEXT("\", aborting");
                             driver->error(*yylloc, error_message);

                             yyterminate();
                           }
%%
