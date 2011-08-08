%{
#include <ace/Log_Msg.h>
%}

%option pointer full nounistd 8bit
/* %option c++ outfile="rpg_net_protocol_IRCbisect.cpp" prefix="IRCBisect" */
%option warn nodefault noyywrap nounput batch nomain
%option debug perf-report verbose
/* %option   outfile="RPG_Net_Protocol_IRCScanner.cpp" header-file="RPG_Net_Protocol_IRCScanner.h" */
%option reentrant extra-type="unsigned long*"
/* %option bison-bridge bison-locations stack */
%option prefix="IRCBisect" yylineno

/* *NOTE*: "\r\n" is (ANSI-C) implementation-dependent --> use ASCII codes directly */
CRLF    \x0D\x0A
MESSAGE ([\x00-\xFF]{-}[\x0D\x0A])+

%s      end_of_frame

%%
<INITIAL>{
"\x0D"                 { BEGIN(end_of_frame); }
{MESSAGE}              { return yyleng; }
(?s:.)                 {
                         ACE_DEBUG((LM_ERROR,
                                    ACE_TEXT("ignoring invalid character: \"%c\" (%d)\n"),
                                    yytext[0],
                                    static_cast<int>(yytext[0])));
                       }
} // end <INITIAL>
<end_of_frame>{
"\x0A"                 { BEGIN(INITIAL);
                         (*yyextra)++;
                         return -1; }
(?s:.)                 { BEGIN(INITIAL);
                         yymore(); }
} // end <end_of_frame>
<<EOF>>                { return 0; }
%%
