%{
#include <string>
#include <stdlib.h>

#include <ace/OS_Memory.h>
#include <ace/Log_Msg.h>

//#include "rpg_net_protocol_IRCparser.h"
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

COLON          ':'
SPACE          [ ]+
HOST           [:alnum:]+
SERVERNAME     {HOST}
LETTER         [:alpha:]
NUMBER         [:digit:]
SPECIAL        [-\[\]\\`^{}]
NICK           {LETTER}({LETTER}|{NUMBER}|{SPECIAL})*
EXCLAMATION    '!'
NONWHITE       [(?s:.)]{-}[ \t\0\r\n]
USER           {NONWHITE}+
AT             '@'
CHSTRING       ([(?s:.)]{-}[ \b\0\r\n,])+
MASK           [#$]{CHSTRING}
CHANNEL        [#&]{CHSTRING}
PARAM          ([(?s:.)]{-}[ \0\r\n])+
TRAILING_PARAM ([(?s:.)]{-}[\0\r\n])*
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
%}

<INITIAL>{COLON}           { yylloc->step();
                             BEGIN(prefix);
                             return token_type(yytext[0]); }
<prefix>{SERVERNAME}       { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::ORIGIN; }
<prefix>{NICK}             { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::ORIGIN; }
<prefix>{SPACE}            { yylloc->step();
                             yylval->ival = yyleng;
                             BEGIN(INITIAL);
                             return token::SPACE; }
<prefix>{EXCLAMATION}      { yylloc->step();
                             BEGIN(user);
                             return token_type(yytext[0]); }
<user>{USER}               { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::USER; }
<prefix>{AT}               { yylloc->step();
                             BEGIN(host);
                             return token_type(yytext[0]); }
<host>{HOST}               { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::HOST; }
{LETTER}+                  { yylloc->step();
                             BEGIN(params);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::CMD_STRING; }
{NUMBER}{3}                { yylloc->step();
                             BEGIN(params);
                             yylval->ival = ::atoi(yytext);
                             return token::CMD_NUMERIC; }
<params>{SPACE}            { yylloc->step();
                             yylval->ival = yyleng;
                             return token::SPACE; }
<params>{COLON}            { yylloc->step();
                             BEGIN(trailing);
                             return token_type(yytext[0]); }
<trailing>{TRAILING_PARAM} { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::PARAM; }
<params>{PARAM}            { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string(yytext));
                             return token::PARAM; }
{CRLF}                     { yylloc->step();
                             BEGIN(INITIAL); }
<<EOF>>                    { yylloc->step();
                             return token::END; }
(?s:.)                     driver.error(*yylloc, ACE_TEXT("invalid character"));
%%

const bool
RPG_Net_Protocol_IRCParserDriver::scan_begin(char* data_in,
                                             const size_t& length_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState == NULL);

//  yy_flex_debug = myTraceScanning;

  // create/init a new buffer state
  // *WARNING*: length_in IS already adjusted for two trailing \0's
    myCurrentState = IRCScanner_scan_buffer(data_in,
                                            length_in,
                                            myScannerContext);
  if (myCurrentState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::IRCScanner_scan_buffer(%@, %d), aborting\n"),
               data_in,
               (length_in)));

    // what else can we do ?
    return false;
  } // end IF

//   if (file == "-")
//     yyin = stdin;
//   else if (!(yyin = fopen(file.c_str (), "r")))
//   {
//     error(std::string("cannot open ") + file);
//     exit(1);
//   }

  return true;
}

void
RPG_Net_Protocol_IRCParserDriver::scan_end()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState);

  // clean state
  IRCScanner_delete_buffer(myCurrentState,
                           myScannerContext);
  myCurrentState = NULL;

//  fclose(yyin);
}
