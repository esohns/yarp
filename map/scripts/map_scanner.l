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
/* %option c++ outfile="rpg_map_scanner.cpp" prefix="MapScanner" nounput */
%option warn nodefault batch nomain noyywrap
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
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%%
%{
  yylloc->step();
  typedef yy::RPG_Map_Parser::token token;
  typedef yy::RPG_Map_Parser::token_type token_type;
  yy_flex_debug = driver.getDebugScanner();
%}

<INITIAL>{
{GLYPH}+/\n                { BEGIN(row);
                             yyless(0); }
{GLYPH}                    { BEGIN(last_row);
                             unput(yytext[0]); }
\n                         { yylloc->step();
                             yylval->val = '\n';
                             line_count++; }
} // end <INITIAL>
<row>{
{GLYPH}                    { yylloc->step();
                             yylval->val = yytext[0];
                             return token::GLYPH; }
\n                         { yylloc->step();
                             BEGIN(INITIAL);
                             yylval->val = '\n';
                             line_count++;
                             return token::END_OF_ROW; }
} // end <row>
<last_row>{
{GLYPH}                    { yylloc->step();
                             yylval->val = yytext[0];
                             return token::GLYPH; }
<<EOF>>                    { BEGIN(row);
                             unput('\n'); }
} // end <last_row>
<<EOF>>                    { yyterminate(); }
<*>(?s:.)                  { yylloc->step();
                             yylval->val = yytext[0];

                             ACE_DEBUG((LM_DEBUG,
                                        ACE_TEXT("unknown/invalid glyph (was: \"%c\"), continuing\n"),
                                        yytext[0]));

                             return token_type(yytext[0]);
                           }
%%
