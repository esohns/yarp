%{
// #include <stdlib.h>

#include <string>

#include <ace/Log_Msg.h>

#include "rpg_map_parser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_map_scanner.cpp" prefix="MapScanner" */
%option warn nodefault batch nomain noyywrap nounput
%option debug perf-report verbose
/*%option reentrant extra-type="unsigned long*"*/
%option reentrant extra-type="RPG_Map_ParserDriver*"
/* %option bison-bridge bison-locations stack */
%option prefix="MapScanner" yylineno

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
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%
%{
  yylloc->step();
  typedef yy::RPG_Map_Parser::token token;
  typedef yy::RPG_Map_Parser::token_type token_type;
  yy_flex_debug = driver.getDebugScanner();
%}

<INITIAL>{
"["{NAME}*/"]\n"           { BEGIN(name);
                             yyless(0); }
{GLYPH}+/\n                { BEGIN(row);
                             yyless(0); }
{GLYPH}*                   { BEGIN(last_row);
                             yyless(0); }
\n                         { yylloc->step();
                             line_count++; }
} // end <INITIAL>
<name>{
"["{NAME}*/"]"             { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             yylval->sval->append(yytext, yyleng);
                             yylval->sval->erase(0, 1);
                             return token::NAME; }
"]\n"                      { yylloc->step();
                             BEGIN(row);
                             line_count++; }
} // end <name>
<row>{
{GLYPH}                    { yylloc->step();
                             yylval->cval = yytext[0];
                             return token::GLYPH; }
\n                         { yylloc->step();
                             BEGIN(INITIAL);
                             line_count++;
                             return token::END_OF_ROW; }
} // end <row>
<last_row>{
{GLYPH}                    { yylloc->step();
                             yylval->cval = yytext[0];
                             return token::GLYPH; }
} // end <last_row>
<<EOF>>                    { BEGIN(INITIAL);
                             yyterminate(); }
<*>(?s:.)                  { yylloc->step();
                             yylval->cval = yytext[0];

                             // debug info
                             std::string error_message = ACE_TEXT("invalid character: \"");
                             error_message += yytext[0];
                             error_message += ACE_TEXT("\", aborting");
                             driver.error(*yylloc, error_message);

                             yyterminate();
                           }
%%
