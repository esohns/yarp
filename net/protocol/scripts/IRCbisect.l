%{
#include <string>

#include <ace/Log_Msg.h>

#include "rpg_net_protocol_module_IRCsplitter.h"

/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
   not conform to C89.  See Debian bug 333231
   <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef yywrap
#define yywrap() 1

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type.  */
#define yyterminate() return token::END
%}

%option pointer full nounistd 8bit stack
/* %option c++ outfile="rpg_net_protocol_IRCbisect.cpp" prefix="IRCBisect" */
%option warn nodefault noyywrap nounput
%option debug perf-report verbose
/* %option   outfile="RPG_Net_Protocol_IRCScanner.cpp" header-file="RPG_Net_Protocol_IRCScanner.h" */
%option bison-bridge bison-locations reentrant extra-type="unsigned long*"
%option yylineno

CRLF     '\r''\n'
MESSAGE  [(?s:.)]*

%s    end_of_message

%%
<INITIAL>{MESSAGE}/{CRLF}     { BEGIN(end_of_message);
                                (*yyextra)++;
                                return yyleng; }
<end_of_message>{CRLF}        { BEGIN(INITIAL);
                                return 0; }
%%

const bool
RPG_Net_Protocol_Module_IRCSplitter::scan_begin(const char* data_in,
                                                const size_t& length_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState == NULL);

  yy_flex_debug = myTraceScanning;

  // create/init a new buffer state
  // *WARNING*: length_in IS already adjusted for two trailing \0's
  myCurrentState = yy_scan_buffer(data_in, length_in);
  if (myCurrentState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::yy_scan_buffer(%@, %d), aborting\n"),
               data_in,
               length_in));

    return false;
  } // end IF

//   if (file == "-")
//     yyin = stdin;
//   else if (!(yyin = fopen(file.c_str (), "r")))
//   {
//     error(std::string("cannot open ") + file);
//     exit(1);
//   }
}

void
RPG_Net_Protocol_Module_IRCSplitter::scan_end()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState);

  // clean state
  yy_delete_buffer(myCurrentState);
  myCurrentState = NULL;

//  fclose(yyin);
}
