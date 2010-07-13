%{
#include <stdlib.h>

#include <string>
#include <sstream>

#include <ace/OS_Memory.h>
#include <ace/Log_Msg.h>

#include "rpg_map_parser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_map_scanner.cpp" prefix="MapScanner" */
%option warn nodefault nounput noyywrap
%option debug perf-report verbose
/*%option reentrant extra-type="unsigned long*"*/
%option reentrant extra-type="RPG_Map_ParserDriver*"
/* %option bison-bridge bison-locations stack */
%option prefix="MapScanner" yylineno

/* currently supported glyphs:
' ': unmapped
'.': floor
'#': wall
'=': door
'@': (room) seed
*/
UNMAPPED       [ ]
FLOOR          [.]
WALL           [#]
DOOR           [=]
SEED           [@]
GLYPH          {UNMAPPED}|{FLOOR}|{WALL}|{DOOR}|{SEED}
LF             \n
CRLF           \r\n
END_OF_LINE    {LF}|{CRLF}

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%%
%{
  yylloc->step();
  typedef yy::RPG_Map_Parser::token token;
  typedef yy::RPG_Map_Parser::token_type token_type;
  yy_flex_debug = driver.getTraceScanning();
  std::ostringstream converter;
%}

<INITIAL>{GLYPH}           { yylloc->step();
                             yylval->val = yytext[0];
                             return token::GLYPH; }
<INITIAL>{END_OF_LINE}     { yylloc->step();
                             line_count++;
                             return token::END_OF_ROW; }
<<EOF>>                    { yylloc->step();
                             yyterminate(); }
<*>(?s:.)                  { yylloc->step();
                             //yylval->val = yytext[0];

                             // debug info
                             std::string error_message(ACE_TEXT("invalid character (was \""));
                             error_message += yytext[0];
                             converter.str(ACE_TEXT(""));
                             converter.clear();
                             converter << ACE_static_cast(unsigned long, yytext[0]);
                             error_message += ACE_TEXT("\" - ");
                             error_message += converter.str();
                             error_message += ACE_TEXT("), continuing");
                             driver.error(*yylloc, error_message);

                             return token_type(yytext[0]);
                           }
%%
