%{
#include <stdlib.h>

#include <string>

#include <ace/OS_Memory.h>
#include <ace/Log_Msg.h>

#include "rpg_net_protocol_IRCparser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCscanner.cpp" prefix="IRCScanner" */
%option warn nodefault nounput noyywrap
%option debug perf-report verbose
/*%option reentrant extra-type="unsigned long*"*/
%option reentrant extra-type="RPG_Net_Protocol_IRCParserDriver*"
/* %option bison-bridge bison-locations stack */
%option prefix="IRCScanner" yylineno

/* *TODO*: "see RFC 952 [DNS:4] for details on allowed hostnames..."
           --> [[:alpha:][:digit:].] is just an uninformed suggestion */

SPACE          [ ]
HOST           [[:alpha:][:digit:].]+
SERVERNAME     {HOST}
LETTER         [[:alpha:]]
/*LETTER    ==     [a-zA-Z]*/
NUMBER         [[:digit:]]
/*NUMBER    ==     [0-9]*/
SPECIAL        [-\[\]\\`^{}]
NICK           {LETTER}([{LETTER}{NUMBER}{SPECIAL}])*
ORIGIN         {SERVERNAME}|{NICK}
NONWHITE       [\x00-\xFF]{-}[ \t\0\r\n]
USER           {NONWHITE}+
CHSTRING       ([\x00-\xFF]{-}[ \b\0\r\n,])+
MASK           [#$]{CHSTRING}
CHANNEL        [#&]{CHSTRING}
PARAM          ([\x00-\xFF]{-}[: \0\r\n])([\x00-\xFF]{-}[ \0\r\n])*
TRAILING_PARAM ([\x00-\xFF]{-}[\0\r\n])*
CRLF           \r\n

%s             prefix
%s             user
%s             host
%s             command
%s             params
%s             trailing

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

/* *NOTE*: it seems that the following "exceptions" are allowed:
  - the trailing param does NOT need a ':' prefix IF it doesn't contain <SPACE>s...
*/

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
{SPACE}+                   { yylloc->step();
                             BEGIN(command);
                             yylval->ival = yyleng;
                             return token::SPACE; }
"!"                        { yylloc->step();
                             BEGIN(user);
                             return token_type(yytext[0]); }
"@"                        { yylloc->step();
                             BEGIN(host);
                             return token_type(yytext[0]); }
{ORIGIN}/{SPACE}           { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::ORIGIN; }
{ORIGIN}/"!"               { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::ORIGIN; }
{ORIGIN}/"@"               { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::ORIGIN; }
} // end <prefix>
<user>{
{USER}/{SPACE}             { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::USER; }
{USER}/"@"                 { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::USER; }
} // end <user>
<host>{HOST}/{SPACE}       { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::HOST; }
<INITIAL,command>{
{LETTER}+/{SPACE}          { yylloc->step();
                             BEGIN(params);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::CMD_STRING; }
{NUMBER}{3}/{SPACE}        { yylloc->step();
                             BEGIN(params);
                             yylval->ival = ::atoi(yytext);
                             return token::CMD_NUMERIC; }
} // end <INITIAL, command>
<params>{
{SPACE}+                   { yylloc->step();
                             yylval->ival = yyleng;
                             return token::SPACE; }
":"/{TRAILING_PARAM}       { yylloc->step();
                             BEGIN(trailing);
                             return token_type(yytext[0]); }
{PARAM}/{SPACE}            { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::PARAM; }
{PARAM}/{CRLF}             { yylloc->step();
                             BEGIN(INITIAL);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::PARAM; }
} // end <params>
<trailing>{
{TRAILING_PARAM}/{CRLF}    { yylloc->step();
                             BEGIN(INITIAL);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext, yyleng));
                             return token::PARAM; }
} // end <trailing>
<INITIAL>{CRLF}            { count++;
                             yylloc->step();
                             return token::END; }
<<EOF>>                    { yylloc->step();
                             if (!driver.switchBuffer())
                              yyterminate(); }
<*>(?s:.)                  { driver.error(*yylloc, ACE_TEXT("invalid character"));
                             // debug info
                             ACE_DEBUG((LM_ERROR,
                                        ACE_TEXT("invalid character: \"%c\" (%d), aborting\n"),
                                        yytext[0],
                                        yytext[0]));
                             return token_type(yytext[0]); }
%%

// #ifdef __cplusplus
// extern "C"
// {
// #endif /* __cplusplus */
// int
// IRCScannerwrap(yyscan_t yyscanner)
// {
//   ACE_TRACE(ACE_TEXT("::IRCScannerwrap"));
//
//   struct yyguts_t* yyg = ACE_static_cast(struct yyguts_t*, yyscanner);
//   // sanity check
//   ACE_ASSERT(yyextra);
//
//   return (yyextra->switchBuffer() ? 0 : 1);
// }
// #ifdef __cplusplus
// }
// #endif /* __cplusplus */
