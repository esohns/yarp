%{
#include <stdio.h>
#include <string>
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCbisect.cpp" prefix="IRCBisect" */
%option warn nodefault noyywrap nounput
%option debug perf-report verbose
/* %option   outfile="RPG_Net_Protocol_IRCScanner.cpp" header-file="RPG_Net_Protocol_IRCScanner.h" */
%option reentrant extra-type="unsigned long*"
/* %option bison-bridge bison-locations stack */
%option yylineno

MESSAGE  [(?s:.)]*
CRLF     \r\n

%s       end_of_message

%%
<INITIAL>{MESSAGE}/{CRLF}     { BEGIN(end_of_message);
                                return yyleng; }
<end_of_message>{CRLF}        { BEGIN(INITIAL);
                                (*yyextra)++;
                                return -1; }
<<EOF>>                       { return 0; }
<*>(?s:.)                     { return 1; }
%%
