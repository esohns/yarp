/* *NOTE*: this prototype deals with some backing-up issues when using
// linked message buffers in conjunction with yy_scan_bytes/switch_buffer logic.
// This whole approach aims to avoid the overhead of "crunching" the buffers forming
// a message frame by dynamically switching buffers on EOF and dealing with
// backing-up issues when the scanner is "reset" in this way. The main issue stems
// from the fact that the remaining content of the input buffer needs to be stored
// into short-term "memory", and this state needs to be restored on the next
// matched token, which is non-trivial, as simply unput()ting the memory will NOT
// work ("flex scanner push-back overflow").
// --> WORK - IN - PROGRESS */

%{
#include <stdlib.h>

#include <string>
#include <sstream>

#include <ace/OS_Memory.h>
#include <ace/Log_Msg.h>

#include "rpg_net_protocol_IRCparser_driver.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCscanner.cpp" prefix="IRCScanner" noyywrap nounput */
%option warn nodefault batch nomain noyywrap
%option debug perf-report verbose
/*%option reentrant extra-type="unsigned long*"*/
%option reentrant extra-type="RPG_Net_Protocol_IRCParserDriver*"
/* %option bison-bridge bison-locations stack */
%option prefix="IRCScanner" yylineno

/* *NOTE*: "see RFC 952 [DNS:4] for details on allowed hostnames..."
RFC 952
1. A "name" (Net, Host, Gateway, or Domain name) is a text string up
   to 24 characters drawn from the alphabet (A-Z), digits (0-9), minus
   sign (-), and period (.).  Note that periods are only allowed when
   they serve to delimit components of "domain style names".
RFC 1123
The syntax of a legal Internet host name was specified in RFC-952.
One aspect of host name syntax is hereby changed: the restriction on
the first character is relaxed to allow either a letter or a digit.

And so on...
--> [[:alpha:][:digit:].-] is just a somewhat informed and tolerant suggestion.
    A more CORRECT implementation would probably look something like this...
<hostname>    = <label> | <hostname>.<label>
<label>       = <let-dig> | <let-dig><let-dig> | <let-dig><ldh-string><let-dig>
<ldh-string>  = <let-dig-hyp> | <ldh-string><let-dig-hyp>
<let-dig-hyp> = <let-dig> | '-'
<let-dig>     = <letter> | <digit>
<digit>       = [[:digit:]]
<letter>      = [[:alpha:]]*/

SPACE          [ ]
HOST           [[:alpha:][:digit:].-]+
SERVERNAME     {HOST}
LETTER         [[:alpha:]]
/*LETTER    ==     [a-zA-Z]*/
NUMBER         [[:digit:]]
/*NUMBER    ==     [0-9]*/
SPECIAL        [-\[\]\\`^{}]
NICK           {LETTER}([{LETTER}{NUMBER}{SPECIAL}])*
ORIGIN         {SERVERNAME}|{NICK}
/* *NOTE*: in order to pickup the hostname in a sequence "!user[@host] ",
           disallow "@" within a username... */
USER           ([\x00-\xFF]{-}[ \t\0\r\n@])+
/* NONWHITE       [\x00-\xFF]{-}[ \t\0\r\n] */
/* USER           {NONWHITE}+ */
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
%s             end_of_frame

%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

/* *NOTE*: it seems that the following "exceptions" are allowed:
           - the trailing param does NOT need a ':' prefix IF it doesn't contain
             <SPACE>s...
*/

%%
%{
  yylloc->step();
  typedef yy::RPG_Net_Protocol_IRCParser::token token;
  typedef yy::RPG_Net_Protocol_IRCParser::token_type token_type;
  yy_flex_debug=driver.getDebugScanner();
  std::stringstream converter;
%}

<INITIAL>{
":"                        { yylloc->step();
                             BEGIN(prefix);
                             return token_type(yytext[0]); }
"\r"                       { yylloc->step();
                             BEGIN(end_of_frame); }
} // end <INITIAL>
<prefix>{
{SPACE}+                   { // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput(' ');
                               return token::ORIGIN;
                              } // end IF
                             yylloc->step();
                             BEGIN(command);
                             //yylval->ival = yyleng;
                             return token::SPACE; }
"!"                        { // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput('!');
                               return token::ORIGIN;
                             } // end IF
                             yylloc->step();
                             BEGIN(user);
                             return token_type(yytext[0]); }
"@"                        { // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput('@');
                               return token::ORIGIN;
                             } // end IF
                             yylloc->step();
                             BEGIN(host);
                             return token_type(yytext[0]); }
{ORIGIN}/{SPACE}           { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);

                             return token::ORIGIN; }
{ORIGIN}/"!"               { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::ORIGIN; }
{ORIGIN}/"@"               { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::ORIGIN; }
} // end <prefix>
<user>{
{USER}/{SPACE}             { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::USER; }
{USER}/"@"                 { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::USER; }
} // end <user>
<host>{
{HOST}/{SPACE}             { yylloc->step();
                             BEGIN(prefix);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::HOST; }
} // end <host>
<INITIAL,command>{
{NUMBER}+/{SPACE}          { yylloc->step();
                             BEGIN(params);
                             memory.append(yytext, yyleng);
                             converter.str(ACE_TEXT_ALWAYS_CHAR(""));
                             converter.clear();
                             converter << memory;
                             converter >> yylval->ival;
                             memory.clear();
                             return token::CMD_NUMERIC; }
{LETTER}+/{SPACE}          { yylloc->step();
                             BEGIN(params);
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::CMD_STRING; }
} // end <INITIAL, command>
<params>{
{SPACE}+                   { // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput(' ');
                               return token::PARAM;
                             } // end IF
                             yylloc->step();
                             //yylval->ival = yyleng;
                             return token::SPACE; }
":"                        { // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput(yytext[0]);
                               return token::PARAM;
                             } // end IF
                             yylloc->step();
                             BEGIN(trailing);
                             return token_type(yytext[0]); }
"\r"                       { // handle back-up case... /* *NOTE*: non-compliant (see above) */
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               unput(yytext[0]);
                               return token::PARAM;
                             } // end IF
                             yylloc->step();
                             BEGIN(end_of_frame);
                             return token_type(yytext[0]); }
{PARAM}/{SPACE}            { yylloc->step();
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::PARAM; }
{PARAM}/"\r"               { yylloc->step(); /* *NOTE*: non-compliant (see above) */
                             ACE_NEW_NORETURN(yylval->sval,
                                              std::string);
                             if (!memory.empty())
                             {
                               *(yylval->sval) = memory;
                               memory.clear();
                             } // end IF
                             yylval->sval->append(yytext, yyleng);
                             return token::PARAM; }
} // end <params>
<trailing>{
"\r"                       { yylloc->step();
                             // handle back-up case...
                             if (!memory.empty())
                             {
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               *(yylval->sval) = memory;
                               memory.clear();
                               BEGIN(end_of_frame);
                               return token::PARAM;
                             } // end IF
                             BEGIN(end_of_frame); }
{TRAILING_PARAM}/"\r"      { if (yyleng > 0)
                             {
                               yylloc->step();
                               ACE_NEW_NORETURN(yylval->sval,
                                                std::string);
                               if (!memory.empty())
                               {
                                 *(yylval->sval) = memory;
                                 memory.clear();
                               } // end IF
                               yylval->sval->append(yytext, yyleng);
                               return token::PARAM;
                             } // end IF
                           }
} // end <trailing>
<end_of_frame>{
"\n"                       { yylloc->step();
                             BEGIN(INITIAL);
                             messageCount++; }
}
<<EOF>>                    { if (!driver.moreData())
                               yyterminate();

                             if (yyleng > 0)
                               memory = yytext;
                             else
                               memory.clear();

                             if (!driver.switchBuffer())
                             {
                               ACE_DEBUG((LM_ERROR,
                                          ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::switchBuffer(), aborting\n")));

                               // debug info
                               std::string error_message(ACE_TEXT("invalid data (was \""));
                               error_message += memory;
                               if (yyleng > 0)
                               {
                                 converter.str(ACE_TEXT(""));
                                 converter << ACE_static_cast(unsigned long, yytext[0]);
                                 error_message += ACE_TEXT("\" - ");
                                 error_message += converter.str();
                               } // end IF
                               else
                                 error_message += ACE_TEXT("\"");
                               error_message += ACE_TEXT("), continuing");
                               driver.error(*yylloc, error_message);

                               yyterminate();
                             } // end IF
                           }
<*>(?s:.)                  { yylloc->step();
                             // *NOTE*: this introduces the back-up case...
                             yymore(); }
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
//
//   RPG_Net_Protocol_IRCParserDriver* driver = IRCScannerget_extra(yyscanner);
//   // sanity check
//   ACE_ASSERT(driver);
//   if (!driver->moreData())
//     return 1;
//
//   // *NOTE*: there SHOULD be more data in a
//   // continuation, so:
//   // 1. gobble/save the rest
//   // 2. switch buffers
//   // 3. unput the rest
//   // 4. continue matching
//
//   // step1
//   std::string the_rest;
//   the_rest += yytext[0];
//   for (char c = yyinput(yyscanner);
//        c != EOF;
//        c = yyinput(yyscanner))
//     the_rest += c;
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("the rest: \"%s\"\n"),
//              the_rest.c_str()));
//
//   // step2
//   if (!driver->switchBuffer())
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::switchBuffer() [data was: \"%s\"], aborting\n"),
//                the_rest.c_str()));
//
//     return 1;
//   } // end IF
//
//   // step3
//   for (std::string::reverse_iterator iterator = the_rest.rbegin();
//        iterator != the_rest.rend();
//        iterator++)
//     unput(*iterator);
//
//   // step4
//  // yymore();
//
//   return 0;
// }
// #ifdef __cplusplus
// }
// #endif /* __cplusplus */
