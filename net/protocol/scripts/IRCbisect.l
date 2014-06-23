%{
#include "stdafx.h"

#include <ace/Log_Msg.h>
%}

%option pointer 8bit reentrant never-interactive nounistd
/* %option c++ yyclass="RPG_Net_Protocol_IRCBisect" stack noline bison-bridge bison-locations */
%option warn batch nomain noyywrap nounput nodefault
%option debug perf-report verbose yylineno ansi-definitions ansi-prototypes
%option full backup perf-report verbose
%option outfile="rpg_net_protocol_IRCbisect.cpp" header-file="rpg_net_protocol_IRCbisect.h" prefix="RPG_Net_Protocol_IRCBisect_"

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
                         /* (*yyextra)++; */
                         return -1; }
(?s:.)                 { BEGIN(INITIAL);
                         yymore(); }
} // end <end_of_frame>
<<EOF>>                { return 0; }
%%
