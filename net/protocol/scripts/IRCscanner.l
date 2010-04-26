%{
#include <stdlib.h>

#include <ace/Log_Msg.h>

#include "rpg_net_protocol_IRCparser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCscanner.cpp" prefix="IRCScanner" */
%option warn nodefault noyywrap nounput
%option debug perf-report verbose
%option reentrant extra-type="unsigned long*"
/* %option bison-bridge bison-locations stack */
%option prefix="IRCScanner" yylineno

/* *TODO*: "see RFC 952 [DNS:4] for details on allowed hostnames..."
           --> [[:alpha:][:digit:].]+ is just an (uninformed) suggestion */

SPACE          [ ]+
HOST           [[:alpha:][:digit:].]+
SERVERNAME     {HOST}
/* LETTER         [:alpha:] <-- for some ODD reason, this matches ":" */
LETTER         [a-zA-Z]
NUMBER         [:digit:]
SPECIAL        [-\[\]\\`^{}]
NICK           {LETTER}[{LETTER}{NUMBER}{SPECIAL}]*
ORIGIN         {SERVERNAME}|{NICK}
NONWHITE       ([\x00-\xFF]{-}[ \t\0\r\n])+
USER           {NONWHITE}
CHSTRING       ([\x00-\xFF]{-}[ \b\0\r\n,])+
MASK           [#$]{CHSTRING}
CHANNEL        [#&]{CHSTRING}
PARAM          ([\x00-\xFF]{-}[: \0\r\n])([\x00-\xFF]{-}[ \0\r\n])*
TRAILING_PARAM ([\x00-\xFF]{-}[\0\r\n])*
CRLF           \r\n

%s             prefix
%s             user
%s             host
%s             trailing
%s             params

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%%
%{
  yylloc->step();
  typedef yy::RPG_Net_Protocol_IRCParser::token token;
  typedef yy::RPG_Net_Protocol_IRCParser::token_type token_type;
  yy_flex_debug=driver.getTraceScanning();
%}

<INITIAL>":"               { yylloc->step();
                             BEGIN(prefix);
                             return token_type(yytext[0]); }
<prefix>{
{SPACE}                    { yylloc->step();
                             BEGIN(INITIAL);
                             yylval->ival = yyleng;
                             return token::SPACE; }
"!"                        { yylloc->step();
                             BEGIN(user);
                             return token_type(yytext[0]); }
"@"                        { yylloc->step();
                             BEGIN(host);
                             return token_type(yytext[0]); }
{ORIGIN}                   { yylloc->step();
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::ORIGIN; }
} // end <prefix>
<user>{USER}               { yylloc->step();
                             BEGIN(prefix);
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::USER; }
<host>{HOST}               { yylloc->step();
                             BEGIN(prefix);
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::HOST; }
<INITIAL>{LETTER}+         { yylloc->step();
                             BEGIN(params);
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::CMD_STRING; }
<INITIAL>{NUMBER}{3}       { yylloc->step();
                             BEGIN(params);
                             yylval->ival = ::atoi(yytext);
                             return token::CMD_NUMERIC; }
<params>{
{SPACE}                    { yylloc->step();
                             yylval->ival = yyleng;
                             return token::SPACE; }
":"                        { yylloc->step();
                             BEGIN(trailing);
                             return token_type(yytext[0]); }
{PARAM}                    { yylloc->step();
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::PARAM; }
} // end <params>
<trailing>{TRAILING_PARAM} { yylloc->step();
                             BEGIN(INITIAL);
                             yylval->ival = yyleng;
                             yylval->sval = yytext;
                             return token::PARAM; }
<INITIAL>{CRLF}            { (*yyextra)++;
                             yylloc->step(); }
<<EOF>>                    { yylloc->step();
                             yyterminate(); }
(?s:.)                     { driver.error(*yylloc, ACE_TEXT("invalid character"));
                             // debug info
                             ACE_DEBUG((LM_ERROR,
                                        ACE_TEXT("invalid character: \"%c\", aborting\n"),
                                        yytext[0]));

                             return token_type(yytext[0]); }
%%
