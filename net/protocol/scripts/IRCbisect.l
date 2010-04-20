%{
#include <stdio.h>
#include <string>

#include <ace/Log_Msg.h>

#include "rpg_net_protocol_module_IRCsplitter.h"
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCbisect.cpp" prefix="IRCBisect" */
%option warn nodefault noyywrap nounput
%option debug perf-report verbose
/* %option   outfile="RPG_Net_Protocol_IRCScanner.cpp" header-file="RPG_Net_Protocol_IRCScanner.h" */
%option reentrant extra-type="unsigned long*"
/* %option bison-bridge bison-locations stack */
%option yylineno

CRLF     \r\n
MESSAGE  [(?s:.)]*

%s    end_of_message

%%
<INITIAL>{MESSAGE}/{CRLF}     { BEGIN(end_of_message);
                                return yyleng; }
<end_of_message>{CRLF}        { BEGIN(INITIAL);
                                (*yyextra)++; }
<<EOF>>                       { return 0; }
(?s:.)                        { ACE_DEBUG((LM_ERROR,
                                           ACE_TEXT("invalid character, continuing\n"))); }
%%

const bool
RPG_Net_Protocol_Module_IRCSplitter::scan_begin(char* data_in,
                                                const size_t& length_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState == NULL);

//  yy_flex_debug = myTraceScanning;

  // create/init a new buffer state
  // *WARNING*: length_in IS already adjusted for two trailing \0's
  myCurrentState = yy_scan_buffer(data_in,
                                  length_in,
                                  myScannerContext);
  if (myCurrentState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::yy_scan_buffer(%@, %d), aborting\n"),
               data_in,
               length_in));

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
RPG_Net_Protocol_Module_IRCSplitter::scan_end()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCSplitter::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentState);

  // clean state
  yy_delete_buffer(myCurrentState,
                   myScannerContext);
  myCurrentState = NULL;

//  fclose(yyin);
}
