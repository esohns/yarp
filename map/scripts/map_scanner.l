%{
// #include <stdlib.h>

#include <string>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include "rpg_map_parser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return yy::RPG_Map_Parser::token::END
%}

/* %option pointer full nounistd 8bit */
%option pointer 8bit c++ stack never-interactive yyclass="RPG_Map_Scanner"
/* %option c++ outfile="rpg_map_scanner.cpp" prefix="MapScanner" */
%option warn nodefault batch nomain noyywrap nounput
%option debug perf-report verbose yylineno
/* %option reentrant extra-type="RPG_Map_ParserDriver*" */
/* %option bison-bridge bison-locations stack */

NAME          [[:print:]]
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

%s             name
%s             row
%s             last_row

%{
#define YY_USER_ACTION myLocation->columns(yyleng);
%}

%%
%{
  myLocation->step();
  //typedef yy::RPG_Map_Parser::token token;
  //typedef yy::RPG_Map_Parser::token_type token_type;
  //yy_flex_debug = driver.getDebugScanner();
%}

<INITIAL>{
"["{NAME}*/"]\n"           { BEGIN(name);
                             yyless(0); }
{GLYPH}+/\n                { BEGIN(row);
                             yyless(0); }
{GLYPH}*                   { BEGIN(last_row);
                             yyless(0); }
\n                         { myLocation->step();
                             /* line_count++; */ }
} // end <INITIAL>
<name>{
"["{NAME}*/"]"             { myLocation->step();
                             ACE_NEW_NORETURN(myToken->sval,
                                              std::string);
                             myToken->sval->append(yytext, yyleng);
                             myToken->sval->erase(0, 1);
                             return yy::RPG_Map_Parser::token::NAME; }
"]\n"                      { myLocation->step();
                             BEGIN(row);
                             /* line_count++; */ }
} // end <name>
<row>{
{GLYPH}                    { myLocation->step();
                             myToken->cval = yytext[0];
                             return yy::RPG_Map_Parser::token::GLYPH; }
\n                         { myLocation->step();
                             BEGIN(INITIAL);
                             /* line_count++; */
                             return yy::RPG_Map_Parser::token::END_OF_ROW; }
} // end <row>
<last_row>{
{GLYPH}                    { myLocation->step();
                             myToken->cval = yytext[0];
                             return yy::RPG_Map_Parser::token::GLYPH; }
} // end <last_row>
<<EOF>>                    { BEGIN(INITIAL);
                             yyterminate(); }
.                          { myLocation->step();
                             myToken->cval = yytext[0];

                             // debug info
                             std::string error_message = ACE_TEXT("invalid character: \"");
                             error_message += yytext[0];
                             error_message += ACE_TEXT("\", aborting");
                             myDriver->error(*myLocation, error_message);

                             yyterminate();
                           }
%%
